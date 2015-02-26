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

    //qRegisterMetaType<const core::RepoNodeAbstract*>("const core::RepoNodeAbstract*");

    this->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(glcWidget, SIGNAL(destroyed()),
                     this, SLOT(close()));

    this->setWindowTitle(windowTitle() + ": " + glcWidget->windowTitle());

    QList<QString> headers;
    headers << tr("Name");
    headers << tr("Type");
    headers << tr("Unique ID");
    headers << tr("Shared ID");
    ui->filterableTreeWidget->setHeaders(headers);
    ui->filterableTreeWidget->setProxyModel(new RepoSortFilterProxyModel(this, true));
    ui->filterableTreeWidget->setExtendedSelection();

    const core::RepoGraphScene* repoScene = glcWidget->getRepoScene();
    if (repoScene)
    {
        addNode(ui->filterableTreeWidget->getModel()->invisibleRootItem(),
            repoScene->getRoot());
    }

    QObject::connect(ui->filterableTreeWidget->getSelectionModel(),
                     SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                     this, SLOT(changeSelection(QItemSelection, QItemSelection)));

    QObject::connect(ui->filterableTreeWidget->getModel(),
                     SIGNAL(itemChanged(QStandardItem*)),
                     this, SLOT(changeItem(QStandardItem*)));
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
    nameItem->setToolTip(QString::fromStdString(node->toString()));
    nameItem->setEditable(false);
    nameItem->setCheckable(true);
    nameItem->setData(qVariantFromValue((void *) node));
    nameItem->setCheckState(Qt::Checked);
    row << nameItem;

    // Type
    QString type = QString::fromStdString(node->getType());
    QStandardItem* typeItem = new QStandardItem(type);
    typeItem->setEditable(false);
    typeItem->setToolTip(type);
    row << typeItem;


    // Unique ID
    QString uid = QString::fromStdString(
                core::MongoClientWrapper::uuidToString(node->getUniqueID()));
    QStandardItem* uidItem = new QStandardItem(uid);
    uidItem->setEditable(false);
    uidItem->setToolTip(uid);
    row << uidItem;

    // Shared ID
    QString sid = QString::fromStdString(
                core::MongoClientWrapper::uuidToString(node->getSharedID()));
    QStandardItem* sidItem = new QStandardItem(sid);
    sidItem->setEditable(false);
    sidItem->setToolTip(sid);
    row << sidItem;

    parentItem->appendRow(row);

    std::set<const core::RepoNodeAbstract*> children = node->getChildren();
    std::set<const core::RepoNodeAbstract*>::iterator it;
    for (it = children.begin(); it != children.end(); ++it)
    {
        addNode(nameItem, *it);
    }
}

void repo::gui::RepoSelectionTreeDockWidget::changeItem(QStandardItem* item)
{

    QObject::disconnect(ui->filterableTreeWidget->getModel(),
                     SIGNAL(itemChanged(QStandardItem*)),
                     this, SLOT(changeItem(QStandardItem*)));

    switch(item->checkState())
    {
        case Qt::Checked :
            std::cerr << "checked" << std::endl;
            break;
        case Qt::Unchecked :
            std::cerr << "unchecked" << std::endl;
            break;
        case Qt::PartiallyChecked :
            std::cerr << "partially checked" << std::endl;
            break;
    }

    QObject::connect(ui->filterableTreeWidget->getModel(),
                     SIGNAL(itemChanged(QStandardItem*)),
                     this, SLOT(changeItem(QStandardItem*)));
}

void repo::gui::RepoSelectionTreeDockWidget::changeSelection(
        const QItemSelection& selection,
        bool unselectSelected)
{
    QList<QModelIndex> selectionIndexes = selection.indexes();
    for (int i = 0; i < selectionIndexes.size(); ++i)
    {
        QModelIndex index = selectionIndexes[i];
        if (0 == index.column()) // Name column
        {
            core::RepoNodeAbstract* node =
                (core::RepoNodeAbstract*)index.data(Qt::UserRole+1).value<void*>();
           select(node, unselectSelected);
        }
    }
}

void repo::gui::RepoSelectionTreeDockWidget::changeSelection(
        const QItemSelection& selected, const QItemSelection& deselected)
{
    changeSelection(deselected, true);
    changeSelection(selected, false);
    glcWidget->repaint();
}

void repo::gui::RepoSelectionTreeDockWidget::select(
        const core::RepoNodeAbstract* node,
        bool unselectSelected)
{

    if (node)
    {
        if (REPO_NODE_TYPE_MESH == node->getType())
            glcWidget->select(QString::fromStdString(node->getName()), true, unselectSelected, false);

//        std::set<const core::RepoNodeAbstract*> children = node->getChildren();
//        std::set<const core::RepoNodeAbstract*>::iterator it;
//        for (it = children.begin(); it != children.end(); ++it)
//           select(*it, unselectSelected);
    }
}

















