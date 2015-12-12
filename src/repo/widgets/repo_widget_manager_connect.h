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

#pragma once

#include <QWidget>
#include <QDialog>
#include <QMessageBox>

//------------------------------------------------------------------------------
// GUI
#include "repo_widget_tree_editable.h"
#include "repo_widget_tree_clickable.h"
#include "repo_dialog_connect.h"
#include "../../primitives/repoidbcache.h"
#include "../primitives/repo_standard_item.h"
#include "../settings/repo_settings_credentials.h"

//------------------------------------------------------------------------------
// CORE
#include <repo/repo_controller.h>
#include <repo/repo_credentials.h>

//------------------------------------------------------------------------------

namespace repo {
namespace widgets {

Q_DECLARE_METATYPE(repo::RepoCredentials)

class RepoWidgetManagerConnect: public RepoWidgetTreeEditable
{
    Q_OBJECT

    static const QString COLUMNS_SETTINGS;

    static const QString SELECTION_SETTINGS;

    //! Tree view columns
    enum class Columns { ALIAS, HOST_PORT, AUTHENTICATION, SSL, SSH };

public:

    explicit RepoWidgetManagerConnect(QWidget *parent);

    ~RepoWidgetManagerConnect();

public slots :

    //! Adds credentials as an item into the table.
    void addItem(const repo::RepoCredentials &credentials);

    //! Updates selected item.
    virtual void edit();

    //! Updates item based on model index.
    virtual void edit(const QModelIndex &index);

    //! Returns currectly selected connection.
    repo::RepoCredentials getConnection();

    //! Returns connection for given model index.
    repo::RepoCredentials getConnection(const QModelIndex &index);

    //! Refreshes the current list
    virtual void refresh();

    //! Copies currectly selected item.
    virtual void copyItem()
    { showEditDialog(getConnection(), QModelIndex(), Action::COPY); }

    //! Removes item and refreshes the DB if necessary.
    virtual void removeItem();

    //! Shows edit dialog.
    virtual void showEditDialog()
    {
        showEditDialog(repo::RepoCredentials(), QModelIndex(), Action::ADD);
    }

    //! Shows edit dialog populated with given credentials.
    void showEditDialog(
            const repo::RepoCredentials &credentials,
            const QModelIndex &index,
            const Action action);

private :

    //! Serializes table contents onto the HDD.
    void serialize();

    //! Returns alias tree item from given credentials
    static QStandardItem *makeAliasItem(const repo::RepoCredentials &credentials);

    //! Returns address tree item from given credentials
    static QStandardItem *makeAddressItem(const repo::RepoCredentials &credentials);

    //! Returns authentication tree item from given credentials
    static QStandardItem *makeAuthenticationItem(const repo::RepoCredentials &credentials);

    //! Returns ssl tree item from given credentials
    static QStandardItem *makeSSLItem(const repo::RepoCredentials &);

    //! Returns ssh tree item from given credentials
    static QStandardItem *makeSSHItem(const repo::RepoCredentials &);

private :

    //! DB controller.
    repo::RepoController *controller;
};

} // widgets
} // repo


