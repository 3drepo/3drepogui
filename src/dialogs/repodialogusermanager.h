/**
 *  Copyright (C) 2014 3D Repo Ltd
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

#ifndef REPO_DIALOG_USER_MANAGER_H
#define REPO_DIALOG_USER_MANAGER_H

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QThreadPool>
#include <QItemSelection>
#include <QMessageBox>

//------------------------------------------------------------------------------
// Core
#include <RepoWrapperMongo>
#include <RepoUser>

//------------------------------------------------------------------------------
// GUI
#include "../workers/repo_workerusers.h"
#include "repodialoguser.h"
#include "repoabstractmanagerdialog.h"

Q_DECLARE_METATYPE(repo::core::RepoUser)

namespace repo {
namespace gui {

class RepoDialogUserManager : public RepoAbstractManagerDialog
{
    Q_OBJECT

    enum Columns { ACTIVE, USERNAME, FIRST_NAME, LAST_NAME, EMAIL, PROJECTS, GROUPS, ROLES };

public:

    explicit RepoDialogUserManager(
            const core::MongoClientWrapper &mongo,
            const std::string &database = core::MongoClientWrapper::ADMIN_DATABASE,
            QWidget *parent = 0);

    ~RepoDialogUserManager();

    //! Clears the users model.
    void clearUsersModel();

    //! Returns icon associated with this dialog.
    static QIcon getIcon();

public slots:

    //! Adds a fresh list of custom roles.
    void addCustomRoles(const std::list<std::string> &);

    //! Adds a fresh mapping of databases with their associated projects.
    void addDatabasesWithProjects(const std::map<std::string, std::list<std::string> > &);

    //! Adds user to the list of users.
    void addUser(const core::RepoUser &user);

    //! Updates selected user.
    void edit();

    //! Updates user based on model index.
    void edit(const QModelIndex &index);

    //! Returns a currently selected user if any.
    core::RepoUser getUser();

    //! Returns a user specified by the model index.
    core::RepoUser getUser(const QModelIndex &index);

    //! Refreshes the current list of users by fetching from a database.
    void refresh(const core::RepoBSON &command = core::RepoBSON());

    //! Drops user from the database.
    void removeItem();

    //! Shows the user dialog and saves edits to the database.

    void showEditDialog() {  showEditDialog(core::RepoUser()); }
    void showEditDialog(const core::RepoUser &user);

private:

    //! List of custom roles updated upon each refresh.
    std::list<std::string> customRolesList;

    //! Mapping of databases to their associated projects.
    std::map<std::string, std::list<std::string> > databasesWithProjects;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_MANAGER_H
