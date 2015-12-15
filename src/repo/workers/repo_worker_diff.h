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

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson.h>


//------------------------------------------------------------------------------
// Repo GUI
#include "repo_worker_abstract.h"
#include "../widgets/repo_widget_rendering.h"


namespace repo {
	namespace worker {
		class DiffWorker : public RepoAbstractWorker {

			Q_OBJECT

		public:

			/*!
			* Default worker constructor.
			*/
			DiffWorker(
				repo::RepoController                    *controller, 
				const repo::RepoToken                   *token,
				repo::core::model::RepoScene            *sceneA,
				repo::core::model::RepoScene            *sceneB);

			//! Default empty destructor.
			~DiffWorker();

            void processResults(
                const repo::manipulator::diff::DiffResult &aRes,
                const repo::manipulator::diff::DiffResult &bRes);

		signals:

			void colorChangeOnA(const repoUUID &id, const qreal &opacity, const QColor &color);
			void colorChangeOnB(const repoUUID &id, const qreal &opacity, const QColor &color);

			public slots :

			void run();

		private:
			repo::RepoController              *controller;
			const repo::RepoToken             *token;
			repo::core::model::RepoScene      *sceneA;
			repo::core::model::RepoScene      *sceneB;
			

		}; // end class

	} // end namespace worker
} // end namespace repo

