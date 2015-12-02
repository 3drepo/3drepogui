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

#include "repo_worker_roles.h"
#include "../logger/repo_logger.h"

using namespace repo::worker;

RepoWorkerRoles::RepoWorkerRoles(const repo::RepoToken *token,
        repo::RepoController *controller,
        const std::string &database,
        const repo::core::model::RepoRole &role,
        const repo::core::model::RepoRoleSettings &settings,
        Command command)
    : token(token)
    , controller(controller)
    , database(database)
    , role(role)
    , settings(settings)
    , command(command)
{
    qRegisterMetaType<repo::core::model::RepoRole>("repo::core::model::RepoRole");
    qRegisterMetaType<repo::core::model::RepoRoleSettings>("repo::core::model::RepoRoleSettings");
}

//------------------------------------------------------------------------------

RepoWorkerRoles::~RepoWorkerRoles() {}

//------------------------------------------------------------------------------

void RepoWorkerRoles::run()
{
    int jobsCount = 0;
    int jobsDone = 0;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar

    if (controller && token)
    {

        //----------------------------------------------------------------------
        // Execute command (such as drop or update user) if any
        if (!role.isEmpty())
        {
            emit progressRangeChanged(0, jobsCount++);
            switch (command)
            {
            case Command::INSERT :
                repoLog("Adding new role\n");
                controller->insertRole(token, role);
                break;
            case Command::UPDATE :
                repoLog("Updating role\n");
                controller->updateRole(token, role);
                break;
            case Command::DROP:
                repoLog("Removing role\n");
                controller->removeRole(token, role);
                break;
            }
            emit progressValueChanged(jobsDone++);
        }

        //----------------------------------------------------------------------
        if (!settings.isEmpty())
        {
            emit progressRangeChanged(0, jobsCount++);
            switch (command)
            {
            case Command::INSERT :
            case Command::UPDATE :
                repoLog("Upserting role settings\n");
                controller->upsertRoleSettings(token, database, role);
                break;
            case Command::DROP:
                repoLog("Removing role\n");
                controller->removeRoleSettings(token, database, role);
                break;
            }
            emit progressValueChanged(jobsDone++);
        }


        //----------------------------------------------------------------------
        // Retrieve role settings into a map
        // TODO: once getRoleSettingByName function on controller is coded in
        // this whole section can go!
        //----------------------------------------------------------------------
        // START DELETE
        //----------------------------------------------------------------------
        emit progressRangeChanged(0, jobsCount++);
        std::vector< repo::core::model::RepoRoleSettings > roleSettings =
                controller->getRoleSettingsFromDatabase(token, database);
        jobsCount += roleSettings.size();
        emit progressRangeChanged(0, jobsCount++);
        std::map<std::string, repo::core::model::RepoRoleSettings> tempSettingsMap;
        for (auto const & setting : roleSettings)
        {
            tempSettingsMap[setting.getName()] = setting;
            emit progressValueChanged(jobsDone++);
        }
        //----------------------------------------------------------------------
        // END DELETE
        //----------------------------------------------------------------------




        //----------------------------------------------------------------------
        // Retrieve roles
        emit progressRangeChanged(0, jobsCount++);
        std::vector<repo::core::model::RepoRole> roles =
                controller->getRolesFromDatabase(token, database);
        emit progressValueChanged(jobsDone++);


        jobsCount += roles.size();
        emit progressRangeChanged(0, jobsCount);
        for (int i = 0; i < roles.size(); ++i)
        {

            // TODO: once getRoleSettingByName function on controller is coded in
            // retrieve role setting here based on role name one at a time
            repo::core::model::RepoRoleSettings setting =
                    controller->getRoleSettingByName(token, database, roles[i].getName());

            // TODO: remove this
            repo::core::model::RepoRoleSettings tempSetting = tempSettingsMap[roles[i].getName()];

            emit roleFetched(roles[i], tempSetting);
            emit progressValueChanged(jobsDone++);
        }

    }
    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    emit RepoAbstractWorker::finished();
}


