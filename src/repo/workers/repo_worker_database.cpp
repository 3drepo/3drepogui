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


#include "repo_worker_database.h"
#include <list>
#include <string>
#include <cctype>

using namespace repo::worker;

DatabaseWorker::DatabaseWorker(
	repo::RepoController *controller, 
	repo::RepoToken *token)
	: RepoAbstractWorker(),
	controller(controller),
	token(token)
{
	qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
	qRegisterMetaType<repo::core::model::CollectionStats>("repo::core::model::CollectionStats");
}

DatabaseWorker::~DatabaseWorker() {}

void DatabaseWorker::run()
{
	int jobsCount = 0;
	emit progressRangeChanged(0, 0); // undetermined (moving) progress bar
	emit progressValueChanged(0);

	//----------------------------------------------------------------------
	// For each database (if not cancelled)
	std::list<std::string> databases = controller->getDatabases(token);
	emit hostFetched(QString::fromStdString(controller->getHostAndPort(token)));

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
		emit databaseFetched(QString::fromStdString(database));
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


