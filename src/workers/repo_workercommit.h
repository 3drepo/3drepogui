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

#ifndef REPO_WORKER_COMMIT_H
#define REPO_WORKER_COMMIT_H

#include <QImage>
//------------------------------------------------------------------------------
#include <RepoNodeAbstract>
#include <RepoGraphAbstract>
#include <RepoGraphHistory>
#include <RepoGraphScene>
#include <RepoWrapperAssimp>
#include <RepoWrapperMongo>
//------------------------------------------------------------------------------
#include "repo_worker_abstract.h"
#include "../conversion/repo_transcoder_assimp.h"
//------------------------------------------------------------------------------

namespace repo {
namespace gui {
	 
class RepoWorkerCommit : public RepoWorkerAbstract 
{
	Q_OBJECT

public :

    //! Constructor.
	RepoWorkerCommit(
        const core::MongoClientWrapper &mongo,
        const QString &database,
        const QString& project,
        const core::RepoNodeRevision *revision,
        const core::RepoNodeAbstractSet &nodes);

    //! Destructor
	~RepoWorkerCommit();

public slots :

	/*! 
     * Commits the scene nodes and emits signals as the commit progresses and
     * once it is finished.
	 */
	void run();

public :

    /*!
     * Returns a sanitized name of a database suitable for mongo, ie no dots
     * and only 63 chars long.
     */
    static QString sanitizeDatabaseName(const QString &database);

    //! See http://docs.mongodb.org/manual/reference/limits/#Restriction-on-Collection-Names
    static QString sanitizeCollectionName(const QString &collection);
	
private :

	//! Name of the database to commit to.
    const std::string database;

    //! Name of the project to commit to.
    const std::string project;

    /*!
     * Revision history to be committed. This is commited first in case of
     * a connection loss.
     */
    const core::RepoNodeRevision *revision;
	
    //! Nodes to be committed.
    core::RepoNodeAbstractSet nodes;
	
	//! MongoDB connection to commit to.
    core::MongoClientWrapper mongo;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_WORKER_COMMIT_H

