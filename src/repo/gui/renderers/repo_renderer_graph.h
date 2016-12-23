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

#pragma once

// Qt
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsItem>

// GUI
#include "../primitives/repo_color.h"

// Core
#include <repo/core/model/collection/repo_scene.h>
#include <repo/core/model/repo_node_utils.h>

Q_DECLARE_METATYPE(repo::core::model::RepoNode *)

namespace repo {
namespace gui{
namespace renderer {

	class GraphRenderer : public QGraphicsScene
	{

	public:

		GraphRenderer(const repo::core::model::RepoScene *scene,
			repo::core::model::RepoScene::GraphType graphType =
			repo::core::model::RepoScene::GraphType::DEFAULT);

		~GraphRenderer();

	public:

		void initialize();

	private:

		//! Breadth first search in recursive fashion
		void addNodesRecursively(
			const std::vector<core::model::RepoNode *> nodes,
			int row);

		//! Adds node
		QGraphicsEllipseItem *addNode(
			repo::core::model::RepoNode *node, float row, float column);

		//! Adds directed link from node to all its parents assuming they are all already painted.
		std::vector<QGraphicsLineItem*> addLines(
			const repo::core::model::RepoNode *node,
			const QGraphicsItem *item);

		bool areAllParentsPainted(const repo::core::model::RepoNode *node);

	public:

        static QString uuidToQString(const repo::lib::RepoUUID &uuid);

	private:

		const repo::core::model::RepoScene *scene;

		repo::core::model::RepoScene::GraphType graphType;

		//! Diameter of each node in the graph.
		qreal nodeDiameter;

		qreal penWidth;

		QHash<QString, QGraphicsItem *> painted;


	}; // end class

} //end namespace renderer
}
} // end namespace repo
