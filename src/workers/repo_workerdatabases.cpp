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


#include "repo_workerdatabases.h"

repo::gui::RepoWorkerDatabases::RepoWorkerDatabases(const repo::core::MongoClientWrapper& mongo)
	: RepoWorkerAbstract()
	, mongo(mongo)
{
	//qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
	qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
	//qRegisterMetaType<QVector<int>>("QVector<int>");
}

repo::gui::RepoWorkerDatabases::~RepoWorkerDatabases() {}

void repo::gui::RepoWorkerDatabases::run()
{	
	int jobsCount = 0;
	// undetermined (moving) progress bar
	emit progressRangeChanged(0, 0);
	emit progressValueChanged(0);

	if (!cancelled && !mongo.reconnect())
        std::cout << "Connection failed" << std::endl;
	else
	{
		mongo.reauthenticate();

		emit hostFetched(QString::fromStdString(mongo.getUsernameAtHostAndPort()));			
        //----------------------------------------------------------------------
		// For each database (if not cancelled)
		std::list<std::string> databases = mongo.getDbs();

        //----------------------------------------------------------------------
		// TODO: write custom sorter which is case insensitive
		databases.sort();

        //----------------------------------------------------------------------
		jobsCount = databases.size();
		emit progressRangeChanged(0, jobsCount);

		int counter = 0;
        //----------------------------------------------------------------------

		for (std::list<std::string>::const_iterator dbIterator = databases.begin(); 
			!cancelled && dbIterator != databases.end(); 
			++dbIterator) 
		{
			const std::string database = *dbIterator;
			emit databaseFetched(QString::fromStdString(database));
            //------------------------------------------------------------------
			// For each collection within the database (if not cancelled)
			std::list<std::string> collections = mongo.getCollections(database);	
			for (std::list<std::string>::const_iterator colIterator = collections.begin(); 
				!cancelled && colIterator != collections.end(); 
				++colIterator) 
			{
				const std::string collection = *colIterator;			
				const unsigned long long count = mongo.countItemsInCollection(collection);
				const unsigned long long size = mongo.getCollectionSize(collection);
				emit collectionFetched(
					QString::fromStdString(mongo.nsGetCollection(collection)), 
					count, size);
			}
            //------------------------------------------------------------------
			emit progressValueChanged(counter++);
		}
	}
    //--------------------------------------------------------------------------
	emit progressValueChanged(jobsCount);
	emit RepoWorkerAbstract::finished();
}
