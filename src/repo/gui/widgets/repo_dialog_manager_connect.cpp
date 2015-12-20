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


#include "repo_dialog_manager_connect.h"
#include "ui_repo_dialog_manager_connect.h"

using namespace repo::widgets;

RepoDialogManagerConnect::RepoDialogManagerConnect(
        repo::RepoController *controller,
        QWidget *parent)
    : QDialog(parent)
    , controller(controller)
    , ui(new Ui::RepoDialogManagerConnect)
{
    ui->setupUi(this);
    setWindowIcon(repo::gui::RepoFontAwesome::getConnectIcon());

//    ui->buttonBox->addButton(tr("Connect"), QDialogButtonBox::AcceptRole);

    QPushButton *connectPushButton = ui->buttonBox->button(QDialogButtonBox::Open);
    connectPushButton->setText(tr("Connect"));

    //--------------------------------------------------------------------------
    // Connect double click to accept action for convenient UI
    QTreeView *treeView = ui->connectionManagerWidget->getFilterableTree()->getTreeView();
    QObject::disconnect(treeView, SIGNAL(doubleClicked(const QModelIndex &)),
                      ui->connectionManagerWidget, SLOT(edit(const QModelIndex &)));

    QObject::connect(treeView,
                     &QTreeView::doubleClicked,
                     this,
                     &QDialog::accept);

    QObject::connect(ui->connectionManagerWidget,
                     &RepoWidgetTreeEditable::editButtonsEnabledChanged,
                     connectPushButton,
                     &QPushButton::setEnabled);

    ui->connectionManagerWidget->setController(controller);
}

RepoDialogManagerConnect::~RepoDialogManagerConnect()
{
    delete ui;
}

int RepoDialogManagerConnect::exec()
{
    refresh();
    return QDialog::exec();
}

void RepoDialogManagerConnect::refresh()
{
    ui->connectionManagerWidget->refresh();
}

repo::RepoCredentials RepoDialogManagerConnect::getConnection()
{
    return ui->connectionManagerWidget->getConnection();
}
