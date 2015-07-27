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
    const QString &database,
    const QString &project,
    const core::RepoNodeRevision *revision,
    const core::RepoNodeAbstractSet &nodes,
    const core::RepoNodeAbstractSet &nodesOptim)
    : mongo(mongo)
    , database(sanitizeDatabaseName(database).toStdString())
    , project(sanitizeCollectionName(project).toStdString())
    , revision(revision)
    , nodes(nodes)
    , nodesOptim(nodesOptim)
{}

repo::gui::RepoWorkerCommit::~RepoWorkerCommit() {}

void repo::gui::RepoWorkerCommit::run()
{
    std::cout << tr("Uploading, please wait...").toStdString() << std::endl;
    int jobsCount = 0;
    try
    {
        if (!cancelled && mongo.reconnectAndReauthenticate(database))
        {
            jobsCount = nodes.size();
            jobsCount = nodesOptim.size();
            jobsCount++; // +1 for revision entry and +1 project settings
            jobsCount++;
            if (revision && nodes.size() > 0)
            {
                //--------------------------------------------------------------
                // Start
                emit progress(0, 0);
                int counter = 0;
                std::string historyCollection =
                        core::MongoClientWrapper::getHistoryCollectionName(project);
                std::string sceneCollection =
                        core::MongoClientWrapper::getSceneCollectionName(project);
				std::string stashCollection =
						core::MongoClientWrapper::getRepoStashCollectionName(project);

                //--------------------------------------------------------------
                // Insert project settings object first in case of a lost connection
                core::RepoProjectSettings projectSettings =
                                core::RepoProjectSettings(
                                    project,
                                    mongo.getUsername(database));
                // TODO: check if it already exists (eg use insert instead of upsert as this is unsafe!)
                mongo.runCommand(database, projectSettings.upsert());

                //--------------------------------------------------------------
                // Insert the revision object first in case of a lost connection.
                mongo.insertRecord(database, historyCollection, revision->toBSONObj());
                emit progress(++counter, jobsCount);

                core::RepoNodeAbstractSet::iterator it;
                //--------------------------------------------------------------
                // Insert new records one-by-one
                for (it = nodes.begin(); it != nodes.end(); ++it)
                {
                    const core::RepoNodeAbstract *node = *it;
                    mongo::BSONObj nodeObj = node->toBSONObj();
                    if (nodeObj.objsize() > 16777216) // 16MB
                        std::cerr << "Node '" << node->getName() << "' over 16MB in size is not committed." << std::endl;
                    else
                        mongo.insertRecord(database, sceneCollection, nodeObj);
                    emit progress(++counter, jobsCount);
                }

                //--------------------------------------------------------------
                // Insert optimized graph records one-by-one
                for (it = nodesOptim.begin(); it != nodesOptim.end(); ++it)
                {
                    const core::RepoNodeAbstract *node = *it;
                    (*it)->setRevisionID(revision->getUniqueID());

                    mongo::BSONObj nodeObj = node->toBSONObj();

                    if (nodeObj.objsize() > 16777216) // 16MB
                        std::cerr << "Node '" << node->getName() << "' over 16MB in size is not committed." << std::endl;
                    else
                        mongo.insertRecord(database, stashCollection, nodeObj);
                    emit progress(++counter, jobsCount);
                }

               }
            }
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    //--------------------------------------------------------------------------
    // End
    emit progress(jobsCount, jobsCount);
    //--------------------------------------------------------------------------
    // Done
    emit RepoWorkerAbstract::finished();
}

// TODO: move to CORE
QString repo::gui::RepoWorkerCommit::sanitizeDatabaseName(const QString &database)
{
    QString sanitized = database;
    // MongoDB cannot have dots in database names, hence replace with underscores
    // (and remove file extension if any)
    // http://docs.mongodb.org/manual/reference/limits/#naming-restrictions

    // Cannot contain any of /\. "$*<>:|?
    sanitized.replace("/", "_");
    sanitized.replace("\\", "_");
    sanitized.replace(".", "_");
    sanitized.replace(" ", "_");
    sanitized.replace("\"", "_");
    sanitized.replace("$", "_");
    sanitized.replace("*", "_");
    sanitized.replace("<", "_");
    sanitized.replace(">", "_");
    sanitized.replace(":", "_");
    sanitized.replace("|", "_");
    sanitized.replace("?", "_");

    // MongoDB db name can only have fewer than 64 chars
    if (sanitized.size() > 63)
        sanitized.resize(63);

    return sanitized;
}

// TODO: move to CORE
QString repo::gui::RepoWorkerCommit::sanitizeCollectionName(const QString& collection)
{
    // http://docs.mongodb.org/manual/reference/limits/#Restriction-on-Collection-Names
    QString sanitized = collection;
    sanitized.replace(" ", "_");
    sanitized.replace("$", "_");
    sanitized.replace("system.", "_");
    return sanitized;
}
