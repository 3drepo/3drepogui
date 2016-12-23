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
// Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson.h>


//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
	namespace worker {

		/*!
		* Worker class that fetches individual objects in a given db.collection from Mongo.
		*/
		class CollectionWorker : public RepoAbstractWorker {

			Q_OBJECT

		public:

			/*!
			* Default worker constructor.
			*/
			CollectionWorker(
				repo::RepoController *controller, 
				repo::RepoController::RepoToken      *token,
				const std::string    &database,
				const std::string    &collection);

			//! Default empty destructor.
			~CollectionWorker();


		signals:

			void keyValuePairAdded(
				QVariant /* key */,
				QVariant /* value */,
				QVariant /* type */,
				unsigned int /* depth */);

			public slots :

			void run();

		private:

			void decodeRecords(const repo::core::model::RepoBSON &bson, unsigned int depth);

			repo::RepoController *controller;
			repo::RepoController::RepoToken      *token;

			//! Database in Mongo to fetch data from.
			std::string database;

			//! Collection in the database to fetch data from.
			std::string collection;


		}; // end class

	} // end namespace worker
} // end namespace repo

#endif // end REPO_WORKER_COLLECTION_H

