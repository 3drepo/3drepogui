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

#include "repo_worker_modified_nodes.h"

using namespace repo::worker;

const int RepoWorkerModifiedNodes::DEFAULT_LIMIT = 1000;

RepoWorkerModifiedNodes::RepoWorkerModifiedNodes(
        repo::core::model::RepoScene *scene,
        int skip,
        int limit)
    : RepoAbstractWorker()
    , scene(scene)
    , skip(skip)
    , limit(limit)
{}

RepoWorkerModifiedNodes::~RepoWorkerModifiedNodes() {}

void RepoWorkerModifiedNodes::run()
{
    std::cout << tr("Populating Commit Dialog...").toStdString() << std::endl;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar

    int jobsDone = skip;

    if (scene && !cancelled)
    {
        std::vector<repoUUID> modifiedNodes = scene->getModifiedNodesID();
		std::vector<repoUUID> addedNodes = scene->getAddedNodesID();
		std::vector<repo::core::model::RepoNode*> removedNodes = scene->getRemovedNodes();
        emit progressRangeChanged(0, modifiedNodes.size());
		int i = skip;
		int base = i;
		repoLog("i = " + std::to_string(i) + " skip = " + std::to_string(skip) + " limit = " + std::to_string(limit) + " added node size = " + std::to_string(addedNodes.size()));
        //----------------------------------------------------------------------
        // Emit nodes one by one
		for (;
			!cancelled &&
			i < addedNodes.size() + base &&
			i < (skip + limit);
		++i)
		{
			repoLog("Emitting a node for added");
			emit modifiedNode(scene->getNodeBySharedID(addedNodes[i-base]), QString("added"));
			emit progressValueChanged(++jobsDone);
		}
		repoLog("[after added] i = " + std::to_string(i) + " skip = " + std::to_string(skip) + " limit = " + std::to_string(limit) + " mod node size = " + std::to_string(modifiedNodes.size()));

		base = i;
        for (;
             !cancelled &&
			 i < modifiedNodes.size() + base  &&
             i < (skip + limit);
             ++i)
        {                       
			repoLog("Emitting a node for modified");
			emit modifiedNode(scene->getNodeBySharedID(modifiedNodes[i - base]), QString("modified"));
            emit progressValueChanged(++jobsDone);
        }
			 repoLog("[after modified] i = " + std::to_string(i) + " skip = " + std::to_string(skip) + " limit = " + std::to_string(limit) + " del node size = " + std::to_string(removedNodes.size()));

		base = i;
		for (;
			!cancelled &&
			i < removedNodes.size() + base  &&
			i < (skip + limit);
		++i)
		{
			repoLog("Emitting a node for removed");
			emit modifiedNode(removedNodes[i - base], QString("removed"));
			emit progressValueChanged(++jobsDone);
		}
    }
    //--------------------------------------------------------------------------
    // Done
    emit RepoAbstractWorker::finished();
}


