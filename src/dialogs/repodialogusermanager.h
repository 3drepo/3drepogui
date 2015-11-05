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
#include <QMutex>


//------------------------------------------------------------------------------
// GUI
#include "repodialoguser.h"
#include "repoabstractmanagerdialog.h"
#include "../primitives/repoidbcache.h"
#include "../repo/workers/repo_worker_users.h"

//------------------------------------------------------------------------------
// CORE
#include <repo/core/model/bson/repo_bson_user.h>

Q_DECLARE_METATYPE(repo::core::model::RepoUser)

namespace repo {
namespace gui {

class RepoDialogUserManager : public RepoAbstractManagerDialog
{
    Q_OBJECT

	enum class Columns { ACTIVE, USERNAME, FIRST_NAME, LAST_NAME, EMAIL, PROJECTS, GROUPS, ROLES };

public:

	explicit RepoDialogUserManager(
			repo::RepoController *controller,
            const RepoIDBCache *dbCache,
            QWidget *parent = 0);

    ~RepoDialogUserManager();

public slots:

    //! Adds a fresh list of custom roles.
    void addCustomRoles(const std::list<std::string> &);

    //! Adds a fresh mapping of databases with their associated projects.
    void addDatabasesWithProjects(const std::map<std::string, std::list<std::string> > &);

    //! Adds user to the list of users.
    void addUser(const repo::core::model::RepoUser &user);

    //! Updates selected user.
    void edit();

    //! Updates user based on model index.
    void edit(const QModelIndex &index);

    //! Returns a currently selected user if any.
	repo::core::model::RepoUser getUser();

    //! Returns a user specified by the model index.
	repo::core::model::RepoUser getUser(const QModelIndex &index);

    //! Refreshes the current list of users by fetching from a database.
	void refresh()
	{
		refresh(repo::core::model::RepoUser(), repo::worker::UsersWorker::Command::INSERT);
	}

	void refresh(
		const repo::core::model::RepoUser &user,
		const repo::worker::UsersWorker::Command &command);

    //! Drops user from the database.
    void removeItem();

    //! Shows the user dialog and saves edits to the database.
	void showEditDialog() { showEditDialog(repo::core::model::RepoUser()); }

    //! Shows the user dialog and saves edits to the database.
	void showEditDialog(const repo::core::model::RepoUser &user);

    //! Unlocks refresh mutex.
    void unlockMutex() { mutex.unlock(); }

private:

    //! List of custom roles updated upon each refresh.
    std::list<std::string> customRolesList;

    //! Mapping of databases to their associated projects.
    std::map<std::string, std::list<std::string> > databasesWithProjects;

    //! DB connection controller
	repo::RepoController *controller;

    //! Refresh mutex.
    QMutex mutex;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_MANAGER_H
