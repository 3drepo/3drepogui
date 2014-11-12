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

#include "repo_workerfetchrevision.h"
//------------------------------------------------------------------------------
#include "graph/repo_node_revision.h"
#include "graph/repo_graph_history.h"
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
	int jobsCount = 6;
	int done = 0;
	emit progress(0, 0); // undetermined (moving) progress bar

	core::RepoGraphScene* repoGraphScene;
	GLC_World glcWorld;

	if (!cancelled && !mongo.reconnect())
    {
        std::cerr << "Connection failed" << std::endl;
    }
	else
	{
		mongo.reauthenticate(database);

		//-------------------------------------------------------------------------
		// Fetch data from DB
		std::vector<mongo::BSONObj> data;

		// TODO: fetch transformations first to build the scene
		// and reconstruct meshes later so as to give visual feedback to the user immediatelly
		// (eg as meshes popping up in XML3DRepo)

		//---------------------------------------------------------------------------
		// First load revision object 
		// a) by its SID if head revision (latest according to timestamp)
		// b) by its UID if not head revision
		std::list<std::string> fieldsToReturn;
		fieldsToReturn.push_back(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS);
		
		std::string idString = id.toString().toStdString();
		mongo::BSONObj bson = headRevision 
			? mongo.findOneBySharedID(
				database, 
				REPO_COLLECTION_HISTORY, 
				idString, 
				REPO_NODE_LABEL_TIMESTAMP, 
				fieldsToReturn)
			: mongo.findOneByUniqueID(
				database, 
				REPO_COLLECTION_HISTORY, 
				idString, 
				fieldsToReturn);
		mongo::BSONArray array = mongo::BSONArray(bson.getObjectField(REPO_NODE_LABEL_CURRENT_UNIQUE_IDS));
		//---------------------------------------------------------------------
		emit progress(done++, jobsCount);
		//---------------------------------------------------------------------
		int fieldsCount = array.nFields();
		if (fieldsCount > 0)
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
            mongo.fetchEntireCollection(database, REPO_COLLECTION_SCENE, data);
		}
		//---------------------------------------------------------------------
		emit progress(done++, jobsCount);
		//---------------------------------------------------------------------
		// Convert to Repo scene graph
		if (!cancelled)
		{
			repoGraphScene = new core::RepoGraphScene(data);
			emit progress(done++, jobsCount);
		}

		//---------------------------------------------------------------------
		// Convert to Assimp
		// TODO: code in direct conversion from RepoSceneGraph to GLC_World
		// to avoid intermediary Assimp aiScene conversion!
		aiScene* scene = new aiScene();
		if (!cancelled)
		{
			repoGraphScene->toAssimp(scene);
			emit progress(done++, jobsCount);
		}

		//---------------------------------------------------------------------
		// Convert raw textures into QImages
		std::map<std::string, QImage> namedTextures;
		std::vector<repo::core::RepoNodeTexture*> textures = repoGraphScene->getTextures();
		for (unsigned int i = 0; !cancelled && i < textures.size(); ++i)
		{
			repo::core::RepoNodeTexture* repoTex = textures[i];
			const unsigned char* data = (unsigned char*) repoTex->getData();
			QImage image = QImage::fromData(data, repoTex->getDataSize());
			namedTextures.insert(std::make_pair(repoTex->getName(), image));
		}
		emit progress(done++, jobsCount);

		//---------------------------------------------------------------------
		// GLC World conversion
		if (!cancelled)
			glcWorld = repo::gui::RepoTranscoderAssimp::toGLCWorld(scene, namedTextures);
	}

	//-------------------------------------------------------------------------
	emit progress(jobsCount, jobsCount);
	//-------------------------------------------------------------------------
	// Done
	emit finished(repoGraphScene, glcWorld);
	emit RepoWorkerAbstract::finished();
}
