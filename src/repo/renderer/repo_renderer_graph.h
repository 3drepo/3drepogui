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

// GUI
#include "../../primitives/repo_color.h"

// Core
#include <repo/core/model/collection/repo_scene.h>
#include <repo/core/model/repo_node_utils.h>

Q_DECLARE_METATYPE(repo::core::model::RepoNode *)

namespace repo {
namespace renderer {

class RepoRendererGraph : public QGraphicsScene
{

public:

    RepoRendererGraph(const repo::core::model::RepoScene *scene);

    ~RepoRendererGraph();

public :

    void addSceneRecursively(const repo::core::model::RepoScene *scene);

private :

    //! Breadth first search in recursive fashion
    void addNodesRecursively(
            const repo::core::model::RepoScene *scene,
            const std::set<repo::core::model::RepoNode *> nodes,
            const int row);

    //! Adds node
    void addNode(repo::core::model::RepoNode *node, float row, float column);


}; // end class

} //end namespace renderer
} // end namespace repo
