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

    enum Columns { DATABASE, VALUE };

    //! Access rights tabs.
    enum Tabs { PROJECTS, GROUPS, ROLES };

public:

    //! Explicit constructor.
    explicit RepoDialogUser(core::RepoUser user,
            const std::map<string, std::list<string> > &databasesWithProjects,
            const std::list<std::string> &customRolesList,
            QWidget *parent = 0);

    //! Destructor.
    ~RepoDialogUser();

public slots:

    //! Adds a DB Group pair to the Groups table.
    QTreeWidgetItem *addGroup(const std::pair<std::string, std::string> &);

    //! Adds Access Rights item depending on the selected tab.
    QTreeWidgetItem *addItem();

    //! Adds specific item pair to a given parent tree widget.
    QTreeWidgetItem *addItem(
            const std::pair<std::string, std::string> &pair,
            QTreeWidget *parent,
            const QHash<QString, RepoComboBoxDelegate* > &delegates);

    //! Adds a DB Project pair to the Projects table.
    QTreeWidgetItem *addProject(const std::pair<std::string, std::string> &);

    //! Adds a DB Role pair to the Roles table.
    QTreeWidgetItem *addRole(const std::pair<std::string, std::string> &);

    //! Sets the appropriate delegate if the database column on the project item has changed.
    void updateProjectsDelegate(QTreeWidgetItem * current, int column);

    //! Sets the appropriate delegate if the database column on the role item has changed.
    void updateRolesDelegate(QTreeWidgetItem * current, int column);

public :

    //! Returns the icon for this dialog.
    static QIcon getIcon();

    //! Removes currently selected Access Rights item depending on the selected tab.
    void removeItem();

private:

    //! User to be created or modified.
    core::RepoUser user;

    //! Ui var.
    Ui::RepoDialogUser *ui;

    //! Lookup table for projects delegates by database name.
    QHash<QString, RepoComboBoxDelegate* > groupsDelegates;

    //! Lookup table for projects delegates by database name.
    QHash<QString, RepoComboBoxDelegate* > projectsDelegates;

    //! Lookup table for roles delegates by database name.
    QHash<QString, RepoComboBoxDelegate* > rolesDelegates;
};

} // end namespace gui
} // end namespace repo

#endif // REPO_DIALOG_USER_H
