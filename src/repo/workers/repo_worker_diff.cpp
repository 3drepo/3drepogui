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


using namespace repo::worker;
namespace repoModel = repo::core::model;

DiffWorker::DiffWorker(
	repo::RepoController                    *controller,
	const repo::RepoToken                   *token,
	      repo::core::model::RepoScene      *sceneA,    
		  repo::core::model::RepoScene      *sceneB)
	: controller(controller)
	, token(token)
	, sceneA(sceneA)
	, sceneB(sceneB) 
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
		controller->compareScenesByNames(token, sceneA, sceneB, aRes, bRes);

        processResults(aRes, bRes);
	}
	else
	{
		repoLogError("Failed to compare scenes: Null pointer to scene(s)!");
	}
	
	emit progressValueChanged(jobsCount);
	emit RepoAbstractWorker::finished();
}

void DiffWorker::processResults(
	const repo::manipulator::diff::DiffResult &aRes,
	const repo::manipulator::diff::DiffResult &bRes)
{
	for (const repoUUID id : aRes.added)
	{
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnA(node->getUniqueID(), 0.9, Qt::red);
	}

	for (const repoUUID id : aRes.modified)
	{
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnA(node->getUniqueID(), 0.9, Qt::cyan);
	}

	for (const repoUUID id : bRes.added)
	{
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnB(node->getUniqueID(), 0.9, Qt::green);
	}

	for (const repoUUID id : aRes.modified)
	{
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
            emit colorChangeOnB(node->getUniqueID(), 0.9, Qt::cyan);
	}

}
