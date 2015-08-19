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


#include "repo_worker_file_export.h"
#include "../logger/repo_logger.h"
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
using namespace repo::worker;


repo::worker::FileExportWorker::FileExportWorker(const std::string & fullPath,
	repo::RepoController *controller,
	const repo::manipulator::graph::RepoScene *scene)
	: fullPath(fullPath)
	, controller(controller)
	, scene(scene)
	, RepoAbstractWorker() {}

repo::worker::FileExportWorker::~FileExportWorker() {}

void repo::worker::FileExportWorker::run()
{
	//-------------------------------------------------------------------------
	// Start
	emit progress(0, 0);

	repoLog("Exporting Repo Scene to " + fullPath);
	if (controller->saveSceneToFile(fullPath, scene))
	{
		repoLog("Export completed successfully.");
	}
	else
	{
		repoLog("Export failed");
	}
	emit progress(1, 1);
	
	//-------------------------------------------------------------------------
	// Done
	emit RepoAbstractWorker::finished();
}
