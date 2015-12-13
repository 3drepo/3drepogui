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
    : scene(scene)
    , nodeDiameter(20.0f)
    , penWidth(3.0f)
{
    qRegisterMetaType<repo::core::model::RepoNode *>("repo::core::model::RepoNode *");
    initialize();
}

RepoRendererGraph::~RepoRendererGraph() {}

void RepoRendererGraph::initialize()
{
    if (!scene)
    {
        std::cerr << tr("Attempting to render graph from nullptr scene!").toStdString() << std::endl;
    }
    else
    {
        RepoNode *root = scene->getRoot();
        std::cout << "Rendering graph for " << root->getName() << std::endl;

        std::vector<RepoNode*> nodes(1);
        nodes[0] = root;
        addNodesRecursively(nodes, 0);
    }
}

void RepoRendererGraph::addNodesRecursively(
        const std::vector<RepoNode*> nodes,
        const int row)
{
    if (nodes.size()) // base case
    {
        int counter = 0;

        //----------------------------------------------------------------------
        // X position as half from minus to half plus number of all nodes
        float i = nodes.size() / 2.0f * (-1.0f);

        QHash<QString, QGraphicsItem *> freshlyPainted;
        std::vector<RepoNode*> unpainted;
        for(RepoNode *node : nodes)
        {
            // Collect children from all nodes into one set (so they are unique)
            std::vector<RepoNode*> ch = scene->getChildrenAsNodes(node->getSharedID());
            std::copy(ch.begin(), ch.end(), std::back_inserter(unpainted));

            // Only paint a node if all of its parent are already painted
            if (areAllParentsPainted(node))
            {
                QGraphicsEllipseItem *item = addNode(node, row, i++);
                addLines(node, item);
                freshlyPainted.insert(uuidToQString(node->getSharedID()), item);
                counter++;
            }
            else
            {
                unpainted.push_back(node);
            }
        }

        //----------------------------------------------------------------------
        // Make sure there are no duplicated in unpainted set.
//        auto it = std::unique(unpainted.begin(), unpainted.end());
//        unpainted.resize(std::distance(unpainted.begin(),it));

        std::sort(unpainted.begin(), unpainted.end());
        unpainted.erase(std::unique(unpainted.begin(),unpainted.end()), unpainted.end());

        //----------------------------------------------------------------------
        // Copy over to global painted hashmap
        // these must be included at the very end to make sure that parent-child
        // nodes don't end up on the same graph level in case parent is
        // randomly painted before child in this iteration.
        for (QString uuid : freshlyPainted.keys())
        {
            painted.insert(uuid, freshlyPainted[uuid]);
        }

        //----------------------------------------------------------------------
        std::cout << tr("Created").toStdString() << " ";
        std::cout << counter;
        std::cout << " " << tr("nodes at level").toStdString() << " " << row;
        std::cout << std::endl;
        //----------------------------------------------------------------------

        addNodesRecursively(unpainted, row + 1);
    }
}

QGraphicsEllipseItem *RepoRendererGraph::addNode(repo::core::model::RepoNode *node, float row, float column)
{
    float spacing = nodeDiameter / 4.0f;

    QPen pen;
    pen.setWidthF(penWidth);
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

    qreal x = column * (nodeDiameter + spacing);
    qreal y = row * (nodeDiameter + spacing) * 4;
    QGraphicsEllipseItem *ellipse = addEllipse(
                0, 0, nodeDiameter, nodeDiameter, pen, brush);
    ellipse->setPos(x, y);
    ellipse->setZValue(1.0f);

    //--------------------------------------------------------------------------
    QString toolTip;
    toolTip += tr("_id") + "\t: '" + uuidToQString(node->getUniqueID()) + "',\n";
    toolTip += tr("sid") + "\t: '" + uuidToQString(node->getSharedID()) + "',\n";
    if (!node->getName().empty())
        toolTip += tr("name") + "\t: '" + QString::fromStdString(node->getName()) + "',\n";
    toolTip += tr("type") + "\t: '" + QString::fromStdString(node->getType()) + "'";
    ellipse->setToolTip(toolTip);
    //--------------------------------------------------------------------------
    QVariant var;
    var.setValue(node);
    ellipse->setData(0, var);
    //--------------------------------------------------------------------------
    return ellipse;
}

std::vector<QGraphicsLineItem*> RepoRendererGraph::addLines(
        const repo::core::model::RepoNode *node,
        const QGraphicsItem *nodeItem)
{
    std::vector<QGraphicsLineItem*> lines(node->getParentIDs().size());
    if (node)
    {
        QPen pen(Qt::lightGray);
        pen.setWidthF(penWidth);

        qreal halfDiameter = nodeDiameter/2;
        qreal halfPen = penWidth/2;

        int i = 0;
        for (repoUUID parentID : node->getParentIDs())
        {
            QGraphicsItem *parentItem = painted[uuidToQString(parentID)];
            QGraphicsLineItem *line = addLine(
                        parentItem->scenePos().x() + halfDiameter,
                        parentItem->scenePos().y() + halfDiameter + halfPen,
                        nodeItem->scenePos().x() + halfDiameter,
                        nodeItem->scenePos().y() + halfDiameter + halfPen,
                        pen);
            line->setZValue(0.0f);
            lines[i++] = line;
        }
    }
    return lines;
}

bool RepoRendererGraph::areAllParentsPainted(const repo::core::model::RepoNode *node)
{
    bool allPainted = true;
    for (repoUUID parentID : node->getParentIDs())
    {
        allPainted = allPainted && (painted.contains(uuidToQString(parentID)));
    }
    return allPainted;
}

QString RepoRendererGraph::uuidToQString(const repoUUID &uuid)
{
    return QString::fromStdString(UUIDtoString(uuid));
}

