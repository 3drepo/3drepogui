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

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QThreadPool>

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

    explicit RepoDialogUserManager(QWidget *parent = 0);
    ~RepoDialogUserManager();

    //! Clears the users model.
    void clearUsersModel();

    //! Returns icon associated with this dialog.
    static QIcon getIcon();

signals :

    //! Emitted whenever running threads are to be cancelled.
    void cancel();

public slots:

    //! Cancels all running threads and waits for their completion.
    bool cancelAllThreads();

    //! Forces refresh.
    int exec();

    //! Refreshes the current list of users by fetching from a database.
    void refresh();

private:

    Ui::RepoDialogUserManager *ui;

    //! Model of the users table.
    QStandardItemModel *usersModel;

    //! Proxy model for users table to enable filtering.
    QSortFilterProxyModel *usersProxy;

    //! Threadpool for this object only.
    QThreadPool threadPool;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_MANAGER_H
