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
        RepoGLCWidget *widgetA,
        RepoGLCWidget *widgetB)
    : 
	controller(controller),
	token(token),
	widgetA(widgetA),
    widgetB(widgetB)
{

	repo::core::model::RepoScene *sceneA = widgetA->getRepoScene();
	repo::core::model::RepoScene *sceneB = widgetB->getRepoScene();

	std::vector<repoUUID> added, deleted, modified;
	controller->compareScenesByIDs(token, sceneA, sceneB, added, deleted, modified);

	for (const repoUUID id : added)
	{
		
		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widgetB->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
				0.9, Qt::green);
	}

	for (const repoUUID id : deleted)
	{

		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneA->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH )
			widgetA->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
				0.9, Qt::red);
	}

	for (const repoUUID id : modified)
	{

		//colour widgetB's graph to reflect the comparison
		repo::core::model::RepoNode* node = sceneB->getNodeBySharedID(id);
		if (node && node->getTypeAsEnum() == repo::core::model::NodeType::MESH)
			widgetB->setGLCOccurrenceOpacity(QString::fromStdString(node->getName()),
				0.9, Qt::blue);	

	}

	widgetB->repaintCurrent();
//
//	//================================================= OLD CODE
//
//    
//
//        std::pair<core::RepoSelfSimilarSet::iterator, core::RepoSelfSimilarSet::iterator> keyRange = selfSimilarSetA.equal_range(key);
//        // Iterate over all map elements with key == key
//        for (valuesIt = keyRange.first;  valuesIt != keyRange.second;  ++valuesIt)
//        {
//            cout << "    value = " << (*valuesIt).second << " : " << (*valuesIt).second->getName() << endl;
//
////                    QMetaObject::invokeMethod(
////                                widgetA, "setGLCOccurrenceOpacity", Qt::QueuedConnection,
////                                Q_ARG(QString, QString::fromStdString((*valuesIt).second->getName())),
////                                Q_ARG(qreal, 0.9),
////                                Q_ARG(QColor, color));
//
//                    widgetA->setGLCOccurrenceOpacity(QString::fromStdString((*valuesIt).second->getName()),
//                                                     0.9, color);
//        }
//    }
//
//    std::string currentKey("");
//    RepoColor color;
//
//    for (auto it = selfSimilarSetA.begin(); it != selfSimilarSetA.end(); ++it)
//    {
//
//        core::RepoPCA pca = ((core::RepoNodeMesh*)(it->second))->getPCA();
//        const double lx = pca.getPrincipalComponent(core::RepoPCA::U).magnitude;
//        const double ly = pca.getPrincipalComponent(core::RepoPCA::V).magnitude;
//        const double lz = pca.getPrincipalComponent(core::RepoPCA::W).magnitude;
//
//        //                widgetA->addBoundingBox(
//        //                            pca.getUVWBoundingBox().getLengthX(),
//        //                            pca.getUVWBoundingBox().getLengthY(),
//        //                            pca.getUVWBoundingBox().getLengthZ(),
//        //                            pca.getXYZTransformationMatrix());
//
//        core::RepoNodeMesh* mesh = (core::RepoNodeMesh*)(it->second);
//        widgetA->addBoundingBox(mesh->getBoundingBox().getLengthX(),
//                                mesh->getBoundingBox().getLengthY(),
//                                mesh->getBoundingBox().getLengthZ(),
//                                core::RepoNodeTransformation::matrixToVector(mesh->getBoundingBoxTransformation()));
//    }
//    widgetA->updateGL();
//
//
//    //------------------------------------------------------------------
//
//    core::RepoSelfSimilarSet selfSimilarSetB = diff->getSelfSimilarSetB();
//    for (auto it = selfSimilarSetB.begin(); it != selfSimilarSetB.end(); ++it)
//    {
//        if(currentKey.compare(it->first))
//        {
//            currentKey = it->first;
//            color = RepoColor::getNext();
//        }
//
//        //				QMetaObject::invokeMethod(
//        //					widgetB, "setGLCOccurrenceOpacity", Qt::QueuedConnection,
//        //					Q_ARG(QString, QString::fromStdString(it->second->getName())),
//        //					Q_ARG(qreal, 0.9),
//        //					Q_ARG(QColor, color));
//
//        core::RepoNodeMesh* mesh = (core::RepoNodeMesh*)(it->second);
//        widgetB->addBoundingBox(mesh->getBoundingBox().getLengthX(),
//                                mesh->getBoundingBox().getLengthY(),
//                                mesh->getBoundingBox().getLengthZ(),
//                                core::RepoNodeTransformation::matrixToVector(mesh->getBoundingBoxTransformation()));
//    }
//    widgetB->paintGL();
}

repo::gui::Repo3DDiffRenderer::~Repo3DDiffRenderer()
{
    //delete diff;
}

