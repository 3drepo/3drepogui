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


#ifndef REPO_WORKER_DATABASES_H
#define REPO_WORKER_DATABASES_H

//------------------------------------------------------------------------------
// Repo Core
#include <RepoWrapperMongo>

//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
namespace gui {
	
/*!
 * Worker class to that fetches individual databases from given Mongo client.
 */
class RepoWorkerDatabases : public RepoWorkerAbstract {

	Q_OBJECT

public :

	//! Default worker constructor.
	RepoWorkerDatabases(const repo::core::MongoClientWrapper& /* mongo */);

	//! Default empty destructor.
	~RepoWorkerDatabases();

signals :

	void hostFetched(QString host);

	void databaseFetched(QString database);

	void collectionFetched(QString collection, unsigned long long count, unsigned long long size);

public slots :

	/*! 
	 * Loads individual databases and for each propagates their collections to 
	 * the given tree model.
	 */
	void run();

signals:

    //! Emitted when a single database processing is finished
    void databaseFinished(const QString &dbName);

public :

    static bool caseInsensitiveStringCompare(const std::string& s1, const std::string& s2);

private :

	//! Database connector.
	repo::core::MongoClientWrapper mongo;

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_WORKER_DATABASES_H
