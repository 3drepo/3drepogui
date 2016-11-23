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


#include "repo_worker_databases.h"
#include <list>
#include <string>
#include <cctype>

using namespace repo::worker;
using namespace repo::gui::primitive;
using namespace repo::core::model;

DatabasesWorker::DatabasesWorker(
        repo::RepoController *controller,
        repo::RepoController::RepoToken *token)
    : RepoAbstractWorker(),
      controller(controller),
      token(token)
{  
    qRegisterMetaType<gui::primitive::RepoStandardItemRow>("gui::primitive::RepoStandardItemRow");
    qRegisterMetaType<core::model::DatabaseStats>("core::model::DatabaseStats");
}

DatabasesWorker::~DatabasesWorker() {}

void DatabasesWorker::run()
{
    int jobsCount = 0;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar
    emit progressValueChanged(0);


    RepoStandardItemRow hostRow = RepoStandardItemFactory::makeHost(controller, token);
    emit hostFetched(hostRow);



    QList<RepoStandardItemRow> databaseRows;
    for (std::string database : controller->getDatabases(token))
    {
        RepoStandardItemRow databaseRow = RepoStandardItemFactory::makeDatabase(database);
        emit databaseFetched(hostRow, databaseRow);
        databaseRows.append(databaseRow);
    }


    // This has to be separate loop for speed purposes
    // Setting is emitted as it needs to happen on the main GUI thread to get
    // instant visual feedback for the end user
    for (const RepoStandardItemRow &databaseRow : databaseRows)
    {
        QString database = databaseRow[NAME]->text();
        emit databaseStatsFetched(hostRow,
                                  databaseRow,
                                  controller->getDatabaseStats(
                                      token,
                                      database.toStdString()));
    }

    //----------------------------------------------------------------------
    // For each database (if not cancelled)
    std::list<std::string> databases = controller->getDatabases(token);
//    emit hostFetched(QString::fromStdString(controller->getHostAndPort(token)));

    //----------------------------------------------------------------------
    jobsCount = (int)databases.size() * 2;
    emit progressRangeChanged(0, jobsCount);


    int counter = 0;

    //----------------------------------------------------------------------
    // Populate collections with sizes
    for (std::list<std::string>::const_iterator dbIterator = databases.begin();
         !cancelled && dbIterator != databases.end();
         ++dbIterator)
    {
        const std::string database = *dbIterator;
//        emit databaseFetched(QString::fromStdString(database));
        emit progressValueChanged(counter++);
        //------------------------------------------------------------------
        // For each collection within the database (if not cancelled)
        std::list<std::string> collections = controller->getCollections(token, database);
        for (std::list<std::string>::const_iterator colIterator = collections.begin();
             !cancelled && colIterator != collections.end();
             ++colIterator)
        {
            emit collectionFetched(controller->getCollectionStats(token, database, *colIterator));
        }
        emit databaseFinished(QString::fromStdString(database));
        //------------------------------------------------------------------
        emit progressValueChanged(counter++);
    }


    //--------------------------------------------------------------------------
    emit progressValueChanged(jobsCount);
    emit RepoAbstractWorker::finished();
}


