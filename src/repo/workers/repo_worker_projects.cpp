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


#include "repo_worker_projects.h"
#include <list>
#include <string>
#include <cctype>

using namespace repo::worker;

RepoWorkerProjects::RepoWorkerProjects(
        repo::RepoController *controller,
        const repo::RepoToken *token)
    : RepoAbstractWorker(),
      controller(controller),
      token(token)
{
    qRegisterMetaType<std::list<std::string> >("std::list<std::string>");
    qRegisterMetaType<RepoDatabasesWithProjects>("RepoDatabasesWithProjects");
}

RepoWorkerProjects::~RepoWorkerProjects() {}

void RepoWorkerProjects::run()
{
    int jobsCount = 2;
    int jobsDone = 0;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar
    emit progressValueChanged(0);

    emit progressRangeChanged(0, jobsCount);

    //--------------------------------------------------------------------------
    // Get mapping of databases with their associated projects.
    // This is long running job!
    std::list<std::string> databases = controller->getDatabases(token);
    emit databasesFetched(databases);
    emit progressValueChanged(jobsDone++);

    RepoDatabasesWithProjects databasesWithProjects =
        controller->getDatabasesWithProjects(token, databases);

    emit databasesWithProjectsFetched(databasesWithProjects);
    emit progressValueChanged(jobsDone++);

    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    emit RepoAbstractWorker::finished();
}


