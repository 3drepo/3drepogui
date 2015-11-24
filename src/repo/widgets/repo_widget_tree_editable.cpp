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


#include "repo_widget_tree_editable.h"
#include "ui_repo_widget_tree_editable.h"

using namespace repo::widgets;

RepoWidgetTreeEditable::RepoWidgetTreeEditable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepoWidgetTreeEditable)
{
    ui->setupUi(this);
    ui->filterableTreeWidget->setMargins(0);

    QObject::connect(
        ui->filterableTreeWidget->getSelectionModel(), &QItemSelectionModel::selectionChanged,
        this, &RepoWidgetTreeEditable::select);

    QObject::connect(
        ui->addPushButton, SIGNAL(pressed()),
                this, SLOT(showEditDialog()));

    QObject::connect(
        ui->removePushButton, SIGNAL(pressed()),
                this, SLOT(removeItem()));

    QObject::connect(ui->editPushButton, SIGNAL(pressed()),
                     this, SLOT(edit()));

    QObject::connect(ui->filterableTreeWidget->getTreeView(),
                     SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(edit(const QModelIndex &)));


//    QObject::connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
//                     this, SLOT(showCustomContextMenu(const QPoint&)));

    // FIXME: restore
//    QSettings settings(this->parentWidget());
//    ui->treeView->header()->restoreState(
//                settings.value(COLUMNS_SETTINGS).toByteArray());
}

RepoWidgetTreeEditable::~RepoWidgetTreeEditable()
{
    // FIXME: restore
//    QSettings settings(this->parentWidget());
//    settings.setValue(COLUMNS_SETTINGS, ui->treeView->header()->saveState());
    //--------------------------------------------------------------------------

    delete ui;
}

void RepoWidgetTreeEditable::clear()
{
    ui->filterableTreeWidget->clear();
    ui->removePushButton->setEnabled(false);
    ui->editPushButton->setEnabled(false);
}

void RepoWidgetTreeEditable::select(
        const QItemSelection &,
        const QItemSelection &)
{
    ui->removePushButton->setEnabled(true);
    ui->editPushButton->setEnabled(true);
}

void RepoWidgetTreeEditable::showCustomContextMenu(const QPoint &point)
{
//    QMenu menu(ui->treeView);

//    QAction *addAction = menu.addAction(
//                ui->addPushButton->text(),
//                this,
//                SLOT(showEditDialog()));
//    addAction->setEnabled(ui->addPushButton->isEnabled());

//    QAction *removeAction = menu.addAction(
//                ui->removePushButton->text(),
//                this,
//                SLOT(removeItem()));
//    removeAction->setEnabled(ui->removePushButton->isEnabled());

//    QAction *editAction = menu.addAction(
//                ui->editPushButton->text(),
//                this,
//                SLOT(edit()));
//    editAction->setEnabled(ui->editPushButton->isEnabled());

//    menu.exec(ui->treeView->mapToGlobal(point));

}

RepoWidgetTreeFilterable* RepoWidgetTreeEditable::getFilterableTree() const
{
    return ui->filterableTreeWidget;
}
