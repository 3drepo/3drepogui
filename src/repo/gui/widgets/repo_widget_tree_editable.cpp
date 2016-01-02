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

using namespace repo::gui;
using namespace repo::gui::widget;

EditableTreeWidget::EditableTreeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditableTreeWidget)
{
    ui->setupUi(this);
    ui->filterableTreeWidget->setMargins(0);

    QObject::connect(
        ui->filterableTreeWidget->getSelectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &EditableTreeWidget::select);

    QObject::connect(
        ui->addPushButton, SIGNAL(pressed()),
                this, SLOT(addItem()));

    QObject::connect(
        ui->copyPushButton, SIGNAL(pressed()),
                this, SLOT(copyItem()));

    QObject::connect(
        ui->removePushButton, SIGNAL(pressed()),
                this, SLOT(removeItem()));

    QObject::connect(ui->editPushButton, SIGNAL(pressed()),
                     this, SLOT(edit()));

    QObject::connect(ui->filterableTreeWidget->getTreeView(),
                     SIGNAL(doubleClicked(const QModelIndex &)),
                     this, SLOT(edit(const QModelIndex &)));

    QObject::connect(ui->filterableTreeWidget->getTreeView(),
                     SIGNAL(customContextMenuRequested(const QPoint &)),
                     this, SLOT(showCustomContextMenu(const QPoint&)));
}

EditableTreeWidget::~EditableTreeWidget()
{
    delete ui;
}

void EditableTreeWidget::clear()
{
    ui->filterableTreeWidget->clear();
    ui->removePushButton->setEnabled(false);
    ui->editPushButton->setEnabled(false);
    ui->copyPushButton->setEnabled(false);

    emit editButtonsEnabledChanged(false);
}

void EditableTreeWidget::select(
        const QItemSelection &,
        const QItemSelection &)
{
    ui->removePushButton->setEnabled(true);
    ui->editPushButton->setEnabled(true);
    ui->copyPushButton->setEnabled(true);

    emit editButtonsEnabledChanged(true);
}

void EditableTreeWidget::showCustomContextMenu(const QPoint &point)
{
    QTreeView *treeView = ui->filterableTreeWidget->getTreeView();
    QMenu menu(treeView);

    QAction *addAction = menu.addAction(
                ui->addPushButton->text(),
                this,
                SLOT(showEditDialog()));
    addAction->setEnabled(ui->addPushButton->isEnabled());

    QAction *removeAction = menu.addAction(
                ui->removePushButton->text(),
                this,
                SLOT(removeItem()));
    removeAction->setEnabled(ui->removePushButton->isEnabled());

    QAction *editAction = menu.addAction(
                ui->editPushButton->text(),
                this,
                SLOT(edit()));
    editAction->setEnabled(ui->editPushButton->isEnabled());

    QAction *copyAction = menu.addAction(
                ui->copyPushButton->text(),
                this,
                SLOT(copyItem()));
    copyAction->setEnabled(ui->copyPushButton->isEnabled());
    menu.exec(treeView->mapToGlobal(point));
}

FilterableTreeWidget* EditableTreeWidget::getFilterableTree() const
{
    return ui->filterableTreeWidget;
}
