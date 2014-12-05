/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//------------------------------------------------------------------------------
// GUI
#include "repo_workerfetchrevision.h"
//------------------------------------------------------------------------------
// Core
#include <RepoNodeAbstract>
#include <RepoNodeRevision>
#include <RepoNodeReference>
#include <RepoGraphHistory>
#include <RepoTranscoderString>
//------------------------------------------------------------------------------

repo::gui::RepoWorkerFetchRevision::RepoWorkerFetchRevision(
    const repo::core::MongoClientWrapper &mongo,
	const QString& database,
	const QUuid& id,
	bool headRevision)
	: mongo(mongo)
	, database(database.toStdString())
	, id(id)
	, headRevision(headRevision)
{}

repo::gui::RepoWorkerFetchRevision::~RepoWorkerFetchRevision() {}

void repo::gui::RepoWorkerFetchRevision::run()
{
	//-------------------------------------------------------------------------
	// Start
    jobsCount = 3;
    done = 0;
	emit progress(0, 0); // undetermined (moving) progress bar


	GLC_World glcWorld;
    core::RepoGraphScene *masterSceneGraph = NULL;
	if (!cancelled && !mongo.reconnect())
    {
        std::cerr << "Connection failed" << std::endl;
    }
	else
	{
         masterSceneGraph = fetchSceneRecursively(
            database,
            id.toString().toStdString(),
            headRevision,
            NULL,
            NULL);

        //----------------------------------------------------------------------
		// Convert to Assimp
		// TODO: code in direct conversion from RepoSceneGraph to GLC_World
		// to avoid intermediary Assimp aiScene conversion!
		aiScene* scene = new aiScene();
        if (!cancelled && masterSceneGraph)
		{
            masterSceneGraph->toAssimp(scene);
			emit progress(done++, jobsCount);

            //------------------------------------------------------------------
            // Convert raw textures into QImages
            std::map<std::string, QImage> namedTextures;
            std::vector<repo::core::RepoNodeTexture*> textures =
                    masterSceneGraph->getTextures();
            for (unsigned int i = 0; !cancelled && i < textures.size(); ++i)
            {
                repo::core::RepoNodeTexture* repoTex = textures[i];
                const unsigned char* data = (unsigned char*) repoTex->getData();
                QImage image = QImage::fromData(data, repoTex->getDataSize());
                namedTextures.insert(std::make_pair(repoTex->getName(), image));
            }
            emit progress(done++, jobsCount);

            //------------------------------------------------------------------
            // GLC World conversion
            if (!cancelled)
                glcWorld = repo::gui::RepoTranscoderAssimp::toGLCWorld(scene, namedTextures);
        }
	}

    //--------------------------------------------------------------------------
	emit progress(jobsCount, jobsCount);
    //--------------------------------------------------------------------------
    emit finished(masterSceneGraph, glcWorld);
	emit RepoWorkerAbstract::finished();
}

repo::core::RepoGraphScene * repo::gui::RepoWorkerFetchRevision::fetchSceneRecursively(
        const std::string &database,
        const std::string &uuid,
        bool isHeadRevision,
        core::RepoGraphScene *masterSceneGraph,
        core::RepoNodeReference *referenceNode)
{
    std::cout << "Fetching: " << database << " " << uuid << std::endl;
    jobsCount += 4;

    mongo.reauthenticate(database);
    //--------------------------------------------------------------------------
    // Fetch data from DB
    std::vector<mongo::BSONObj> data;
    // TODO: fetch transformations first to build the scene
    // and reconstruct meshes later so as to give visual feedback to the user immediatelly
    // (eg as meshes popping up in XML3DRepo)

    //--------------------------------------------------------------------------
    // First load revision object
    // a) by its SID if head revision (latest according to timestamp)
    // b) by its UID if not head revision
    std::list<std::string> fieldsToReturn;
    fieldsToReturn.push_back(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS);


    // TODO: make this adhere to the revision history graph so that it does not
    // rely on timestamps!
    mongo::BSONObj bson = isHeadRevision
        ? mongo.findOneBySharedID(
            database,
            REPO_COLLECTION_HISTORY,
            uuid,
            REPO_NODE_LABEL_TIMESTAMP,
            fieldsToReturn)
        : mongo.findOneByUniqueID(
            database,
            REPO_COLLECTION_HISTORY,
            uuid,
            fieldsToReturn);

   // std::cout << bson.toString(false, true) << std::endl;
    mongo::BSONArray array = mongo::BSONArray(bson.getObjectField(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS));
    //----------------------------------------------------------------------
    emit progress(done++, jobsCount);
    //----------------------------------------------------------------------
    int fieldsCount = array.nFields();
    if (!cancelled && fieldsCount > 0)
    {
        jobsCount += fieldsCount;
        unsigned long long retrieved = 0;
        std::auto_ptr<mongo::DBClientCursor> cursor;
        do
        {
            for (; !cancelled && cursor.get() && cursor->more(); ++retrieved)
            {
                data.push_back(cursor->nextSafe().copy());
                emit progress(done++, jobsCount);
            }
            if (!cancelled)
                cursor = mongo.findAllByUniqueIDs(
                    database,
                    REPO_COLLECTION_SCENE,
                    array,
                    retrieved);
        }
        while (!cancelled && cursor.get() && cursor->more());
    }
    else
    {
        std::cerr << "Deprecated DB retrieval" << std::endl;
        mongo.fetchEntireCollection(database, REPO_COLLECTION_SCENE, data);
    }
    //----------------------------------------------------------------------

    emit progress(done++, jobsCount);

    //----------------------------------------------------------------------
    // Convert to Repo scene graph
    std::vector<core::RepoNodeAbstract *> references;
    if (!cancelled)
    {
        core::RepoGraphScene *childSceneGraph = new core::RepoGraphScene(data);
        emit progress(done++, jobsCount);

        //----------------------------------------------------------------------
        if (childSceneGraph)
        {
            references = childSceneGraph->getReferences();
            if (masterSceneGraph == NULL)
                masterSceneGraph = childSceneGraph;
            else
            {
                // Append child scene graph to the master graph, this
                // clears the childSceneGraph blank
                masterSceneGraph->append(referenceNode, childSceneGraph);

                // Delete the remaining empty graph.
                delete childSceneGraph;
            }
            jobsCount += references.size();
            std::cout << "Found " << references.size() << " references" << std::endl;
        }
        emit progress(done++, jobsCount);
    }


    //--------------------------------------------------------------------------
    //
    // FEDERATION
    //
    //--------------------------------------------------------------------------
    // Fetch references if any
    for (unsigned int i = 0; !cancelled && i < references.size(); ++i)
    {
        core::RepoNodeReference *reference = (core::RepoNodeReference *) references[i];
        // TODO: modify mongowrapper to take into account reference owner and project!

        // Recursion
        std::string refUuuid = core::RepoTranscoderString::toString(reference->getRevisionID());
        fetchSceneRecursively(
                    reference->getProject(),
                    refUuuid,
                    !reference->getIsUniqueID(),
                    masterSceneGraph,
                    reference);

        emit progress(done++, jobsCount);
    }

    return masterSceneGraph;
}
