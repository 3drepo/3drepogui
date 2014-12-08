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
//------------------------------------------------------------------------------
// Core
#include <RepoWrapperMongo>
//------------------------------------------------------------------------------
#include "../workers/repo_workerusers.h"
namespace Ui {
class RepoDialogUserManager;
}

namespace repo {
namespace gui {


class RepoDialogUserManager : public QDialog
{
    Q_OBJECT

    enum RepoUsersColumns { USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL };

public:

    explicit RepoDialogUserManager(
            const core::MongoClientWrapper &mongo,
            QWidget *parent = 0);
    ~RepoDialogUserManager();

    //! Clears the users model.
    void clearUsersModel();

    //! Returns icon associated with this dialog.
    static QIcon getIcon();

signals :

    //! Emitted whenever running threads are to be cancelled.
    void cancel();

public slots:

    //! Adds user to the list of users
    void addUser(
            QVariant username,
            QVariant password,
            QVariant firstName,
            QVariant lastName,
            QVariant email);

    //! Cancels all running threads and waits for their completion.
    bool cancelAllThreads();

    //! Forces refresh.
    int exec();

    //! Refreshes the current list of users by fetching from a database.
    void refresh();

    //! Sets the number of users shown in the "Showing x of y" label.
    void updateUsersCount() const;

private :

    QStandardItem *createItem(QVariant& data);


private:

    //! Model of the users table.
    QStandardItemModel *usersModel;

    //! Proxy model for users table to enable filtering.
    QSortFilterProxyModel *usersProxy;

    //! Threadpool for this object only.
    QThreadPool threadPool;

    //! Mongo connector.
    core::MongoClientWrapper mongo;

    //! Ui var.
    Ui::RepoDialogUserManager *ui;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_MANAGER_H
