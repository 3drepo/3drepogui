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


#include "repo_worker_file_import.h"
#include "../logger/repo_logger.h"
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
using namespace repo::worker;


repo::worker::FileImportWorker::FileImportWorker(const std::string & fullPath,
    repo::RepoController *controller)
	: fullPath(fullPath)
	, controller(controller)
	, RepoAbstractWorker() {}

repo::worker::FileImportWorker::~FileImportWorker() {}

void repo::worker::FileImportWorker::run()
{
	//-------------------------------------------------------------------------
	// Start
	int jobsCount = 5;
	emit progress(0, 0);

	repoLog("loading repoScene from file");
    repo::settings::RepoSettings settings;
	repo::core::model::RepoScene * repoGraphScene 
        = controller->loadSceneFromFile(fullPath, true, &settings);


    QFileInfo pathInfo(QString::fromStdString(fullPath));
    if ("fbx" == pathInfo.suffix())
        repoGraphScene->reorientateDirectXModel();


	emit progress(jobsCount, jobsCount);
	repoLog("done.");
	//-------------------------------------------------------------------------
	// Done
	emit finished(repoGraphScene);
	emit RepoAbstractWorker::finished();
}
