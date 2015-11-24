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

RepoWorkerRoles::RepoWorkerRoles(
        const repo::RepoToken *token,
        repo::RepoController *controller,
        const std::string &database,
        const repo::core::model::RepoRole &role,
        Command command)
    : token(token)
    , controller(controller)
    , database(database)
    , role(role)
    , command(command)
{
    qRegisterMetaType<repo::core::model::RepoUser>("repo::core::model::RepoRole");
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
        //--------------------------------------------------------------------------
        // Execute command (such as drop or update user) if any
        if (!role.isEmpty())
        {
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
            }
        }


        std::vector<repo::core::model::RepoRole> roles =
                controller->getRolesFromDatabase(token, database);

        jobsCount = roles.size();
        emit progressRangeChanged(0, jobsCount);
        for (int i = 0; i < roles.size(); ++i)
        {
            emit roleFetched(roles[i]);
            emit progressValueChanged(jobsDone++);
        }

    }
    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    emit RepoAbstractWorker::finished();
}


