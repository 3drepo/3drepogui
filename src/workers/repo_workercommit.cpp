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

#include "repo_workercommit.h"

repo::gui::RepoWorkerCommit::RepoWorkerCommit(
    const core::MongoClientWrapper &mongo,
    const QString &repositoryName,
    const repo::core::RepoGraphHistory *history,
	const repo::core::RepoGraphScene *scene)
	: mongo(mongo) 
	, repositoryName(repositoryName)
	, history(history)
	, scene(scene)
{}

repo::gui::RepoWorkerCommit::~RepoWorkerCommit() {}

void repo::gui::RepoWorkerCommit::run() 
{
    std::string dbName = repositoryName.toStdString();
    if (!cancelled && mongo.reconnectAndReauthenticate(dbName))
    {
        // TODO: only get those nodes that are mentioned in the revision object.
        std::set<const core::RepoNodeAbstract *> nodes = scene->getNodesRecursively();

        // TODO: remove
        std::set<core::RepoNodeAbstract *> tester = scene->getNodes();
        if (nodes.size() != tester.size())
        {
            std::cerr << "Nodes difference recursively: " << nodes.size();
            std::cerr << ", set: " << tester.size() << std::endl;
        }
        core::RepoNodeRevision *revision = history->getCommitRevision();
        int jobsCount = nodes.size() + 1; // +1 for revision entry
        if (revision && nodes.size() > 0)
        {
            //------------------------------------------------------------------
            // Start
            emit progress(0, 0);


            int counter = 0;

            //------------------------------------------------------------------
            // Insert the revision object first in case of a lost connection.
            mongo.insertRecord(dbName, REPO_COLLECTION_HISTORY, revision->toBSONObj());
            emit progress(++counter, jobsCount);


            std::set<const core::RepoNodeAbstract *>::iterator it;
            //------------------------------------------------------------------
            // Insert new records one-by-one

            for (it = nodes.begin(); it != nodes.end(); ++it)
            {
                const core::RepoNodeAbstract *node = *it;
                mongo::BSONObj nodeObj = node->toBSONObj();
                if (nodeObj.objsize() > 16777216) // 16MB
                    std::cerr << "Node '" << node->getName() << "' over 16MB in size is not committed." << std::endl;
                else
                    mongo.insertRecord(dbName, REPO_COLLECTION_SCENE, nodeObj);
                emit progress(++counter, jobsCount);
            }
            //------------------------------------------------------------------
            // End
            emit progress(jobsCount, jobsCount);
        }
    }
    //--------------------------------------------------------------------------
    // Done
    emit RepoWorkerAbstract::finished();
}


QString repo::gui::RepoWorkerCommit::sanitizeDatabaseName(const QString &dbName)
{
    QString sanitized = dbName;
    // MongoDB cannot have dots in database names, hence replace with underscores
    // (and remove file extension if any)
    // http://docs.mongodb.org/manual/reference/limits/#naming-restrictions

    sanitized.replace(".", "_");
    sanitized.replace(" ", "_");

    // MongoDB db name can only have fewer than 64 chars
    if (sanitized.size() > 63)
        sanitized.resize(63);

    return sanitized;
}
