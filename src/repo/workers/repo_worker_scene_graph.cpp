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

//------------------------------------------------------------------------------
// GUI
#include "repo_worker_scene_graph.h"
//------------------------------------------------------------------------------

using namespace repo::worker;

SceneGraphWorker::SceneGraphWorker(
	repo::RepoController *controller,
	const repo::RepoController::RepoToken      * token,
	const QString& database,
	const QString &project,
	const QUuid& id,
	bool headRevision)
	: controller(controller)
	, token(token)
	, database(database.toStdString())
	, project(project.toStdString())
	, id(id)
	, headRevision(headRevision)
{}

SceneGraphWorker::~SceneGraphWorker() {}

void SceneGraphWorker::run()
{
	//-------------------------------------------------------------------------
	// Start
	jobsCount = 1;
	done = 0;
	emit progress(0, 0); // undetermined (moving) progress bar

	repo::core::model::RepoScene *masterSceneGraph
		= controller->fetchScene(
		token,
		database,
		project,
		id.toString().toStdString(),
		headRevision,
		//THIS NEEDS TO SET TO FALSE IF YOU WANT TO EXAMINE THE SCENE
		//at the moment we only visualise the scene so there isn't a need for the whole
		//scene graph being loaded onto memory.
		true ); 


	//--------------------------------------------------------------------------
	emit progress(++jobsCount, jobsCount);
	//--------------------------------------------------------------------------
	emit finished(masterSceneGraph);
	emit RepoAbstractWorker::finished();
}
