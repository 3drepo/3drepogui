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


#include "repofederationdialog.h"
#include "ui_repofederationdialog.h"

#include "../../../primitives/repo_fontawesome.h"
#include "../../../primitives/repocomboboxdelegate.h"
#include "../../logger/repo_logger.h"

using namespace repo::gui;

RepoFederationDialog::RepoFederationDialog(
        RepoIDBCache *dbCache,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RepoFederationDialog)
    , dbCache(dbCache)

{
    ui->setupUi(this);
    ui->availableWidget->setExpandedUI();
    ui->availableWidget->setExtendedSelection();
    ui->availableWidget->setRootIsDecorated(true);

    ui->federatedWidget->setExpandedUI();
    ui->federatedWidget->setExtendedSelection();
    ui->federatedWidget->setRootIsDecorated(true);

//    ui->federatedWidget->getTreeView()->setDragEnabled(true);
//    ui->federatedWidget->getTreeView()->viewport()->setAcceptDrops(true);
//    ui->federatedWidget->getTreeView()->setDropIndicatorShown(true);
//    ui->federatedWidget->getTreeView()->setDragDropMode(QAbstractItemView::InternalMove);

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText(tr("Next"));

    //--------------------------------------------------------------------------

    dbCache->setHostsComboBox(ui->hostComboBox);
    dbCache->setDatabasesComboBox(ui->databaseComboBox);

    //--------------------------------------------------------------------------

    ui->addPushButton->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_arrow_right));
    ui->removePushButton->setIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_arrow_left));


    QStandardItemModel *availableModel = ui->availableWidget->getModel();
    availableModel->setColumnCount(1);
    availableModel->setHeaderData(
                0,
                Qt::Horizontal,
                tr("Project"));


    QStandardItemModel *federatedModel = ui->federatedWidget->getModel();
    federatedModel->setColumnCount(3);
    federatedModel->setHeaderData(
                0,
                Qt::Horizontal,
                tr("Project"));
    federatedModel->setHeaderData(
                1,
                Qt::Horizontal,
                tr("Branch"));
    federatedModel->setHeaderData(
                2,
                Qt::Horizontal,
                tr("Revision"));

    QObject::connect(ui->refreshPushButton, SIGNAL(pressed()),
                     this, SLOT(refresh()));

    QObject::connect(ui->refreshPushButton, SIGNAL(pressed()),
                     this, SLOT(refreshCache()));

    QObject::connect(ui->databaseComboBox, SIGNAL(currentIndexChanged(const QString &)),
                     this, SLOT(refresh()));

    QObject::connect(ui->addPushButton, SIGNAL(pressed()),
                     this, SLOT(addProjectsToFederation()));

    QObject::connect(ui->removePushButton, SIGNAL(pressed()),
                     this, SLOT(removeProjectsFromFederation()));

    QObject::connect(ui->federatedWidget->getTreeView(), SIGNAL(customContextMenuRequested(QPoint)),
        this, SLOT(showFederationMenu(QPoint)));
}

RepoFederationDialog::~RepoFederationDialog()
{
    delete ui;
}

void RepoFederationDialog::addAvailableProject(const QString &project)
{
    QStandardItem *item = new QStandardItem(project);
    item->setEditable(false);
    item->setToolTip(project);
    ui->availableWidget->addTopLevelRow(item);
}


void RepoFederationDialog::addProjectsToFederation()
{
    QStandardItem *item = getCurrentFederatedItem();
    if (item)
    {
        std::string database = ui->databaseComboBox->currentText().toStdString();
        for (QModelIndex selectedIndex : getAvailableSelection())
        {            
            QString project = selectedIndex.data().toString();

            //------------------------------------------------------------------

            RepoTransRefPair p;
			p.first = repo::core::model::RepoBSONFactory::makeTransformationNode();
			p.second = repo::core::model::RepoBSONFactory::makeReferenceNode(database, project.toStdString());
            QVariant var;
            var.setValue(p);

            //------------------------------------------------------------------

            QList<QStandardItem*> row;            
            row << new repo::primitives::RepoStandardItem(project);
            row[0]->setData(var);

            row << new repo::primitives::RepoStandardItem(QString("master"));
            row[1]->setEditable(false); //TODO: this is set to false because it does nothing even edited.

            row << new repo::primitives::RepoStandardItem(QString("head"));
			row[2]->setEditable(false); //TODO: this is set to false because it does nothing even edited.
            item->appendRow(row);
        }
        ui->federatedWidget->expandItem(item);
    }
}

int RepoFederationDialog::exec()
{
    refresh();
    return QDialog::exec();
}

void RepoFederationDialog::refresh()
{
    ui->availableWidget->clear();

    // TODO: make multithreaded
    QStringList availableProjects = dbCache->getProjects(ui->hostComboBox->currentText(), ui->databaseComboBox->currentText());

    for (QString project : availableProjects)
        addAvailableProject(project);
}


void RepoFederationDialog::removeProjectsFromFederation()
{
    QStandardItemModel *federatedModel = ui->federatedWidget->getModel();
    QModelIndexList selectedIndexes = getFederatedSelection();

    bool isRemoved = true;
    while (!selectedIndexes.empty() && isRemoved)
    {
        QModelIndex selectedIndex = selectedIndexes[0];
        isRemoved = federatedModel->removeRow(selectedIndex.row(), selectedIndex.parent());
        selectedIndexes = getFederatedSelection();
    }
}

void RepoFederationDialog::showFederationMenu(const QPoint &point)
{
    bool on = ui->federatedWidget->getModel()->invisibleRootItem()->rowCount() > 0;
    QTreeView *treeView = ui->federatedWidget->getTreeView();
    QMenu menu(treeView);

    QAction *action = menu.addAction(
                tr("Transformation..."),
                this,
                SLOT(showTransformationDialog()));
    action->setEnabled(on);

    menu.addSeparator();

    QAction *remove = menu.addAction(
                tr("Remove"),
                this,
                SLOT(removeProjectsFromFederation()));
    remove->setEnabled(on);

    menu.exec(treeView->mapToGlobal(point));
}

void RepoFederationDialog::showTransformationDialog()
{
    RepoTransformationDialog d(getCurrentFederatedTransformation(), this);
    if (d.exec())
    {
		repo::core::model::TransformationNode transformation = d.getTransformation();
        for (QModelIndex selectedIndex : getFederatedSelection())
        {
            RepoTransRefPair p =
                    selectedIndex.data(Qt::UserRole+1).value<RepoTransRefPair>();

            p.first = transformation;

            QVariant var;
            var.setValue(p);
            ui->federatedWidget->getModel()->setData(selectedIndex, var,Qt::UserRole+1);
        }
    }
}


QStandardItem *RepoFederationDialog::getCurrentFederatedItem() const
{
    QModelIndex proxyCurrent = ui->federatedWidget->getSelectionModel()->currentIndex();
    QStandardItem *item = 0;
    if (proxyCurrent.isValid())
    {
        QModelIndex modelCurrent = ui->federatedWidget->getProxyModel()->mapToSource(proxyCurrent);
        QModelIndex modelCurrentFirstColumn = modelCurrent.sibling(modelCurrent.row(), Columns::PROJECT);
        item = ui->federatedWidget->getModel()->itemFromIndex(modelCurrentFirstColumn);
    }
    else
        item = ui->federatedWidget->getModel()->invisibleRootItem();
    return item;
}

repo::core::model::TransformationNode RepoFederationDialog::getCurrentFederatedTransformation() const
{
    QStandardItem *item = getCurrentFederatedItem();
	repo::core::model::TransformationNode transformation;
    if (item)
    {
        RepoTransRefPair p =
            getCurrentFederatedItem()->data(Qt::UserRole+1).value<RepoTransRefPair>();
        transformation = p.first;
    }
    return transformation;
}

//------------------------------------------------------------------------------

QModelIndexList RepoFederationDialog::getAvailableSelection() const
{
    return ui->availableWidget->getCurrentSelection();
}

QModelIndexList RepoFederationDialog::getFederatedSelection() const
{
    return ui->federatedWidget->getCurrentSelection();
}

std::map<repo::core::model::TransformationNode, repo::core::model::ReferenceNode> RepoFederationDialog::getFederation()
{

	return getFederationRecursively(ui->federatedWidget->getModel()->invisibleRootItem());
}

std::map<repo::core::model::TransformationNode, repo::core::model::ReferenceNode>
	RepoFederationDialog::getFederationRecursively(
        QStandardItem *parentItem)
{
	std::map<repo::core::model::TransformationNode, repo::core::model::ReferenceNode> fedMap;
	if (parentItem)
	{
		for (int i = 0; i < parentItem->rowCount(); ++i)
		{
			QStandardItem *childItem = parentItem->child(i, Columns::PROJECT);

			RepoTransRefPair p =
				childItem->data(Qt::UserRole + 1).value<RepoTransRefPair>();
			fedMap[p.first] = p.second;
			//------------------------------------------------------------------
			// Recursive call (base case is not having any more children (rows)
			std::map<repo::core::model::TransformationNode, repo::core::model::ReferenceNode>  childmap
				= getFederationRecursively(childItem);
			fedMap.insert(childmap.begin(), childmap.end());
		}
	}

	return fedMap;
}
