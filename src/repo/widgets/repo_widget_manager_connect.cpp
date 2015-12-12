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


#include "repo_widget_manager_connect.h"

using namespace repo::widgets;

const QString RepoWidgetManagerConnect::COLUMNS_SETTINGS = "RepoWidgetManagerConnectHeaders";
const QString RepoWidgetManagerConnect::SELECTION_SETTINGS = "RepoWidgetManagerSelectionSettings";

RepoWidgetManagerConnect::RepoWidgetManagerConnect(QWidget *parent)
    : RepoWidgetTreeEditable(parent)
    , controller(nullptr)
{
    QList<QString> headers = {
        tr("Alias"),
        tr("Address"),
        tr("Authentication"),
        tr("SSL"),
        tr("SSH")};

    RepoWidgetTreeFilterable *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

RepoWidgetManagerConnect::~RepoWidgetManagerConnect()
{
    getFilterableTree()->storeHeaders(COLUMNS_SETTINGS);
    getFilterableTree()->storeSelection(SELECTION_SETTINGS);
}

void RepoWidgetManagerConnect::addItem(const repo::RepoCredentials &credentials)
{
    QList<QStandardItem *> row;
    row.append(makeAliasItem(credentials));
    row.append(makeAddressItem(credentials));
    row.append(makeAuthenticationItem(credentials));

    // TODO: add support for SSL and SSH
    row.append(makeSSLItem(credentials));
    row.append(makeSSHItem(credentials));

    getFilterableTree()->addTopLevelRow(row);
}

void RepoWidgetManagerConnect::edit()
{
    edit(getFilterableTree()->getCurrentIndex());
}

void RepoWidgetManagerConnect::edit(const QModelIndex &index)
{
    showEditDialog(getConnection(index), index, Action::EDIT);
}

repo::RepoCredentials RepoWidgetManagerConnect::getConnection()
{
    return getConnection(getFilterableTree()->getCurrentIndex());
}

repo::RepoCredentials RepoWidgetManagerConnect::getConnection(const QModelIndex &index)
{
    repo::RepoCredentials credentials;
    if (index.isValid())
    {
        QModelIndex credentialsIndex = index.sibling(index.row(), (int) Columns::ALIAS);
        credentials = credentialsIndex.data(Qt::UserRole + 1).value<repo::RepoCredentials>();
    }
    return credentials;
}

void RepoWidgetManagerConnect::refresh()
{
    //    ui->progressBar->show(); // TODO: delete line
    //----------------------------------------------------------------------
    clear(); // Clear any previous entries


    // TODO: new worker here
    repo::settings::RepoSettingsCredentials settings;
    for (repo::RepoCredentials credentials : settings.readCredentials())
    {
        addItem(credentials);
    }

    getFilterableTree()->restoreSelection(SELECTION_SETTINGS);

    //----------------------------------------------------------------------
    //    ui->progressBar->hide(); // TODO: show
}

void RepoWidgetManagerConnect::removeItem()
{
    repo::RepoCredentials credentials = getConnection();
    switch(QMessageBox::warning(this,
                                tr("Remove connection?"),
                                tr("Are you sure you want to remove '") + QString::fromStdString(credentials.getAlias()) + "'?",
                                tr("&Yes"),
                                tr("&No"),
                                QString::null, 1, 1))
    {
    case 0: // yes
        getFilterableTree()->removeRow();
        serialize();
        break;
    case 1: // no
        std::cout << "Remove connection warning box cancelled by user." << std::endl;
        break;
    }
}

void RepoWidgetManagerConnect::showEditDialog(
        const repo::RepoCredentials &credentials,
        const QModelIndex &index,
        const Action action)
{
    RepoDialogConnect connectionSettingsDialog(
                controller,
                credentials,
                action == RepoWidgetTreeEditable::Action::COPY,
                this);
    if (QDialog::Rejected == connectionSettingsDialog.exec())
        repoLog("Connection Settings Dialog cancelled by user.\n");
    else // QDialog::Accepted
    {
        repoLog("Create or update connection settings...\n");

        RepoCredentials credentials = connectionSettingsDialog.getConnectionSettings();
        if (!index.isValid())
        {
            addItem(credentials);
        }
        else
        {
            int row = getFilterableTree()->getProxyModel()->mapToSource(index).row();
            QStandardItemModel *model = getFilterableTree()->getModel();
            model->setItem(row, (int) Columns::ALIAS, makeAliasItem(credentials));
            model->setItem(row, (int) Columns::HOST_PORT, makeAddressItem(credentials));
            model->setItem(row, (int) Columns::AUTHENTICATION, makeAuthenticationItem(credentials));
            model->setItem(row, (int) Columns::SSL, makeSSLItem(credentials));
            model->setItem(row, (int) Columns::SSH, makeSSHItem(credentials));
        }
        serialize();
    }
}

void RepoWidgetManagerConnect::serialize()
{
    // TODO: put into async worker
    QList<repo::RepoCredentials> list;
    QStandardItemModel *model = getFilterableTree()->getModel();
    for (int i = 0; i < model->invisibleRootItem()->rowCount(); ++i)
    {
        QModelIndex index = model->indexFromItem(
                    model->invisibleRootItem()->child(i, (int) Columns::ALIAS));
        list.append(index.data(Qt::UserRole + 1).value<repo::RepoCredentials>());
    }
    settings::RepoSettingsCredentials credentialsSettings;
    credentialsSettings.writeCredentials(list);
}


QStandardItem *RepoWidgetManagerConnect::makeAliasItem(
        const repo::RepoCredentials &credentials)
{
    QVariant var;
    var.setValue(credentials);
    QStandardItem *item = new repo::primitives::RepoStandardItem(credentials.getAlias());
    item->setData(var);
    item->setEnabled(true);
    item->setIcon(repo::gui::RepoFontAwesome::getHostIcon());
    return item;
}

QStandardItem *RepoWidgetManagerConnect::makeAddressItem(
        const repo::RepoCredentials &credentials)
{
    return new repo::primitives::RepoStandardItem(credentials.getHostAndPort());
}

QStandardItem *RepoWidgetManagerConnect::makeAuthenticationItem(
        const repo::RepoCredentials &credentials)
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
        item = new repo::primitives::RepoStandardItem(label);
        item->setIcon(repo::gui::RepoFontAwesome::getDatabaseIcon());
    }
    else
        item = new repo::primitives::RepoStandardItem(label);
    return item;
}

QStandardItem *RepoWidgetManagerConnect::makeSSLItem(
        const repo::RepoCredentials &)
{
    QStandardItem *item = new repo::primitives::RepoStandardItem();
    item->setEnabled(true);
    item->setIcon(repo::gui::RepoFontAwesome::getInstance().getIcon(
                      repo::gui::RepoFontAwesome::fa_unlock_alt));
    return item;
}

QStandardItem *RepoWidgetManagerConnect::makeSSHItem(
        const repo::RepoCredentials &)
{
    QStandardItem *item = new repo::primitives::RepoStandardItem();
    item->setEnabled(true);
    item->setIcon(repo::gui::RepoFontAwesome::getInstance().getIcon(
                      repo::gui::RepoFontAwesome::fa_unlock_alt));
    return item;
}



