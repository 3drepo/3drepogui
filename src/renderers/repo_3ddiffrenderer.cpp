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

repo::gui::Repo3DDiffRenderer::Repo3DDiffRenderer(
	repo::RepoController *controller,
	const repo::RepoToken *token,
        RepoRenderingWidget *widgetA,
        RepoRenderingWidget *widgetB)
    : 
	controller(controller),
	token(token),
	widgetA(widgetA),
    widgetB(widgetB)
{

	repo::core::model::RepoScene *sceneA = widgetA->getRepoScene();
	repo::core::model::RepoScene *sceneB = widgetB->getRepoScene();

	repo::manipulator::diff::DiffResult aRes, bRes;
	//FIXME : Probably should move this out, this class should purely be about rendering differences
	controller->compareScenesByIDs(token, sceneA, sceneB, aRes, bRes);
	//TODO: If it is a modification of a transformation or a transformation has been added, it needs to 
	// highlight the entire subtree as everything is moved.

	for (const repoUUID id : bRes.added)
	{
		
		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widgetB->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
				0.9, Qt::green);

		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::TRANSFORMATION)
			repoLogError("Transformation difference isn't yet supported by the renderer yet.");
	}


	for (const repoUUID id : bRes.modified)
	{

		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widgetB->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
			0.9, Qt::cyan);

		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::TRANSFORMATION)
			repoLogError("Transformation difference isn't yet supported by the renderer yet.");
	}

	for (const repoUUID id : aRes.added)
	{

		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH )
			widgetA->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
				0.9, Qt::red);

		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::TRANSFORMATION)
			repoLogError("Transformation difference isn't yet supported by the renderer yet.");
	}

	for (const repoUUID id : aRes.modified)
	{

		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widgetA->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
				0.9, Qt::cyan);	
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::TRANSFORMATION)
			repoLogError("Transformation difference isn't yet supported by the renderer yet.");
	}


	widgetA->repaintCurrent();
	widgetB->repaintCurrent();
}

repo::gui::Repo3DDiffRenderer::~Repo3DDiffRenderer()
{
    //delete diff;
}

