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

//------------------------------------------------------------------------------
// Qt
#include <QDialog>
#include <QString>
#include <QHash>
#include <QComboBox>
#include <QItemDelegate>
#include <QTreeWidgetItem>
#include <QStringList>

//------------------------------------------------------------------------------
// GUI
#include "../../primitives/repocomboboxdelegate.h"

//------------------------------------------------------------------------------
// CORE
#include <repo/core/model/bson/repo_bson_role.h>
#include <repo/core/model/bson/repo_bson_factory.h>


namespace Ui {
class RepoDialogRole;
}

namespace repo {
namespace widgets {

class RepoDialogRole : public QDialog
{
    Q_OBJECT

    //! Read, Write and ReadWrite strings.
    static const repo::gui::RepoComboBoxEditor::SeparatedEntries rwSeparatedEntries;

public:

    explicit RepoDialogRole(
            const repo::core::model::RepoRole &role,
            const QString &currentDatabase,
            const std::map<std::string, std::list<std::string> > &databasesWithProjects,
            QWidget *parent = 0);

    ~RepoDialogRole();

public slots:

    //! Adds an empty item.
    QTreeWidgetItem *addItem();

    //! Adds an item based on given project and readWrite permission.
    QTreeWidgetItem *addItem(
            const std::string &project,
            const repo::core::model::AccessRight &rw);

    //! Removes currently selected Access Rights item.
    void removeItem();

    //! Returns role name.
    std::string getName() const;

    //! Returns currently selected database name.
    std::string getDatabase() const;

    //! Returns a vector of permissions
    std::vector<repo::core::model::RepoPermission> getPermissions() const;

    //! Returns true if the role name is different from original, false otherwise.
    bool isNewRole() const;

    //! Sets current delegate based on given database.
    void setDelegate(const QString &database);


public :

    //! Returns updated role.
    repo::core::model::RepoRole getUpdatedRole();

private :

    //! Converts access right to string.
    static QString accessRightToString(const repo::core::model::AccessRight &rw);

    //! Converts string to access right.
    static repo::core::model::AccessRight stringToAccessRight(const QString &str);

private:

    //! Ui var
    Ui::RepoDialogRole *ui;

    //! Role to edit
    repo::core::model::RepoRole role;

    //! RW delegate
    repo::gui::RepoComboBoxDelegate *rwDelegate;

    //! A map of databases to associated projects.
    std::map<std::string, std::list<std::string> > databasesWithProjects;

};

} // end widgets
} // end repo


