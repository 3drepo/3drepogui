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


#include "repo_3ddiffrenderer.h"

using namespace repo::gui;

Repo3DDiffRenderer::Repo3DDiffRenderer(
	repo::RepoController *controller,
	const repo::RepoToken *token,
        widgets::RepoRenderingWidget *widgetA,
        widgets::RepoRenderingWidget *widgetB)
    : 
	controller(controller),
	token(token),
	widgetA(widgetA),
    widgetB(widgetB)
{

	repo::core::model::RepoScene *sceneA = widgetA->getRepoScene();
	repo::core::model::RepoScene *sceneB = widgetB->getRepoScene();


	repo::worker::DiffWorker *worker = new repo::worker::DiffWorker(
		controller,
		token,
		sceneA,
		sceneB);

	
	QObject::connect(worker, &repo::worker::DiffWorker::diffResultOnA,
		this, &Repo3DDiffRenderer::resultingDifferenceA);
	QObject::connect(worker, &repo::worker::DiffWorker::diffResultOnB, 
		this, &Repo3DDiffRenderer::resultingDifferenceB);

	//----------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	QThreadPool::globalInstance()->start(worker);

}

Repo3DDiffRenderer::~Repo3DDiffRenderer()
{
}

void Repo3DDiffRenderer::resultingDifference(const repo::manipulator::diff::DiffResult &res,
							widgets::RepoRenderingWidget *widget,
							const QColor &modColor,
							const QColor &addColor)
{
	auto scene = widget->getRepoScene();
	repoLog("@resultingDifference");

	//TODO: other types of nodes
	for (const repoUUID id : res.added)
	{
		repo::core::model::RepoNode* node = scene->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widget->setMeshColor(QString::fromStdString(node->getName()),
			node->getUniqueID(), 0.9, addColor);

	}

	for (const repoUUID id : res.modified)
	{
		repo::core::model::RepoNode* node = scene->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widget->setMeshColor(QString::fromStdString(node->getName()),
			node->getUniqueID(), 0.9, modColor);
	}

	widget->repaintCurrent();
}



