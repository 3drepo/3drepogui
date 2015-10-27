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

    ui->buttonBox->addButton("Connect", QDialogButtonBox::AcceptRole);
}

void repo::gui::RepoDialogManagerConnect::addCredentials(const RepoCredentials &credentials)
{
    QList<QStandardItem *> row;
    //--------------------------------------------------------------------------
    // Token object itself
    QVariant var;
    var.setValue(credentials);

    QStandardItem *item = createItem(QString::fromStdString(credentials.getAlias()));
    item->setData(var);
    item->setEnabled(true);
    row.append(item);

    // Address
    row.append(createItem(QString::fromStdString(credentials.getHostAndPort())));

    // Authentication
    row.append(createItem(QString::fromStdString(credentials.getUsername())));

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

repo::RepoCredentials repo::gui::RepoDialogManagerConnect::getConnection()
{
    return getConnection(ui->treeView->selectionModel()->currentIndex());
}

repo::RepoCredentials repo::gui::RepoDialogManagerConnect::getConnection(const QModelIndex &index)
{
    repo::RepoCredentials token;
    if (index.isValid())
    {
        QModelIndex tokenIndex = index.sibling(index.row(), (int) Columns::ALIAS);
        return tokenIndex.data(Qt::UserRole + 1).value<repo::RepoCredentials>();
    }
    return token;
}

void repo::gui::RepoDialogManagerConnect::refresh()
{
    if (cancelAllThreads())
    {
        ui->progressBar->show(); // TODO: delete line
        //----------------------------------------------------------------------
        clear(false); // Clear any previous entries


        // TODO: new worker here
        repo::settings::RepoSettingsCredentials settings;
        QList<repo::RepoCredentials> credentialsList = settings.readCredentials();
        for (int i = 0; i < credentialsList.size() < i; ++i)
        {
            addCredentials(credentialsList[i]);
        }

        //----------------------------------------------------------------------
        ui->progressBar->hide(); // TODO: show
    }
}


void repo::gui::RepoDialogManagerConnect::showEditDialog(const repo::RepoCredentials &credentials)
{
    RepoDialogConnect connectionSettingsDialog(credentials, this);
    if (QDialog::Rejected == connectionSettingsDialog.exec())
        repoLog("Connection Settings Dialog cancelled by user.\n");
    else // QDialog::Accepted
    {
        repoLog("create or update connection settings...\n");

        repo::RepoCredentials credentials = connectionSettingsDialog.getConnectionSettings();

        repo::settings::RepoSettingsCredentials credentialsSettings;

        QList<repo::RepoCredentials> list;
        list.append(credentials);
        credentialsSettings.writeCredentials(list);

        // TODO: SAVE
    }
}
