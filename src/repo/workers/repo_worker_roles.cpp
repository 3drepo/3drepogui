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

RepoWorkerRoles::RepoWorkerRoles(const repo::RepoController::RepoToken *token,
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
                repoLog("Adding new role '" + role.getName() +"'\n");
                controller->insertRole(token, role);
                break;
            case Command::UPDATE :
                repoLog("Updating role '" + role.getName() +"'\n");
                if (role.getName() != "nodeUserRole")
                    controller->updateRole(token, role);
                else
                    repoLog("Updating 'nodeUserRole' is not allowed!");
                break;
            case Command::DROP:
                repoLog("Removing role '" + role.getName() +"'\n");
                if (role.getName() != "nodeUserRole")
                    controller->removeRole(token, role);
                else
                    repoLog("Removing 'nodeUserRole' is not allowed!");
                break;
            }
            emit progressValueChanged(jobsDone++);


            //------------------------------------------------------------------
            // Settings
            if (!settings.isEmpty())
            {
                emit progressRangeChanged(0, jobsCount++);
                switch (command)
                {
                case Command::INSERT :
                case Command::UPDATE :
                    repoLog("Upserting role settings '" + settings.getName() +"'\n");
                    controller->upsertRoleSettings(token, role, settings);
                    break;
                case Command::DROP:
                    repoLog("Removing role settings '" + settings.getName() +"'\n");
                    controller->removeRoleSettings(token, role, settings);
                    break;
                }
                emit progressValueChanged(jobsDone++);
            }
        }

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
            emit roleFetched(roles[i], controller->getRoleSettings(token, roles[i]));
            emit progressValueChanged(jobsDone++);
        }

    }
    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    emit RepoAbstractWorker::finished();
}


