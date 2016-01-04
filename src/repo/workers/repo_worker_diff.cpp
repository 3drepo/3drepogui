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


#include "repo_worker_diff.h"

#include "../logger/repo_logger.h"
#include "../gui/primitives/repo_color.h"


using namespace repo::worker;
namespace repoModel = repo::core::model;

DiffWorker::DiffWorker(
	repo::RepoController                    *controller,
	const repo::RepoToken                   *token,
	      repo::core::model::RepoScene      *sceneA,    
          repo::core::model::RepoScene      *sceneB,
    const repo::manipulator::diff::Mode     diffMode,
    const bool                              colourCorres)
	: controller(controller)
	, token(token)
	, sceneA(sceneA)
	, sceneB(sceneB) 
    , diffMode(diffMode)
	, colourCorres(colourCorres)
{
	qRegisterMetaType<repoUUID>("repoUUID");
}

DiffWorker::~DiffWorker() {}

void DiffWorker::run()
{
	int jobsCount = 1;
	// undetermined (moving) progress bar
	emit progressRangeChanged(0, 0);
	emit progressValueChanged(0);

	if (sceneA && sceneB)
	{
		repo::manipulator::diff::DiffResult aRes, bRes;
		//FIXME: pick diff mode
        controller->compareScenes(token, sceneA, sceneB, aRes, bRes, diffMode);

		if (colourCorres)
			processResultsByCorrespondence(aRes, bRes);
		else
			processResultsByDiff(aRes, bRes);
	}
	else
	{
		repoLogError("Failed to compare scenes: Null pointer to scene(s)!");
	}
	
	emit progressValueChanged(jobsCount);
	emit RepoAbstractWorker::finished();
}

void DiffWorker::processResultsByCorrespondence(
	const repo::manipulator::diff::DiffResult &aRes,
	const repo::manipulator::diff::DiffResult &bRes)
{
    for (const auto pair : aRes.correspondence)
    {
        QColor color = repo::gui::primitive::RepoColor::getNext();
        repo::core::model::RepoNode* nodeA = sceneA->getNodeBySharedID(pair.first);
        if (nodeA && nodeA->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnA(nodeA->getUniqueID(), 1.0, color);

        repo::core::model::RepoNode* nodeB = sceneB->getNodeBySharedID(pair.second);
        if (nodeB && nodeB->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnB(nodeB->getUniqueID(), 1.0, color);
    }

}

void DiffWorker::processResultsByDiff(
	const repo::manipulator::diff::DiffResult &aRes,
	const repo::manipulator::diff::DiffResult &bRes)
{
	for (const repoUUID id : aRes.added)
	{
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnA(node->getUniqueID(), 1.0, Qt::red);
	}

	for (const repoUUID id : aRes.modified)
	{
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			emit colorChangeOnA(node->getUniqueID(), 1.0, Qt::cyan);
	}

	for (const repoUUID id : bRes.added)
	{
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			emit colorChangeOnB(node->getUniqueID(), 1.0, Qt::green);
	}

	for (const repoUUID id : bRes.modified)
	{
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			emit colorChangeOnB(node->getUniqueID(), 1.0, Qt::cyan);
	}

}
