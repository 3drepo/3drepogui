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

#pragma once

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>

//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"

namespace repo {
	namespace worker {

		/*!
		* Worker class to that fetches individual databases from given Mongo client.
		*/
		class OptimizeWorker : public RepoAbstractWorker {

			Q_OBJECT

		public:

			//! Default worker constructor.
			OptimizeWorker(
				repo::RepoController *controller, 
				const repo::RepoController::RepoToken *token,
				repo::core::model::RepoScene *scene);

			//! Default empty destructor.
			~OptimizeWorker();
			public slots :

			/*!
			* Loads individual databases and for each propagates their collections to
			* the given tree model.
			*/
			void run();

		signals:

		private:
			repo::RepoController *controller;
			const repo::RepoController::RepoToken      *token;
			repo::core::model::RepoScene *scene;

		}; // end class

	} // end namespace worker
} // end namespace repo
