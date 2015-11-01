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

RepoWorkerModifiedNodes::RepoWorkerModifiedNodes(
        repo::core::model::RepoScene *scene)
    : RepoAbstractWorker()
    , scene(scene)
{}

RepoWorkerModifiedNodes::~RepoWorkerModifiedNodes() {}

void RepoWorkerModifiedNodes::run()
{
    std::cout << tr("Populating Commit Dialog...").toStdString() << std::endl;
    emit progressRangeChanged(0, 0); // undetermined (moving) progress bar

    int jobsDone = 0;

    if (scene)
    {
        std::vector<repoUUID> modifiedNodes = scene->getModifiedNodesID();

        emit progressRangeChanged(0, modifiedNodes.size());

        //----------------------------------------------------------------------
        // Emit nodes one by one
        for (const auto &sharedID : modifiedNodes)
        {
            repo::core::model::RepoNode *node = scene->getNodeBySharedID(sharedID);
            emit modifiedNode(node);
            emit progressValueChanged(++jobsDone);
        }
    }
    //--------------------------------------------------------------------------
    // Done
    emit RepoAbstractWorker::finished();
}


