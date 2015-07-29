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
    const QString &project,
    const QUuid& id,
    bool headRevision)
	: mongo(mongo)
	, database(database.toStdString())
    , project(project.toStdString())
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
    core::RepoGraphScene *masterSceneGraph = 0;
    core::RepoGraphScene *optimizedMasterSceneGraph = 0;

    try
    {
        if (!cancelled && !mongo.reconnect())
        {
            std::cerr << "Connection failed" << std::endl;
        }
        else
        {
             fetchSceneRecursively(
                database,
                project,
                id.toString().toStdString(),
                headRevision,
                masterSceneGraph,
                optimizedMasterSceneGraph,
                NULL);

            //----------------------------------------------------------------------
            // Convert to Assimp
            // TODO: code in direct conversion from RepoSceneGraph to GLC_World
            // to avoid intermediary Assimp aiScene conversion!
            aiScene* scene = new aiScene();
            if (!cancelled && optimizedMasterSceneGraph)
            {
                optimizedMasterSceneGraph->toAssimp(scene);
                emit progress(done++, jobsCount);

                //------------------------------------------------------------------
                // Convert raw textures into QImages
                std::map<std::string, QImage> namedTextures;
                std::vector<repo::core::RepoNodeTexture*> textures =
                        optimizedMasterSceneGraph->getTextures();
                for (unsigned int i = 0; !cancelled && i < textures.size(); ++i)
                {
                    repo::core::RepoNodeTexture* repoTex = textures[i];
                    const unsigned char* data = (unsigned char*) repoTex->getRawData();
                    QImage image = QImage::fromData(data, repoTex->getRawDataSize());
                    namedTextures.insert(std::make_pair(repoTex->getName(), image));
                }
                emit progress(done++, jobsCount);

                //------------------------------------------------------------------
                // GLC World conversion
                if (!cancelled)
                    glcWorld = repo::gui::RepoTranscoderAssimp::toGLCWorld(scene, namedTextures);
            }
        }
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    //--------------------------------------------------------------------------
	emit progress(jobsCount, jobsCount);
    //--------------------------------------------------------------------------
    emit finished(masterSceneGraph, optimizedMasterSceneGraph, glcWorld);
	emit RepoWorkerAbstract::finished();
}

void repo::gui::RepoWorkerFetchRevision::fetchSceneRecursively(
        const std::string &database,
        const string &project,
        const std::string &uuid,
        bool isHeadRevision,
        core::RepoGraphScene *&masterSceneGraph,
        core::RepoGraphScene *&optimizedMasterSceneGraph,
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
    // (eg as meshes popping up in 3drepo.io)

    //--------------------------------------------------------------------------
    // First load revision object
    // a) by its SID if head revision (latest according to timestamp)
    // b) by its UID if not head revision
    std::list<std::string> fieldsToReturn;
    fieldsToReturn.push_back(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS);


    // TODO: make this adhere to the revision history graph so that it does not
    // rely on timestamps!
    mongo::BSONObj revisionBSON = isHeadRevision
        ? mongo.findOneBySharedID(
            database,
            core::MongoClientWrapper::getHistoryCollectionName(project),
            uuid,
            REPO_NODE_LABEL_TIMESTAMP,
            fieldsToReturn)
        : mongo.findOneByUniqueID(
            database,
            core::MongoClientWrapper::getHistoryCollectionName(project),
            uuid,
            fieldsToReturn);

    std::string revisionID = repo::core::RepoTranscoderString::toString(repo::core::RepoTranscoderBSON::retrieve(revisionBSON.getField(REPO_NODE_LABEL_ID)));

    fieldsToReturn.clear();
    fieldsToReturn.push_back(REPO_NODE_LABEL_ID);

    // See if there is a stashed optimized version available
    mongo::BSONObj stash = mongo.findOneByRevID(
        database,
        core::MongoClientWrapper::getRepoStashCollectionName(project),
        revisionID,
        REPO_NODE_LABEL_ID,
        fieldsToReturn);


    // -------------------------------------------------------------------------------------
    // First load the unoptimized scene for the tree
    // -------------------------------------------------------------------------------------

    mongo::BSONArray array = mongo::BSONArray(revisionBSON.getObjectField(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS));
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
                    core::MongoClientWrapper::getSceneCollectionName(project),
                    array,
                    retrieved);
        }
        while (!cancelled && cursor.get() && cursor->more());
    }
    else
    {
        std::cerr << "Deprecated DB retrieval" << std::endl;
        mongo.fetchEntireCollection(database,  core::MongoClientWrapper::getSceneCollectionName(project), data);
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


    // -------------------------------------------------------------------------------------
    // Then load the optimized scene
    // -------------------------------------------------------------------------------------

    if (!stash.isEmpty())
    {
        data.clear();

        //----------------------------------------------------------------------
        emit progress(done++, jobsCount);
        //----------------------------------------------------------------------

        if (!cancelled && fieldsCount > 0)
        {
            jobsCount += fieldsCount; // This is the upper bound on the optimized graph
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
                    cursor = mongo.findAllByRevID(
                        database,
                        core::MongoClientWrapper::getRepoStashCollectionName(project),
                        revisionID,
                        retrieved);
            }
            while (!cancelled && cursor.get() && cursor->more());
        }
        else
        {
            std::cerr << "Deprecated DB retrieval" << std::endl;
            mongo.fetchEntireCollection(database,  core::MongoClientWrapper::getSceneCollectionName(project), data);
        }

        //----------------------------------------------------------------------
        emit progress(done++, jobsCount);
        //----------------------------------------------------------------------

        // Convert to Repo scene graph
        if (!cancelled)
        {
            core::RepoGraphScene *childSceneGraph = new core::RepoGraphScene(data);
            emit progress(done++, jobsCount);

            //----------------------------------------------------------------------
            if (childSceneGraph)
            {
                if (optimizedMasterSceneGraph == NULL)
                    optimizedMasterSceneGraph = childSceneGraph;
                else
                {
                    // Append child scene graph to the master graph, this
                    // clears the childSceneGraph blank
                    optimizedMasterSceneGraph->append(referenceNode, childSceneGraph);

                    // Delete the remaining empty graph.
                    delete childSceneGraph;
                }
            }
            emit progress(done++, jobsCount);
        }
    } else {
        // If the stash doesn't exist then send the unoptimized graph
        optimizedMasterSceneGraph = masterSceneGraph; 
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
                    database,
                    reference->getProject(),
                    refUuuid,
                    !reference->getIsUniqueID(),
                    masterSceneGraph,
                    optimizedMasterSceneGraph,
                    reference);

        emit progress(done++, jobsCount);
    }
}
