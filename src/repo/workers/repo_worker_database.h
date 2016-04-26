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


#ifndef REPO_WORKER_DATABASES_H
#define REPO_WORKER_DATABASES_H

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_collection_stats.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
	namespace worker {

		/*!
		* Worker class to that fetches individual databases from given Mongo client.
		*/
		class DatabaseWorker : public RepoAbstractWorker {

			Q_OBJECT

		public:

			//! Default worker constructor.
			DatabaseWorker(repo::RepoController *controller, repo::RepoController::RepoToken *token);

			//! Default empty destructor.
			~DatabaseWorker();

		signals:

			void hostFetched(QString host);

			void databaseFetched(QString database);

			void collectionFetched(repo::core::model::CollectionStats stats);

			public slots :

			/*!
			* Loads individual databases and for each propagates their collections to
			* the given tree model.
			*/
			void run();

		signals:

			//! Emitted when a single database processing is finished
			void databaseFinished(const QString &dbName);

		private:
			repo::RepoController *controller;
			repo::RepoController::RepoToken      *token;

		}; // end class

	} // end namespace worker
} // end namespace repo

#endif // end REPO_WORKER_DATABASES_H
