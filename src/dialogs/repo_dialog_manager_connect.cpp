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
        QWidget *parent)
    : RepoAbstractManagerDialog(0, parent)
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

//    ui->treeView->sortByColumn((int) Columns::ALIAS, Qt::SortOrder::AscendingOrder);

    ui->hostComboBox->hide();
    ui->databaseComboBox->hide();

    ui->buttonBox->addButton("Connect", QDialogButtonBox::AcceptRole);
}

void repo::gui::RepoDialogManagerConnect::addCredentials(const RepoCredentials &credentials)
{
    QList<QStandardItem *> row;
    row.append(getAliasItem(credentials));
    row.append(getAddressItem(credentials));
    row.append(getAuthenticationItem(credentials));

    // TODO: add support for SSL and SSH
    row.append(getSSLItem(credentials));
    row.append(getSSHItem(credentials));

    model->invisibleRootItem()->appendRow(row);
}

void repo::gui::RepoDialogManagerConnect::edit()
{
    showEditDialog(getConnection(), ui->treeView->selectionModel()->currentIndex());
}

void repo::gui::RepoDialogManagerConnect::edit(const QModelIndex &index)
{
    showEditDialog(getConnection(index), index);
}

repo::RepoCredentials repo::gui::RepoDialogManagerConnect::getConnection()
{
    return getConnection(ui->treeView->selectionModel()->currentIndex());
}

repo::RepoCredentials repo::gui::RepoDialogManagerConnect::getConnection(const QModelIndex &index)
{
    repo::RepoCredentials credentials;
    if (index.isValid())
    {
        QModelIndex credentialsIndex = index.sibling(index.row(), (int) Columns::ALIAS);
        credentials = credentialsIndex.data(Qt::UserRole + 1).value<repo::RepoCredentials>();
    }
    return credentials;
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
        for (int i = 0; i < credentialsList.size(); ++i)
        {
            addCredentials(credentialsList[i]);
        }

        //----------------------------------------------------------------------
        ui->progressBar->hide(); // TODO: show
    }
}

void repo::gui::RepoDialogManagerConnect::removeItem()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    int row = proxy->mapToSource(index).row();
    model->removeRow(row);
    sync();
    updateCountLabel();
}


void repo::gui::RepoDialogManagerConnect::showEditDialog(
        const repo::RepoCredentials &credentials,
        const QModelIndex &index)
{
    RepoDialogConnect connectionSettingsDialog(credentials, this);
    if (QDialog::Rejected == connectionSettingsDialog.exec())
        repoLog("Connection Settings Dialog cancelled by user.\n");
    else // QDialog::Accepted
    {
        repoLog("Create or update connection settings...\n");

        RepoCredentials credentials = connectionSettingsDialog.getConnectionSettings();
        if (!index.isValid())
        {
            addCredentials(credentials);
        }
        else
        {
            int row = proxy->mapToSource(index).row();
            model->setItem(row, (int) Columns::ALIAS, getAliasItem(credentials));
            model->setItem(row, (int) Columns::HOST_PORT, getAddressItem(credentials));
            model->setItem(row, (int) Columns::AUTHENTICATION, getAuthenticationItem(credentials));
            model->setItem(row, (int) Columns::SSL, getSSLItem(credentials));
            model->setItem(row, (int) Columns::SSH, getSSHItem(credentials));
        }
        sync();
    }
}

void repo::gui::RepoDialogManagerConnect::sync()
{
    // TODO: put into async worker
    QList<repo::RepoCredentials> list;
    for (int i = 0; i < model->invisibleRootItem()->rowCount(); ++i)
    {
        QModelIndex index = model->indexFromItem(model->invisibleRootItem()->child(i, (int) Columns::ALIAS));
        list.append(index.data(Qt::UserRole + 1).value<repo::RepoCredentials>());
    }
    settings::RepoSettingsCredentials credentialsSettings;
    credentialsSettings.writeCredentials(list);
}

QStandardItem *repo::gui::RepoDialogManagerConnect::getAliasItem(
        const RepoCredentials &credentials)
{
    QVariant var;
    var.setValue(credentials);
    QStandardItem *item = createItem(QString::fromStdString(credentials.getAlias()));
    item->setData(var);
    item->setEnabled(true);
    return item;
}

QStandardItem *repo::gui::RepoDialogManagerConnect::getAddressItem(
        const RepoCredentials &credentials)
{
    return createItem(QString::fromStdString(credentials.getHostAndPort()));
}

QStandardItem *repo::gui::RepoDialogManagerConnect::getAuthenticationItem(
        const RepoCredentials &credentials)
{
    QString label;
    QStandardItem *item = 0;
    if (!credentials.getAuthenticationDatabase().empty() &&
        !credentials.getUsername().empty())
    {
        label = QString::fromStdString(
                credentials.getAuthenticationDatabase() +
                " / " +
                credentials.getUsername());
        item = createItem(label);
        item->setIcon(RepoFontAwesome::getAuthenticationIcon());
    }
    else
        item = createItem(label);
    return item;
}

QStandardItem *repo::gui::RepoDialogManagerConnect::getSSLItem(
        const RepoCredentials &credentials)
{
    QStandardItem *item = createItem(QVariant());
    item->setEnabled(false);
    item->setIcon(RepoFontAwesome::getInstance().getIcon(
                      RepoFontAwesome::fa_unlock_alt));
    return item;
}

QStandardItem *repo::gui::RepoDialogManagerConnect::getSSHItem(
        const RepoCredentials &credentials)
{
    QStandardItem *item = createItem(QVariant());
    item->setEnabled(false);
    item->setIcon(RepoFontAwesome::getInstance().getIcon(
                      RepoFontAwesome::fa_unlock_alt));
    return item;
}
