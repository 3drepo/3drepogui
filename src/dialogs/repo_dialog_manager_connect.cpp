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
#include "ui_repoabstractmanagerdialog.h"



repo::gui::RepoDialogManagerConnect::RepoDialogManagerConnect(
        const RepoIDBCache *dbCache,
        QWidget *parent)
    : RepoAbstractManagerDialog(dbCache, parent)
{
    setWindowTitle("Connection Manager");
    setWindowIcon(RepoFontAwesome::getConnectIcon());

    //--------------------------------------------------------------------------
    // Connections table
    model->setColumnCount(5);
    model->setHeaderData(
                (int) Columns::ALIAS,
                Qt::Horizontal,
                tr("Alias"));
    model->setHeaderData(
                (int) Columns::HOST_PORT,
                Qt::Horizontal,
                tr("Address"));
    model->setHeaderData(
                (int) Columns::AUTHENTICATION,
                Qt::Horizontal,
                tr("Authentication"));
    model->setHeaderData(
                (int) Columns::SSL,
                Qt::Horizontal,
                tr("SSL"));
    model->setHeaderData(
                (int) Columns::SSH,
                Qt::Horizontal,
                tr("SSH"));

    ui->treeView->sortByColumn((int) Columns::ALIAS, Qt::SortOrder::AscendingOrder);
    ui->hostComboBox->hide();
    ui->databaseComboBox->hide();
}

void repo::gui::RepoDialogManagerConnect::addConnectionToken(const repo::RepoToken &connectionToken)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // Token object itself
    QVariant var;
    var.setValue(connectionToken);

    QStandardItem *item = new QStandardItem();
    item->setData(var);
    item->setEnabled(true);
    item->setTristate(false);
    item->setEditable(false);
    row.append(item);

    // Address
    row.append(createItem(QString::fromStdString(connectionToken.getDatabaseHostPort())));

    // Authentication
    row.append(createItem(QString::fromStdString(connectionToken.getDatabaseName())));

    row.append(createItem(QVariant(false)));
    row.append(createItem(QVariant(false)));


    //--------------------------------------------------------------------------
    model->invisibleRootItem()->appendRow(row);
}

void repo::gui::RepoDialogManagerConnect::edit()
{
    showEditDialog(getConnection());
}

void repo::gui::RepoDialogManagerConnect::edit(const QModelIndex &index)
{
    showEditDialog(getConnection(index));
}

repo::RepoToken repo::gui::RepoDialogManagerConnect::getConnection()
{
    return getConnection(ui->treeView->selectionModel()->currentIndex());
}

repo::RepoToken repo::gui::RepoDialogManagerConnect::getConnection(const QModelIndex &index)
{
    repo::RepoToken token;
    if (index.isValid())
    {
        QModelIndex tokenIndex = index.sibling(index.row(), (int) Columns::ALIAS);
        return tokenIndex.data(Qt::UserRole + 1).value<repo::RepoToken>();
    }
    return token;
}

void repo::gui::RepoDialogManagerConnect::refresh()
{
    if (cancelAllThreads())
    {
        std::cout << "Refresh called" << std::endl;
        ui->progressBar->show(); // TODO: delete line
        //----------------------------------------------------------------------
        clear(false); // Clear any previous entries


        // TODO: new worker here

        repo::RepoToken dbToken(0, "localhost", "admin");
        addConnectionToken(dbToken);

        //----------------------------------------------------------------------
        ui->progressBar->hide(); // TODO: show
    }
}


void repo::gui::RepoDialogManagerConnect::showEditDialog(const repo::RepoToken &token)
{

    RepoDialogConnect connectionSettingsDialog(this);
    if (QDialog::Rejected == connectionSettingsDialog.exec())
    {
        repoLog("Connection Settings Dialog cancelled by user.\n");
        std::cout << tr("Connection Settings Dialog cancelled by user.").toStdString() << std::endl;
    }
    else // QDialog::Accepted
    {

        repoLog("create or update user...\n");
        // Create or update user

        // TODO: SAVE
    }
}
