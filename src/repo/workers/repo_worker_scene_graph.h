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


#ifndef REPO_WORKER_FETCH_REVISION_H
#define REPO_WORKER_FETCH_REVISION_H
//-----------------------------------------------------------------------------
#include "repo_worker_abstract.h"
//-----------------------------------------------------------------------------
#include <repo/repo_controller.h>

#include <QImage>

namespace repo {
	namespace worker {

		class SceneGraphWorker : public RepoAbstractWorker
		{
			Q_OBJECT

		public:

			/*! Takes a database path to a 3D file that is to be loaded
			using the run() function. By default the revision to be retrieved
			is NULL Uuid (all zeros, i.e. the master branch).

			\param headRevision true if to get head revision, false otherwise

			\sa ~RepoWorkerAssimp(), run()
			*/
			SceneGraphWorker(
				repo::RepoController *controller,
				const repo::RepoController::RepoToken      * token,
				const QString& database,
				const QString& project,
				const QUuid& id = QUuid(),
				bool headRevision = true);

			//! Default empty destructor.
			~SceneGraphWorker();

			public slots :

			/*!
			* Loads scene and emits signals as the loading progresses and once
			* the loading is finished.
			*/
			void run();

		signals:

			//! Emitted when loading is finished. Passes Repo scene.
			void finished(repo::core::model::RepoScene*);

		private:

			//! Client connection
			repo::RepoController *controller;
			const repo::RepoController::RepoToken      * token;

			//! Database to fetch the revision from.
			std::string database;

			//! Project to fetch the revision from.
			std::string project;

			//! SID of a branch if headRevision is true, UID of a specific revision otherwise.
			QUuid id;

			//! True if to fetch head revision from a branch by SID, false if to fetch specific revision by UID.
			bool headRevision;

			//! Number of jobs to be completed.
			int jobsCount;

			//! Number of jobs already done.
			int done;

		}; // end class

	} // end namespace gui
} // end namespace repo

#endif // end REPO_WORKER_FETCH_REVISION_H
