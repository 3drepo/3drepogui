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
#include <RepoUser>

//------------------------------------------------------------------------------
repo::gui::RepoWorkerUsers::RepoWorkerUsers(
    const repo::core::MongoClientWrapper &mongo,
    const QString &database)
    : mongo(mongo)
    , database(database.toStdString())
{
    qRegisterMetaType<core::RepoUser>("core::RepoUser");
}

//------------------------------------------------------------------------------

repo::gui::RepoWorkerUsers::~RepoWorkerUsers() {}

//------------------------------------------------------------------------------

void repo::gui::RepoWorkerUsers::run()
{
    if (!mongo.reconnect())
        std::cerr << tr("Connection failed").toStdString() << std::endl;
    else
    {
        mongo.reauthenticate(database);

        std::list<std::string> fields; // projection, emtpy at the moment

        //----------------------------------------------------------------------
        // Retrieves all BSON objects until finished or cancelled.
        unsigned long long skip = 0;
        std::auto_ptr<mongo::DBClientCursor> cursor;
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
                    "system.users", // TODO: get strings from mongo itself
                    fields,
                    "user",
                    -1,
                    skip);
        }
        while (!cancelled && cursor.get() && cursor->more());
    }
    //--------------------------------------------------------------------------
    emit RepoWorkerAbstract::finished();
}

