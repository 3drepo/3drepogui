/**
 *  Copyright (C) 2015 3D Repo Ltd
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

#include "repoworkerprojectsettings.h"

repo::gui::RepoWorkerProjectSettings::RepoWorkerProjectSettings(
        const repo::core::MongoClientWrapper &mongo,
        const std::string &database,
        const core::RepoBSON &command)
        : mongo(mongo)
        , database(database)
        , command(command)
{
    qRegisterMetaType<core::RepoProjectSettings>("core::RepoProjectSettings");
}

repo::gui::RepoWorkerProjectSettings::~RepoWorkerProjectSettings() {}

void repo::gui::RepoWorkerProjectSettings::run()
{
    int jobsCount = 1;
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
            // Execute command (such as drop or update setting) if any
            if (command.isOk())
                mongo.runCommand(database, command);

            emit progressValueChanged(jobsDone++);

            jobsCount += mongo.countItemsInCollection(database, REPO_COLLECTION_SETTINGS);
            emit progressRangeChanged(jobsDone, jobsCount);

            //------------------------------------------------------------------
            // Get project settings
            std::auto_ptr<mongo::DBClientCursor> cursor;
            std::list<std::string> fields; // projection, emtpy at the moment
            unsigned long long skip = 0;
            do
            {
                for (; !cancelled && cursor.get() && cursor->more(); ++skip)
                {
                    core::RepoProjectSettings projectSettings(cursor->nextSafe());
                    emit projectSettingsFetched(projectSettings.copy());
                    emit progressValueChanged(jobsDone++);
                }
                if (!cancelled)
                    cursor = mongo.listAllTailable(
                        database,
                        REPO_COLLECTION_SETTINGS,
                        fields,
                        "",
                        -1,
                        skip);
            }
            while (!cancelled && cursor.get() && cursor->more());

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

