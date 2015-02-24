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

#include <RepoWrapperMongo>

#include "reposelectiontreedockwidget.h"
#include "ui_reposelectiontreedockwidget.h"
#include "../primitives/repo_sortfilterproxymodel.h"

repo::gui::RepoSelectionTreeDockWidget::RepoSelectionTreeDockWidget(
        RepoGLCWidget *glcWidget,
        QWidget *parent)
    : QDockWidget(parent)
    , glcWidget(glcWidget)
    , ui(new Ui::RepoSelectionTreeDockWidget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(glcWidget, SIGNAL(destroyed()),
                     this, SLOT(close()));

    this->setWindowTitle(windowTitle() + ": " + glcWidget->windowTitle());

    QList<QString> headers;
    headers << tr("Name");
    headers << tr("Type");
    ui->filterableTreeWidget->setHeaders(headers);
    ui->filterableTreeWidget->setProxyModel(new RepoSortFilterProxyModel(this, true));


    const core::RepoGraphScene* repoScene = glcWidget->getRepoScene();
    if (repoScene)
    {
        addNode(ui->filterableTreeWidget->getModel()->invisibleRootItem(),
            repoScene->getRoot());
    }
}

repo::gui::RepoSelectionTreeDockWidget::~RepoSelectionTreeDockWidget()
{
    delete ui;
}


void repo::gui::RepoSelectionTreeDockWidget::addNode(
        QStandardItem* parentItem,
        const core::RepoNodeAbstract* node)
{
    QString name;
    if (!node->getName().empty())
        name = QString::fromStdString(node->getName());
    else
        name = tr("<empty>"); //QString::fromStdString(core::MongoClientWrapper::uuidToString(node->getUniqueID()));

    QList<QStandardItem*> row;

    // Name
    QStandardItem* nameItem = new QStandardItem(name);
    nameItem->setToolTip(name);
    nameItem->setEditable(false);
    nameItem->setCheckable(true);
    nameItem->setCheckState(Qt::Checked);
    row << nameItem;

    // Type
    QString type = QString::fromStdString(node->getType());
    if (REPO_NODE_TYPE_TRANSFORMATION == type)
    {
        const core::RepoNodeTransformation* transformation =
                dynamic_cast<const core::RepoNodeTransformation*>(node);
        if (transformation)
        {
            aiMatrix4x4 t = transformation->getMatrix();
            QString nameTooltip =
                    QString::number(t.a1) + ", " +
                    QString::number(t.a2) + ", " +
                    QString::number(t.a3) + ", " +
                    QString::number(t.a4) + "\n" +
                    QString::number(t.b1) + ", " +
                    QString::number(t.b2) + ", " +
                    QString::number(t.b3) + ", " +
                    QString::number(t.b4) + "\n" +
                    QString::number(t.c1) + ", " +
                    QString::number(t.c2) + ", " +
                    QString::number(t.c3) + ", " +
                    QString::number(t.c4) + "\n" +
                    QString::number(t.d1) + ", " +
                    QString::number(t.d2) + ", " +
                    QString::number(t.d3) + ", " +
                    QString::number(t.d4);
            nameItem->setToolTip(nameTooltip);
        }
    }
    QStandardItem* typeItem = new QStandardItem(type);
    typeItem->setEditable(false);
    typeItem->setToolTip(type);
    row << typeItem;


    parentItem->appendRow(row);

    std::set<const core::RepoNodeAbstract*> children = node->getChildren();
    std::set<const core::RepoNodeAbstract*>::iterator it;

    for (it = children.begin(); it != children.end(); ++it)
    {
        addNode(nameItem, *it);
    }
}
