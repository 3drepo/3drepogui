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

#include "../primitives/repo_fontawesome.h"
#include "../primitives/repocomboboxdelegate.h"

repo::gui::RepoFederationDialog::RepoFederationDialog(
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



}

repo::gui::RepoFederationDialog::~RepoFederationDialog()
{
    delete ui;
}

void repo::gui::RepoFederationDialog::addAvailableProject(const QString &project)
{
    QStandardItem *item = new QStandardItem(project);
    item->setEditable(false);
    item->setToolTip(project);
    ui->availableWidget->addTopLevelRow(item);
}


void repo::gui::RepoFederationDialog::addProjectsToFederation()
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
            p.first = new core::RepoNodeTransformation();
            p.second = new core::RepoNodeReference(database, project.toStdString());
            QVariant var;
            var.setValue(p);

            //------------------------------------------------------------------

            QList<QStandardItem*> row;            
            row << RepoFilterableTreeWidget::createItem(project);
            row[0]->setData(var);

            row << RepoFilterableTreeWidget::createItem("master");
            row[1]->setEditable(true);

            row << RepoFilterableTreeWidget::createItem("head");
            row[2]->setEditable(true);
            item->appendRow(row);
        }
        ui->federatedWidget->expandItem(item);
    }
}

int repo::gui::RepoFederationDialog::exec()
{
    refresh();
    return QDialog::exec();
}

void repo::gui::RepoFederationDialog::refresh()
{
    ui->availableWidget->clear();

    // TODO: make multithreaded
    QStringList availableProjects = dbCache->getProjects(ui->hostComboBox->currentText(), ui->databaseComboBox->currentText());
    for (QString project : availableProjects)
        addAvailableProject(project);
}


void repo::gui::RepoFederationDialog::removeProjectsFromFederation()
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


QStandardItem *repo::gui::RepoFederationDialog::getCurrentFederatedItem() const
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

//------------------------------------------------------------------------------

QModelIndexList repo::gui::RepoFederationDialog::getAvailableSelection() const
{
    return ui->availableWidget->getCurrentSelection();
}

QModelIndexList repo::gui::RepoFederationDialog::getFederatedSelection() const
{
    return ui->federatedWidget->getCurrentSelection();
}

repo::core::RepoGraphScene *repo::gui::RepoFederationDialog::getFederation()
{
    core::RepoGraphScene *scene = new core::RepoGraphScene();


    getFederationRecursively(ui->federatedWidget->getModel()->invisibleRootItem(),
                                    scene->getRoot());

    scene->getRoot()->setName("<root>");
    scene->printDAG();
    return scene;
}

void repo::gui::RepoFederationDialog::getFederationRecursively(
        QStandardItem *parentItem,
        core::RepoNodeAbstract *parentNode)
{
    if (parentItem && parentNode)
    {
        for (int i = 0; i < parentItem->rowCount(); ++i)
        {
            QStandardItem *childItem = parentItem->child(i, Columns::PROJECT);

            RepoTransRefPair p =
                    childItem->data(Qt::UserRole+1).value<RepoTransRefPair>();
            core::RepoNodeTransformation *t = p.first;
            parentNode->addChild(t);
            t->addParent(parentNode);
            t->setName("<identity>");

            core::RepoNodeReference *ref = p.second;
            t->addChild(ref);
            ref->addParent(t);

            //------------------------------------------------------------------
            // Recursive call (base case is not having any more children (rows)
            getFederationRecursively(childItem, t);
        }
    }
}
