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

#include "repo_worker_history.h"


using namespace repo::worker;

//------------------------------------------------------------------------------
HistoryWorker::HistoryWorker(
	repo::RepoController  *controller,
	const repo::RepoToken *token,
	const QString &database,
	const QString &project)
	: controller(controller)
	, token(token)
	, database(database.toStdString())
	, project(project.toStdString())
{}

//------------------------------------------------------------------------------

HistoryWorker::~HistoryWorker() {}

//------------------------------------------------------------------------------

void HistoryWorker::run()
{
	
	std::string collection = project + ".history";

	uint64_t jobsCount = controller->countItemsInCollection(token, database, collection);
	emit progressRangeChanged(0, jobsCount);

	//----------------------------------------------------------------------
	// Retrieves all BSON objects until finished or cancelled.
	uint64_t retrieved = 0;

	while (!cancelled && jobsCount > retrieved)
	{

		std::vector<repo::core::model::RepoBSON> bsons =
			controller->getAllFromCollectionContinuous(token, database, collection, retrieved);

		for (auto &bson : bsons)
		{
			if (cancelled) break;
			if (bson.isValid() && !bson.isEmpty())
			{
				repo::core::model::RepoNode &node = (repo::core::model::RepoNode)bson;
				if (node.getTypeAsEnum() == repo::core::model::NodeType::REVISION)
					emit revisionFetched(new repo::core::model::RevisionNode(bson));
			}
		}

		retrieved += bsons.size();
	}


	//--------------------------------------------------------------------------
	emit RepoAbstractWorker::finished();
}
