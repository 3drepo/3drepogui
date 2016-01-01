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


#include "reposelectiontreedockwidget.h"
#include "ui_reposelectiontreedockwidget.h"
#include "../primitives/repo_sortfilterproxymodel.h"

repo::gui::RepoSelectionTreeDockWidget::RepoSelectionTreeDockWidget(
        widgets::RepoRenderingWidget *glcWidget,
        QWidget *parent)
    : QDockWidget(parent)
    , glcWidget(glcWidget)
    , ui(new Ui::RepoSelectionTreeDockWidget)
{
    ui->setupUi(this);

    //qRegisterMetaType<const core::RepoNodeAbstract*>("const core::RepoNodeAbstract*");

    this->setAttribute(Qt::WA_DeleteOnClose);

    //const core::RepoGraphScene* repoScene = 0;
    if (glcWidget)
    {
        QObject::connect(glcWidget, SIGNAL(destroyed()),
                         this, SLOT(close()));
        this->setWindowTitle(windowTitle() + ": " + glcWidget->windowTitle());
        //repoScene = glcWidget->getRepoScene();
    }

    QList<QString> headers;
    headers << tr("Name");
    headers << tr("Type");
    headers << tr("Unique ID");
    headers << tr("Shared ID");
    //ui->filterableTreeWidget->setHeaders(headers);
    //ui->filterableTreeWidget->setProxyModel(new RepoSortFilterProxyModel(this, true));
    //ui->filterableTreeWidget->setExtendedSelection();
    //ui->filterableTreeWidget->getTreeView()->setExpandsOnDoubleClick(false);

    //if (repoScene)
    //{
    //    // TODO: make asynchronous
    //    addNode(ui->filterableTreeWidget->getModel()->invisibleRootItem(),
    //        repoScene->getRoot());
    //}

  /*  QObject::connect(ui->filterableTreeWidget->getSelectionModel(),
                     SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                     this, SLOT(changeSelection(QItemSelection, QItemSelection)));

    QObject::connect(ui->filterableTreeWidget->getModel(),
                     SIGNAL(itemChanged(QStandardItem*)),
                     this, SLOT(changeItem(QStandardItem*)));

    QObject::connect(
                ui->filterableTreeWidget->getTreeView(),
                SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(editItem(QModelIndex)));

    QObject::connect(
                ui->filterableTreeWidget->getTreeView(),
                SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(showContextMenu(QPoint)));*/
}

repo::gui::RepoSelectionTreeDockWidget::~RepoSelectionTreeDockWidget()
{
    delete ui;
}


//void repo::gui::RepoSelectionTreeDockWidget::addNode(
//        QStandardItem* parentItem,
//        const core::RepoNodeAbstract* node)
//{
//    QString name;
//    if (!node->getName().empty())
//        name = QString::fromStdString(node->getName());
//    else
//        name = tr("<empty>"); //QString::fromStdString(core::MongoClientWrapper::uuidToString(node->getUniqueID()));
//
//    QList<QStandardItem*> row;
//    QString type = QString::fromStdString(node->getType());
//
//    // Name
//    QStandardItem* nameItem = new QStandardItem(name);
//    nameItem->setToolTip(QString::fromStdString(node->toString()));
//    nameItem->setEditable(false);
//    nameItem->setCheckable(true);
//    nameItem->setData(qVariantFromValue((void *) node));
//    nameItem->setCheckState(Qt::Checked);
//
//    // Icon
//    if (REPO_NODE_TYPE_METADATA == type.toStdString())
//        nameItem->setIcon(RepoFontAwesome::getMetadataIcon());
//
//    row << nameItem;
//
//    // Type
//    QStandardItem* typeItem = new QStandardItem(type);
//    typeItem->setEditable(false);
//    typeItem->setToolTip(type);
//    row << typeItem;
//
//    // Unique ID
//    QString uid = QString::fromStdString(
//                core::MongoClientWrapper::uuidToString(node->getUniqueID()));
//    QStandardItem* uidItem = new QStandardItem(uid);
//    uidItem->setEditable(false);
//    uidItem->setToolTip(uid);
//    row << uidItem;
//
//    // Shared ID
//    QString sid = QString::fromStdString(
//                core::MongoClientWrapper::uuidToString(node->getSharedID()));
//    QStandardItem* sidItem = new QStandardItem(sid);
//    sidItem->setEditable(false);
//    sidItem->setToolTip(sid);
//    row << sidItem;
//
//    parentItem->appendRow(row);
//
//    // Recursion
//    std::set<const core::RepoNodeAbstract*> children = node->getChildren();
//    std::set<const core::RepoNodeAbstract*>::iterator it;
//    for (it = children.begin(); it != children.end(); ++it)
//        addNode(nameItem, *it);
//}

void repo::gui::RepoSelectionTreeDockWidget::attachPDF()
{
    //QModelIndexList currentSelection = ui->filterableTreeWidget->getCurrentSelection();

  /*  if (currentSelection.size() > 0)
    {
        QStringList filePaths = QFileDialog::getOpenFileNames(
                    this->parentWidget(),
                    tr("Select one or more files to open"),
                    QString::null,
                    "*.pdf");*/
        //core::RepoGraphScene *repoScene = glcWidget->getRepoScene();
//        for (QString path : filePaths)
//        {
//            QFileInfo fileInfo(path);
//          /*  core::RepoBinary bin(path.toStdString(), REPO_MEDIA_TYPE_PDF);
//            core::RepoNodeMetadata *meta = new core::RepoNodeMetadata(bin, fileInfo.baseName().toStdString(), REPO_MEDIA_TYPE_PDF);
//
//            for (QModelIndex sourceIndex : currentSelection)
//            {
//                if (sourceIndex.column() == Columns::NAME)
//                {
//                    QStandardItem *item = ui->filterableTreeWidget->getItemFromSource(sourceIndex);
//                    core::RepoNodeAbstract *parent = getNode(item);
//                    parent->addChild(meta);
//                    meta->addParent(parent);
//                    addNode(item, meta);
//                }
//            }
//            repoScene->addNodeByUniqueID(meta);
//            repoScene->addMetadata(meta);
//*/        }
//    }
}

void repo::gui::RepoSelectionTreeDockWidget::changeItem(QStandardItem* item)
{
  /*  QObject::disconnect(ui->filterableTreeWidget->getModel(),
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
    }*/

    //QObject::connect(ui->filterableTreeWidget->getModel(),
    //                 SIGNAL(itemChanged(QStandardItem*)),
    //                 this, SLOT(changeItem(QStandardItem*)));
}

void repo::gui::RepoSelectionTreeDockWidget::changeSelection(
        const QItemSelection& selection,
        bool unselectSelected)
{
    QList<QModelIndex> selectionIndexes = selection.indexes();
    for (int i = 0; i < selectionIndexes.size(); ++i)
    {
        QModelIndex index = selectionIndexes[i];
        if (Columns::NAME == index.column()) // Name column
        {
            //core::RepoNodeAbstract* node =
            //    (core::RepoNodeAbstract*)index.data(Qt::UserRole+1).value<void*>();
           //select(node, unselectSelected);
        }
    }
}

void repo::gui::RepoSelectionTreeDockWidget::changeSelection(
        const QItemSelection& selected, const QItemSelection& deselected)
{
    changeSelection(deselected, true);
    changeSelection(selected, false);
    if (glcWidget)
        glcWidget->repaint();
}

void repo::gui::RepoSelectionTreeDockWidget::editItem(const QModelIndex &) const
{
   editSelectedItems();
}

void repo::gui::RepoSelectionTreeDockWidget::editSelectedItems() const
{
   /* std::string type = getType(ui->filterableTreeWidget->getCurrentItem(Columns::NAME));*/
    /*if (REPO_NODE_TYPE_TRANSFORMATION == type)
        editSelectedTransformations();*/
}

void repo::gui::RepoSelectionTreeDockWidget::editSelectedTransformations() const
{
   /* core::RepoNodeTransformation *t = getTransformation(ui->filterableTreeWidget->getCurrentItem(Columns::NAME));
    TransformationDialog transformationDialog(t ? *t : core::RepoNodeTransformation(),
                                                  this->parentWidget());
    if (transformationDialog.exec())
    {
        core::RepoNodeTransformation newT = transformationDialog.getTransformation();
        for (QModelIndex selectedIndex : ui->filterableTreeWidget->getCurrentSelection())
        {
            core::RepoNodeTransformation *transformation = getTransformationFromSource(selectedIndex);
            if (transformation)
            {
                transformation->setName(newT.getName());
                transformation->setMatrix(newT.getMatrix());
                ui->filterableTreeWidget->getModel()->setData(
                            selectedIndex.sibling(selectedIndex.row(), Columns::NAME),
                            QString::fromStdString(newT.getName()));
            }
        }
    }*/
}


//void repo::gui::RepoSelectionTreeDockWidget::select(
//        const core::RepoNodeAbstract* node,
//        bool unselectSelected)
//{
//
//    if (node)
//    {
//        if (REPO_NODE_TYPE_MESH == node->getType() && glcWidget)
//            glcWidget->select(QString::fromStdString(node->getName()), true, unselectSelected, false);
//
////        std::set<const core::RepoNodeAbstract*> children = node->getChildren();
////        std::set<const core::RepoNodeAbstract*>::iterator it;
////        for (it = children.begin(); it != children.end(); ++it)
////           select(*it, unselectSelected);
//    }
//}

void repo::gui::RepoSelectionTreeDockWidget::showContextMenu(const QPoint &point)
{
 //   bool on = ui->filterableTreeWidget->getModel()->invisibleRootItem()->rowCount() > 0;
	//bool isTransformationSelected = false;
	//	/*REPO_NODE_TYPE_TRANSFORMATION ==
 //           getType(ui->filterableTreeWidget->getCurrentItem(Columns::NAME));*/

 //   QTreeView *treeView = ui->filterableTreeWidget->getTreeView();
 //   QMenu menu(treeView);
 //   QAction *action = menu.addAction(
 //               tr("Edit Transformation..."),
 //               this,
 //               SLOT(editSelectedItems()));
 //   action->setEnabled(on && isTransformationSelected);
 //   menu.addSeparator();
 //   QAction *remove = menu.addAction(
 //               tr("Attach PDF..."),
 //               this,
 //               SLOT(attachPDF()));
 //   remove->setEnabled(on);
 //   menu.exec(treeView->mapToGlobal(point));
}


//repo::core::RepoNodeTransformation *repo::gui::RepoSelectionTreeDockWidget::getTransformationFromSource(
//        const QModelIndex &sourceIndex) const
//{
//    return getTransformation(ui->filterableTreeWidget->getItemFromSource(sourceIndex, Columns::NAME));
//}
//
//repo::core::RepoNodeTransformation *repo::gui::RepoSelectionTreeDockWidget::getTransformationFromProxy(
//        const QModelIndex &proxyIndex) const
//{
//    return getTransformation(ui->filterableTreeWidget->getItemFromProxy(proxyIndex, Columns::NAME));
//}
//
//repo::core::RepoNodeAbstract *repo::gui::RepoSelectionTreeDockWidget::getNode(const QStandardItem * item) const
//{
//    core::RepoNodeAbstract *node = 0;
//    if (item)
//        node = (core::RepoNodeAbstract*)item->data(Qt::UserRole+1).value<void*>();
//    return node;
//}
//
//repo::core::RepoNodeTransformation *repo::gui::RepoSelectionTreeDockWidget::getTransformation(
//        const QStandardItem *item) const
//{
//    core::RepoNodeTransformation *transformation = 0;
//    core::RepoNodeAbstract* node = getNode(item);
//    if (node && node->getType() == REPO_NODE_TYPE_TRANSFORMATION)
//        transformation = dynamic_cast<core::RepoNodeTransformation*>(node);
//    return transformation;
//}
//
//std::string repo::gui::RepoSelectionTreeDockWidget::getType(const QStandardItem * item) const
//{
//    std::string type;
//    core::RepoNodeAbstract* node = getNode(item);
//    if (node)
//        type = node->getType();
//    return type;
//}











