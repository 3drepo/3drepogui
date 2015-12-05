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
#include <QColorDialog>
#include <QPixmap>

//------------------------------------------------------------------------------
// GUI
#include "../../primitives/repocomboboxdelegate.h"
#include "../workers/repo_worker_projects.h"
#include "../../primitives/repo_color.h"

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
            const repo::core::model::RepoRoleSettings &settings,
            const QString &currentDatabase,
            const std::map<std::string, std::list<std::string> > &databasesWithProjects,
            const bool isCopy,
            QWidget *parent = 0);

    ~RepoDialogRole();

public slots:

    //! Adds an empty item.
    QTreeWidgetItem *addPermissionItem();

    QTreeWidgetItem *addModuleItem();

    QTreeWidgetItem *addPrivilegeItem(
            const std::string &database,
            const std::string &collection,
            const std::vector<std::string> &actions);

    QTreeWidgetItem *addModuleItem(const std::string &module);

    void removeModuleItem();

    std::string getColor() const;

    std::string getDescription() const;

    std::vector<std::string> getModules() const;

    //! Returns role name.
    std::string getName() const;

    //! Returns currently selected database name.
    std::string getDatabase() const;

    //! Returns a vector of permissions
    std::vector<repo::core::model::RepoPermission> getPermissions() const;

    //! Returns true if the role or DB are different from originals, false otherwise.
    bool isNewRole() const;

    //! Shows color dialog and sets selected color.
    void showColorDialog();

    void setColor(const QString &hex);

    //! Sets current delegate based on given database.
    void setDelegate(const QString &database);


public :

    //! Returns updated role.
    repo::core::model::RepoRole getUpdatedRole() const;

    repo::core::model::RepoRoleSettings getUpdatedRoleSettings() const;

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

    //! Role settings to edit
    repo::core::model::RepoRoleSettings settings;

    //! RW delegate
    repo::gui::RepoComboBoxDelegate *rwDelegate;

    //! A map of databases to associated projects.
    std::map<std::string, std::list<std::string> > databasesWithProjects;

};

} // end widgets
} // end repo


