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

#ifndef REPO_WORKER_HISTORY_H
#define REPO_WORKER_HISTORY_H
//-----------------------------------------------------------------------------
// Repo Core
#include <RepoWrapperMongo>
#include <RepoNodeRevision>

//-----------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
namespace gui {
	
//! Worker class to that fetches individual revisions from given Mongo client.
class RepoWorkerHistory : public RepoWorkerAbstract
{

	Q_OBJECT

public :

	//! Default worker constructor.
	RepoWorkerHistory(
        const core::MongoClientWrapper &mongo,
        const QString &database,
        const QString& project);

	//! Default empty destructor.
	~RepoWorkerHistory();

signals :

    void revisionFetched(core::RepoNodeRevision *);

public slots :

	void run();

private :

	//! Database connector.
    core::MongoClientWrapper mongo;

	std::string database;

    std::string project;

}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_WORKER_HISTORY_H
