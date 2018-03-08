/**
*  Copyright (C) 2016 3D Repo Ltd
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


#include "repo_worker_repositories.h"

using namespace repo::worker;
using namespace repo::gui::primitive;

RepositoriesWorker::RepositoriesWorker(
        repo::RepoController *controller,
        const repo::RepoController::RepoToken *token,
        const RepoStandardItemRow &databaseRow)
    : RepoAbstractWorker(),
      controller(controller),
      token(token),
      databaseRow(databaseRow)
{
    qRegisterMetaType<std::list<std::string> >("std::list<std::string>");
    qRegisterMetaType<std::map<std::string, std::list<std::string> > >("std::map<std::string, std::list<std::string> >");
    qRegisterMetaType<gui::primitive::RepoStandardItemRow>("gui::primitive::RepoStandardItemRow");
}

RepositoriesWorker::~RepositoriesWorker() {}

void RepositoriesWorker::run()
{
    int jobsCount = 2;
    int jobsDone = 0;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar
    emit progressValueChanged(0);

    emit progressRangeChanged(0, jobsCount);

    //--------------------------------------------------------------------------

    std::list<std::string> database;
    std::string dbName = databaseRow[NAME]->text().toStdString();
    database.push_back(dbName);

    std::map<std::string, std::list<std::string> > databasesWithProjects =
            controller->getDatabasesWithProjects(token, database);



    for (std::string project : databasesWithProjects[dbName])
    {
        RepoStandardItemRow projectRow = RepoStandardItemFactory::makeProjectRow(project);
        emit projectFetched(databaseRow, projectRow);
        emit progressValueChanged(jobsDone++);
    }

    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    // All done, mark host as cached
    databaseRow[NAME]->setType(DATABASE_CACHED);
    emit RepoAbstractWorker::finished();
}
