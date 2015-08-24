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

#include "repo_worker_project_settings.h"
#include "../logger/repo_logger.h"
//------------------------------------------------------------------------------
// Core


//------------------------------------------------------------------------------

using namespace repo::worker;

ProjectSettingsWorker::ProjectSettingsWorker(
	const repo::RepoToken                              *token,
	repo::RepoController                               *controller,
	const std::string                                  &database,
	const repo::core::model::RepoProjectSettings &settings,
	const bool                                         &isDelete)
	: token(token)
	, controller(controller)
	, database(database)
	, settings(settings)
	, isDelete(isDelete)
{
	qRegisterMetaType<repo::core::model::RepoProjectSettings>("repo::core::model::RepoProjectSettings");
}

//------------------------------------------------------------------------------

ProjectSettingsWorker::~ProjectSettingsWorker() {}

//------------------------------------------------------------------------------

void ProjectSettingsWorker::run()
{
	int jobsCount = 1;
	int jobsDone = 0;
	emit progressRangeChanged(0, 0); // undetermined (moving) progress bar

	//------------------------------------------------------------------
	// Execute command (such as drop or update user) if any
	
	if (!settings.isEmpty())
	{

		if (isDelete)
		{
			repoLog("Removing a project settings...");

			controller->removeDocument(token, database, REPO_COLLECTION_SETTINGS, settings);
		}
		else
		{
			repoLog("Update/insert project settings...");
			controller->upsertDocument(token, database, REPO_COLLECTION_SETTINGS, settings);

		}
	}

	emit progressValueChanged(jobsDone++);


	uint32_t nSettings = controller->countItemsInCollection(token, database, REPO_COLLECTION_SETTINGS);
	jobsCount += nSettings;
	emit progressRangeChanged(jobsDone, jobsCount);

	//------------------------------------------------------------------
	// Get project settings
	std::auto_ptr<mongo::DBClientCursor> cursor;
	std::list<std::string> fields; // projection, empty at the moment
	uint64_t retrieved = 0;

	while (!cancelled && nSettings > retrieved)
	{

		std::vector<repo::core::model::RepoBSON> bsons =
			controller->getAllFromCollectionContinuous(token, database, REPO_COLLECTION_SETTINGS, retrieved);

		for (auto bson : bsons)
		{
			if (cancelled) break;
			if (bson.isValid() && !bson.isEmpty())
			{
				emit projectSettingsFetched(repo::core::model::RepoProjectSettings(bson));
			}
			emit progressValueChanged(jobsDone++);
			++retrieved;
		}
	}

	//--------------------------------------------------------------------------
	emit progressValueChanged(jobsCount);
	emit RepoAbstractWorker::finished();
}

