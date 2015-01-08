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


#ifndef REPO_DIALOG_USER_H
#define REPO_DIALOG_USER_H

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QStandardItemModel>

//------------------------------------------------------------------------------
// Core
#include <RepoUser>

namespace Ui {
    class RepoDialogUser;
}

namespace repo {
namespace gui {

class RepoDialogUser : public QDialog
{
    Q_OBJECT

    enum RepoProjectsColumns { OWNER, PROJECT };

public:
    explicit RepoDialogUser(core::RepoUser user, QWidget *parent = 0);

    ~RepoDialogUser();

    static QIcon getIcon();

    static void populateModel(
        QStandardItemModel *model,
        const std::vector<std::pair<std::string, std::string> > &data);

private:

    core::RepoUser user;

    //! Model of the projects table.
    QStandardItemModel *projectsModel;

    //! Model of the roles table.
    QStandardItemModel *rolesModel;

    //! Ui var.
    Ui::RepoDialogUser *ui;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_H
