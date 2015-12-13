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


#include "repo_renderer_graph.h"

using namespace repo::renderer;
using namespace repo::core::model;
using namespace repo::gui;

RepoRendererGraph::RepoRendererGraph(const repo::core::model::RepoScene *scene)
{
    qRegisterMetaType<repo::core::model::RepoNode *>("repo::core::model::RepoNode *");
    addSceneRecursively(scene);
}

RepoRendererGraph::~RepoRendererGraph() {}

void RepoRendererGraph::addSceneRecursively(const repo::core::model::RepoScene *scene)
{
    if (scene)
    {
        RepoNode *root = scene->getRoot();
        std::cout << "Rendering graph for " << root->getName() << std::endl;

        std::set<RepoNode*> nodes;
        nodes.insert(root);
        addNodesRecursively(scene, nodes, 0);
    }
    else
        std::cerr << tr("Attempting to render graph from nullptr scene!").toStdString() << std::endl;
}

void RepoRendererGraph::addNodesRecursively(
        const RepoScene *scene,
        const std::set<RepoNode*> nodes,
        const int row)
{
    if (nodes.size()) // base case
    {
        std::cout << tr("Creating").toStdString() << " ";
        std::cout << nodes.size();
        std::cout << " " << tr("nodes at level").toStdString() << " " << row;
        std::cout << std::endl;

        // X position as half from minus to half plus number of all nodes
        float i = nodes.size() / 2.0f * (-1.0f);

        std::set<RepoNode*> children;
        for(RepoNode *node : nodes)
        {
            addNode(node, row, i++);


            // Collect children from all nodes into one set
            std::vector<RepoNode*> ch = scene->getChildrenAsNodes(node->getSharedID());
            std::copy(ch.begin(), ch.end(), std::inserter(children, children.begin()));
        }
        addNodesRecursively(scene, children, row + 1);
    }
}

void RepoRendererGraph::addNode(repo::core::model::RepoNode *node, float row, float column)
{
    float diameter = 20.0f;
    float spacing = diameter / 4.0f;

    QPen pen;
    pen.setWidthF(3.0f);
    QBrush brush(Qt::SolidPattern);

    QColor dark;
    QColor light;

    switch (node->getTypeAsEnum())
    {
    case NodeType::CAMERA :
        dark = Qt::darkMagenta;
        light = Qt::magenta;
        break;
    case NodeType::MAP :
        dark = RepoColor::fromHex("#cd5b45"); // Coral3
        light = RepoColor::fromHex("#ff7f50"); // Coral
        break;
    case NodeType::MATERIAL :
        dark = Qt::darkRed;
        light = Qt::red;
        break;
    case NodeType::MESH :
        dark = Qt::darkBlue;
        light = Qt::blue;
        break;
    case NodeType::METADATA :
        dark = Qt::darkCyan;
        light = Qt::cyan;
        break;
    case NodeType::REFERENCE :
        dark = RepoColor::fromHex("#6e8b3d"); // DarkOliveGreen4
        light = RepoColor::fromHex("#caff70"); // DarkOliveGreen1
        break;
    case NodeType::REVISION :
        dark = RepoColor::fromHex("#68228b"); // DarkOrchid4
        light = RepoColor::fromHex("#bf3eff"); // DarkOrchid1
        break;
    case NodeType::TEXTURE :
        dark = Qt::darkYellow;
        light = Qt::yellow;
        break;
    case NodeType::TRANSFORMATION :
        dark = Qt::darkGreen;
        light = Qt::green;
        break;
    case NodeType::UNKNOWN :
    default :
        dark = Qt::darkGray;
        light = Qt::gray;
    }

    pen.setColor(dark);
    brush.setColor(light);

    QGraphicsEllipseItem *ellipse = addEllipse(column * (diameter + spacing),
               row * (diameter + spacing) * 4,
               diameter,
               diameter,
               pen,
               brush);

    QString name = QString::fromStdString(node->getType()) + ": " +
            (!(node->getName()).empty()
            ? QString::fromStdString(node->getName())
            : QString::fromStdString(UUIDtoString(node->getUniqueID())));
    ellipse->setToolTip(name);

    QVariant var;
    var.setValue(node);
    ellipse->setData(0, var);
}

