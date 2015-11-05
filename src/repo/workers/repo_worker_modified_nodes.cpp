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
        emit progressRangeChanged(0, modifiedNodes.size());

        //----------------------------------------------------------------------
        // Emit nodes one by one
        for (int i = skip;
             !cancelled &&
             i < modifiedNodes.size() &&
             i < (skip + limit);
             ++i)
        {                       
            emit modifiedNode(scene->getNodeBySharedID(modifiedNodes[i]));
            emit progressValueChanged(++jobsDone);
        }
    }
    //--------------------------------------------------------------------------
    // Done
    emit RepoAbstractWorker::finished();
}


