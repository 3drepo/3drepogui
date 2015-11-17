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

#ifndef REPO_DIALOG_MANAGER_CONNECT_H
#define REPO_DIALOG_MANAGER_CONNECT_H

//------------------------------------------------------------------------------
// CORE
#include <repo/repo_controller.h>
#include <repo/repo_credentials.h>

//------------------------------------------------------------------------------
// GUI
#include "repoabstractmanagerdialog.h"
#include "repo_dialogconnect.h"
#include "../primitives/repoidbcache.h"
#include "../repo/settings/repo_settings_credentials.h"

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QThreadPool>
#include <QItemSelection>
#include <QMessageBox>
#include <QSettings>
#include <QTreeView>
#include <QItemSelectionModel>

Q_DECLARE_METATYPE(repo::RepoCredentials)

namespace repo {
namespace gui {

/**
 * @brief The RepoDialogManagerConnect class
 * Provides a Connection Manager Dialog to set multiple connections and let Qt
 * remember the settings for next user session. Each user settings is stored
 * as a streamable RepoCredentials object in platform independent settings.
 */
class RepoDialogManagerConnect : public RepoAbstractManagerDialog
{
    Q_OBJECT

    //! Tree view columns
    enum class Columns { ALIAS, HOST_PORT, AUTHENTICATION, SSL, SSH };

    static const QString SETTINGS_CONNECTIONS_COLUMNS;
    static const QString SETTINGS_SELECTION;

public:

    //! Explicit constructor
    /**
     * @brief RepoDialogManagerConnect
     * @param parent parent widget which can be null
     */
    explicit RepoDialogManagerConnect(repo::RepoController *controller, QWidget *parent = 0);

    ~RepoDialogManagerConnect();

public :

    //! Adds connection token to the list of available connections.
    /**
     * @brief addConnectionToken
     * @param connectionToken connection token for specific DB connection
     */
    void addCredentials(const repo::RepoCredentials &credentials);

    //! Updates selected item.
    void edit();

    //! Updates item based on model index.
    void edit(const QModelIndex &index);

    //! Returns currently selected connection.
    RepoCredentials getConnection();

    //! Returns connection based on given model index.
    repo::RepoCredentials getConnection(const QModelIndex &index);

    //! Refreshes the current list
    void refresh();

    //! Removes item and refreshes if necessary.
    void removeItem();

    //! Shows edit dialog.
    void showEditDialog()
    { showEditDialog(repo::RepoCredentials(), QModelIndex()); }

    //! Shows the edit dialog and saves edits to token.
    void showEditDialog(const repo::RepoCredentials &credentials,
                        const QModelIndex &index);

private:

    //! Synchronizes current connections settings to disk.
    void sync();

    //! Returns alias tree item from given credentials
    static QStandardItem *getAliasItem(const RepoCredentials &credentials);

    //! Returns address tree item from given credentials
    static QStandardItem *getAddressItem(const RepoCredentials &credentials);

    //! Returns authentication tree item from given credentials
    static QStandardItem *getAuthenticationItem(const RepoCredentials &credentials);

    //! Returns ssl tree item from given credentials
    static QStandardItem *getSSLItem(const RepoCredentials &credentials);

    //! Returns ssh tree item from given credentials
    static QStandardItem *getSSHItem(const RepoCredentials &credentials);

private :

    repo::RepoController *controller;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_MANAGER_CONNECT_H
