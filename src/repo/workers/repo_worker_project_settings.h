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

#pragma once

//-----------------------------------------------------------------------------
// Repo Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_project_settings.h>


//-----------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../logger/repo_logger.h"

namespace repo {
	namespace worker {

		//! Worker class to that fetches individual users from given Mongo client.
		class ProjectSettingsWorker : public RepoAbstractWorker
		{

			Q_OBJECT
		public:


			/**
			* Default worker constructor.
			* @param token repo token for the connection
			* @param controller controller to talk to the core library
			* @param database name of the database where users are stored
			* @param user user to update/delete (empty if none)
			* @param isUpdate true - update/insert user, false - delete user
			*/
			ProjectSettingsWorker(
				const repo::RepoController::RepoToken *token,
				repo::RepoController  *controller,
				const std::string &database,
				const repo::core::model::RepoProjectSettings &settings = repo::core::model::RepoProjectSettings(),
				const bool &isDelete = false);

			//! Default empty destructor.
			~ProjectSettingsWorker();
		signals:

			void projectSettingsFetched(repo::core::model::RepoProjectSettings);
	
		public slots :

			void run();

		private:

			//! Database connector.
			const repo::RepoController::RepoToken *token;
			repo::RepoController *controller;

			//! Database name.
			const std::string database;

			//! User to be create or updated or deleted in the database.
			const repo::core::model::RepoProjectSettings settings;
			const bool isDelete;

		}; // end class

	} // end namespace core
} // end namespace repo


