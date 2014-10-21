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


#ifndef REPO_WORKER_COLLECTION_H
#define REPO_WORKER_COLLECTION_H
//------------------------------------------------------------------------------
// Repo Core
#include <RepoMongoClient>
//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
namespace gui {
	
/*!
 * Worker class that fetches individual objects in a given db.collection from Mongo.
 */
class RepoWorkerCollection : public RepoWorkerAbstract {

	Q_OBJECT

public :

	/*!
	 * Default worker constructor.
	 */
	RepoWorkerCollection(
		const repo::core::MongoClientWrapper& mongo, 
		const QString& database, 
		const QString& collection);

	//! Default empty destructor.
	~RepoWorkerCollection();

signals :

	void keyValuePairAdded(
		QVariant /* key */, 
		QVariant /* value */, 
		QVariant /* type */,
		unsigned int /* depth */);

public slots :

		void run();

private :

	void decodeRecords(const mongo::BSONObj&, unsigned int depth);

	//! Mongo connection to fetch the data from.
	repo::core::MongoClientWrapper mongo;

	//! Database in Mongo to fetch data from.
	std::string database;

	//! Collection in the database to fetch data from.
	std::string collection;


}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_WORKER_COLLECTION_H
	
