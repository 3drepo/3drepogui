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


#include "repo_renderer_3ddiff.h"

using namespace repo::gui;
using namespace repo::gui::renderer;

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


//	repo::worker::DiffWorker *worker = new repo::worker::DiffWorker(
//		controller,
//		token,
//		sceneA,
//		sceneB, true);

	
//	QObject::connect(worker, &repo::worker::DiffWorker::colorChangeOnA,
//		widgetA, &widgets::RepoRenderingWidget::setMeshColor);
//	QObject::connect(worker, &repo::worker::DiffWorker::colorChangeOnB,
//		widgetB, &widgets::RepoRenderingWidget::setMeshColor);

//	//----------------------------------------------------------------------
//	// Fire up the asynchronous calculation.
//	QThreadPool::globalInstance()->start(worker);

}

Repo3DDiffRenderer::~Repo3DDiffRenderer()
{
}
