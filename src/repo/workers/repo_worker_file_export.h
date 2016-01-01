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

/**
* Turns 3D Model files into Repo Scene representation
*/
#pragma once

//------------------------------------------------------------------------------
// Core
#include <repo/repo_controller.h>

//-----------------------------------------------------------------------------
#include "repo_worker_abstract.h"
#include "../gui/widgets/repowidgetassimpflags.h"
#include "../gui/repo_settings.h"
//-----------------------------------------------------------------------------

#include <QImage>

namespace repo {
	namespace worker {

		/*!
		* Worker class to load a 3D file using the Assimp loading library. Use with
		* QThread by connecting signals and slots. The worker needs to be moved to a
		* new thread and the thread has to be started.
		*/
		class FileExportWorker : public RepoAbstractWorker {

			Q_OBJECT

		public:

			/*!
			* Export the given 3D repo scene to a specified file format within the fullPath
			* @param fullPath path to export to (including filename and extension)
			* @param controller controller to the core library
			* @param scene scene to export
			*/
			FileExportWorker(const std::string& fullPath,
				repo::RepoController *controller,
				const repo::core::model::RepoScene *scene);

			//! Default empty destructor.
			~FileExportWorker();


			public slots :

			/*!
			* Loads scene and emits signals as the loading progresses and once
			* the loading is finished.
			*/
			void run();

		signals:


		private:

			//! Full canonical path of the 3D file to be loaded.
			const std::string fullPath;

			repo::RepoController *controller;
			const repo::core::model::RepoScene *scene;
		}; // end class

	} // end namespace core
} // end namespace repo

