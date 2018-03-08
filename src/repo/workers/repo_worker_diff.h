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
#include "../gui/widgets/repo_widget_rendering.h"


namespace repo {
	namespace worker {
		class DiffWorker : public RepoAbstractWorker {

			Q_OBJECT

		public:

			/*!
			* Default worker constructor.
			* @param controller repo controller to bouncer library
			* @param token authentication token to bouncer library
			* @param sceneA base scene to compare from
			* @param sceneB scene to compare against
			* @param colourCorres true: colour correspondence false: colour differences
			*/
            DiffWorker(repo::RepoController                    *controller,
                const repo::RepoController::RepoToken                   *token,
                repo::core::model::RepoScene            *sceneA,
                repo::core::model::RepoScene            *sceneB,
                const repo::DiffMode diffMode,
                const bool colourCorres = false
                );

			//! Default empty destructor.
			~DiffWorker();

		signals:
			/**
			* Signals a change on colour of the mesh is needed given the unique ID of the
			* mesh within scene A
			* @param id unique id of the mesh
			* @param opacity the opacity of the colouring
			* @param color the colour to change to
			*/
                        void colorChangeOnA(const repo::lib::RepoUUID &id, const qreal &opacity, const QColor &color);

			/**
			* Signals a change on colour of the mesh is needed given the unique ID of the
			* mesh within scene A
			* @param id unique id of the mesh
			* @param opacity the opacity of the colouring
			* @param color the colour to change to
			*/
                        void colorChangeOnB(const repo::lib::RepoUUID &id, const qreal &opacity, const QColor &color);

			public slots :

			void run();

		private:
			repo::RepoController              *controller; //Repo Controller
			const repo::RepoController::RepoToken             *token; //Repo Token to access the database
			repo::core::model::RepoScene      *sceneA; //Base scene to compare from
			repo::core::model::RepoScene      *sceneB; //Scene to compare against
			const bool                         colourCorres; //colour correspondence
            repo::DiffMode       diffMode;

			/**
			* Process the results by diff, sending off color change signals
			* base on differences
			* @param aRes results on A
			* @param bRes results on B
			*/
			void processResultsByDiff(
                const repo_diff_result_t &aRes,
                const repo_diff_result_t &bRes);

			/**
			* Process the results by correspondence, sending off color change signals
			* base on correspondence
			* @param aRes results on A
			* @param bRes results on B
			*/
			void processResultsByCorrespondence(
                const repo_diff_result_t &aRes,
                const repo_diff_result_t &bRes);


		}; // end class

	} // end namespace worker
} // end namespace repo

