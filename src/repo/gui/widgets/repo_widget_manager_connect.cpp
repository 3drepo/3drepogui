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

using namespace repo::gui::widget;
using namespace repo::gui;

const QString ConnectionManagerWidget::COLUMNS_SETTINGS = "ConnectionManagerWidgetHeaders";
const QString ConnectionManagerWidget::SELECTION_SETTINGS = "RepoWidgetManagerSelectionSettings";

ConnectionManagerWidget::ConnectionManagerWidget(QWidget *parent)
    : EditableTreeWidget(parent)
    , controller(nullptr)
{
    QList<QString> headers = {
        tr("Alias"),
        tr("Address"),
        tr("Authentication"),
        tr("SSL"),
        tr("SSH")};

    FilterableTreeWidget *filterableTree = getFilterableTree();
    filterableTree->restoreHeaders(headers, COLUMNS_SETTINGS);
    filterableTree->setRootIsDecorated(false);

    clear();
}

ConnectionManagerWidget::~ConnectionManagerWidget()
{
    getFilterableTree()->storeHeaders(COLUMNS_SETTINGS);
    getFilterableTree()->storeSelection(SELECTION_SETTINGS);
}

void ConnectionManagerWidget::addItem(const std::vector<char> &credentials)
{
    repo::RepoController::RepoToken* token=nullptr;
    if(credentials.size())
    {
        token = controller->createTokenFromSerialised((credentials));
    }

    if(token)
    {
        std::string aliasStr, host, username, authDB;
        uint32_t port;
        controller->getInfoFromToken(token, aliasStr, host, port, username, authDB);
        QList<QStandardItem *> row;
        row.append(makeAliasItem(credentials, aliasStr));
        row.append(makeAddressItem(credentials, host + std::to_string(port)));
        row.append(makeAuthenticationItem(credentials, authDB, username));

        // TODO: add support for SSL and SSH
        row.append(makeSSLItem(credentials));
        row.append(makeSSHItem(credentials));

        getFilterableTree()->addTopLevelRow(row);
    }
    else
    {
        repoLogError("Failed to add item");
    }


}

void ConnectionManagerWidget::edit()
{
    edit(getFilterableTree()->getCurrentIndex());
}

void ConnectionManagerWidget::edit(const QModelIndex &index)
{
    showEditDialog(getConnection(index), index, Action::EDIT);
}

std::vector<char> ConnectionManagerWidget::getConnection()
{
    return getConnection(getFilterableTree()->getCurrentIndex());
}

std::vector<char> ConnectionManagerWidget::getConnection(const QModelIndex &index)
{
    std::vector<char> serialisedToken;
    if (index.isValid())
    {
        QModelIndex credentialsIndex = index.sibling(index.row(), (int) Columns::ALIAS);
        serialisedToken = credentialsIndex.data(Qt::UserRole + 1).value<std::vector<char>>();
    }
    return serialisedToken;
}

void ConnectionManagerWidget::refresh()
{
    //    ui->progressBar->show(); // TODO: delete line
    //----------------------------------------------------------------------
    clear(); // Clear any previous entries


    // TODO: new worker here
    repo::settings::RepoSettingsCredentials settings;
    for (std::vector<char> credentials : settings.readCredentials())
    {
        addItem(credentials);
    }

    getFilterableTree()->restoreSelection(SELECTION_SETTINGS);

    //----------------------------------------------------------------------
    //    ui->progressBar->hide(); // TODO: show
}

void ConnectionManagerWidget::removeItem()
{
    switch(QMessageBox::warning(this,
                                tr("Remove connection?"),
                                tr("Are you sure you want to remove this entry?"),
                                tr("&Yes"),
                                tr("&No"),
                                QString::null, 1, 1))
    {
    case 0: // yes
        getFilterableTree()->removeRow();
        serialize();
        break;
    case 1: // no
		repoLog("Remove connection warning box cancelled by user." );
        break;
    }
}

void ConnectionManagerWidget::showEditDialog(
        const std::vector<char> &credentials,
        const QModelIndex &index,
        const Action action)
{
    dialog::ConnectDialog connectionSettingsDialog(
                controller,
                credentials,
                action == EditableTreeWidget::Action::COPY,
                this);
    if (QDialog::Rejected == connectionSettingsDialog.exec())
        repoLog("Connection Settings Dialog cancelled by user.\n");
    else // QDialog::Accepted
    {
        repoLog("Create or update connection settings...\n");

        repo::RepoController::RepoToken* token = connectionSettingsDialog.getConnectionSettings();
        auto credentials = controller->serialiseToken(token);
        if (!index.isValid())
        {
            addItem(credentials);
        }
        else
        {
            std::string aliasStr, host, authDB, username;
            uint32_t port;


            if(token)
            {
                controller->getInfoFromToken(token, aliasStr, host, port, username, authDB);
                int row = getFilterableTree()->getProxyModel()->mapToSource(index).row();
                QStandardItemModel *model = getFilterableTree()->getModel();
                model->setItem(row, (int) Columns::ALIAS, makeAliasItem(credentials, aliasStr));
                model->setItem(row, (int) Columns::HOST_PORT, makeAddressItem(credentials, host + std::to_string(port)));
                model->setItem(row, (int) Columns::AUTHENTICATION, makeAuthenticationItem(credentials, authDB, username));
                model->setItem(row, (int) Columns::SSL, makeSSLItem(credentials));
                model->setItem(row, (int) Columns::SSH, makeSSHItem(credentials));

                delete token;
            }
            else
            {
                repoLogError("Cannot convert serialised data to a RepoToken!");
            }

        }
        serialize();
    }
}

void ConnectionManagerWidget::serialize()
{
    // TODO: put into async worker
    QList<std::vector<char>> list;
    QStandardItemModel *model = getFilterableTree()->getModel();
    for (int i = 0; i < model->invisibleRootItem()->rowCount(); ++i)
    {
        QModelIndex index = model->indexFromItem(
                    model->invisibleRootItem()->child(i, (int) Columns::ALIAS));
        list.append(index.data(Qt::UserRole + 1).value<std::vector<char>>());
    }
    settings::RepoSettingsCredentials credentialsSettings;
    credentialsSettings.writeCredentials(list);
}


QStandardItem *ConnectionManagerWidget::makeAliasItem(
        const std::vector<char> &cred,
        const std::string &alias)
{
    QVariant var;
    var.setValue(cred);
    QStandardItem *item = new repo::gui::primitive::RepoStandardItem(alias);
    item->setData(var);
    item->setEnabled(true);
    item->setIcon(repo::gui::primitive::RepoFontAwesome::getHostIcon());
    return item;
}

QStandardItem *ConnectionManagerWidget::makeAddressItem(
        const std::vector<char> &cred,
        const std::string       &value)
{
    return new repo::gui::primitive::RepoStandardItem(value);
}

QStandardItem *ConnectionManagerWidget::makeAuthenticationItem(
        const std::vector<char> &cred,
        const std::string       &authDB,
        const std::string       &username)
{
    QString label;
    QStandardItem *item = 0;
    if (!authDB.empty() &&
            !username.empty())
    {
        label = QString::fromStdString(
                   authDB + " / " + username);
        item = new repo::gui::primitive::RepoStandardItem(label);
        item->setIcon(repo::gui::primitive::RepoFontAwesome::getDatabaseIcon());
    }
    else
        item = new repo::gui::primitive::RepoStandardItem(label);
    return item;
}

QStandardItem *ConnectionManagerWidget::makeSSLItem(
       const std::vector<char> &)
{
    QStandardItem *item = new repo::gui::primitive::RepoStandardItem();
    item->setEnabled(true);
    item->setIcon(repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                      repo::gui::primitive::RepoFontAwesome::fa_unlock_alt));
    return item;
}

QStandardItem *ConnectionManagerWidget::makeSSHItem(
        const std::vector<char> &)
{
    QStandardItem *item = new repo::gui::primitive::RepoStandardItem();
    item->setEnabled(true);
    item->setIcon(repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                      repo::gui::primitive::RepoFontAwesome::fa_unlock_alt));
    return item;
}



