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

#include "repo_workerusers.h"
//------------------------------------------------------------------------------
// Core
#include <RepoCoreGlobal>
#include <RepoUser>
#include <RepoRole>

//------------------------------------------------------------------------------
repo::gui::RepoWorkerUsers::RepoWorkerUsers(
    const repo::core::MongoClientWrapper &mongo,
    const std::string &database,
    const core::RepoBSON &command)
    : mongo(mongo)
    , database(database)
    , command(command)
{
    qRegisterMetaType<core::RepoUser>("core::RepoUser");
    qRegisterMetaType<std::list<std::string> >("std::list<std::string>");
    qRegisterMetaType<std::map<std::string, std::list<std::string> > >("std::map<std::string, std::list<std::string> >");
}

//------------------------------------------------------------------------------

repo::gui::RepoWorkerUsers::~RepoWorkerUsers() {}

//------------------------------------------------------------------------------

void repo::gui::RepoWorkerUsers::run()
{
    int jobsCount = 3;
    int jobsDone = 0;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar

    try
    {
        if (!cancelled && !mongo.reconnect())
            std::cerr << tr("Connection failed").toStdString() << std::endl;
        else
        {

            mongo.reauthenticate();

            //------------------------------------------------------------------
            // Execute command (such as drop or update user) if any
            if (command.isOk())
                mongo.runCommand(database, command);

            //------------------------------------------------------------------
            // Get mapping of databases with their associated projects.
            // This is long running job!
            std::list<std::string> databases = mongo.getDatabases();
            std::map<std::string, std::list<std::string> > databasesWithProjects =
                    mongo.getDatabasesWithProjects(databases);
            emit databasesWithProjectsFetched(databasesWithProjects);
            emit progressRangeChanged(0, jobsCount);
            emit progressValueChanged(jobsDone++);

            //------------------------------------------------------------------
            std::auto_ptr<mongo::DBClientCursor> cursor;
            std::list<std::string> fields; // projection, emtpy at the moment
            unsigned long long skip = 0;
            //------------------------------------------------------------------
            // Get custom roles
            std::list<std::string> roles;
            fields.clear();
            fields.push_back(REPO_LABEL_ROLE);
            skip = 0;
            do
            {
                for (; !cancelled && cursor.get() && cursor->more(); ++skip)
                {
                    core::RepoRole role(cursor->nextSafe());
                    roles.push_back(role.getName());
                }
                if (!cancelled)
                    cursor = mongo.listAllTailable(
                        database,
                        REPO_SYSTEM_ROLES,
                        fields,
                        REPO_LABEL_ROLE,
                        -1,
                        skip);

            }
            while (!cancelled && cursor.get() && cursor->more());
            emit customRolesFetched(roles);
            emit progressValueChanged(jobsDone++);

            //------------------------------------------------------------------
            // Get users
            fields.clear();
            skip = 0;
            do
            {
                for (; !cancelled && cursor.get() && cursor->more(); ++skip)
                {
                    core::RepoUser user(cursor->nextSafe());
                    emit userFetched(user.copy());
                }
                if (!cancelled)
                    cursor = mongo.listAllTailable(
                        database,
                        REPO_SYSTEM_USERS,
                        fields,
                        "user",
                        -1,
                        skip);
            }
            while (!cancelled && cursor.get() && cursor->more());
            emit progressValueChanged(jobsDone++);
        }
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    emit RepoWorkerAbstract::finished();
}

