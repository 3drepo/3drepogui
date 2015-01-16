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
#include <QComboBox>
#include <QItemDelegate>
#include <QItemEditorFactory>
#include <QStandardItemEditorCreator>
#include <QTreeWidgetItem>


//------------------------------------------------------------------------------
// Core
#include <RepoUser>

//------------------------------------------------------------------------------
// GUI
#include "../primitives/repocomboboxeditor.h"
#include "../primitives/repocomboboxdelegate.h"

namespace Ui {
    class RepoDialogUser;
}

namespace repo {
namespace gui {

class RepoDialogUser : public QDialog
{
    Q_OBJECT

    enum RepoProjectsColumns { OWNER, PROJECT };
    enum RolesColumns { DATABASE, ROLE };

    enum Tabs { PROJECTS, GROUPS, ROLES };

public:
    explicit RepoDialogUser(
            core::RepoUser user,
            const std::list<std::string> &databaseList,
            const std::list<std::string> &customRolesList,
            QWidget *parent = 0);

    ~RepoDialogUser();

    //! Adds Access Rights item depending on the selected tab.
    void addItem();

    //! Adds a DB Role pair to the Roles table.
    QTreeWidgetItem *addRole(const std::pair<std::string, std::string> &role);

    static QIcon getIcon();

    static void populateModel(
        QStandardItemModel *model,
        const std::vector<std::pair<std::string, std::string> > &data);

    //! Removes currently selected Access Rights item depending on the selected tab.
    void removeItem();

public slots :

    void rolesItemChanged(QTreeWidgetItem * current, int column);

private:

    //! User to be created or modified.
    core::RepoUser user;

    //! Model of the projects table.
    QStandardItemModel *projectsModel;

    //! Ui var.
    Ui::RepoDialogUser *ui;

    RepoComboBoxDelegate *databasesDelegate;

    RepoComboBoxDelegate *anyDBRolesDelegate;

    RepoComboBoxDelegate *adminDBRolesDelegate;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_H
