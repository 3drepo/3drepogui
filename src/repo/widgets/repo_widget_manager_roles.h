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

#include <QMessageBox>

//------------------------------------------------------------------------------
// GUI
#include "repo_widget_tree_editable.h"
#include "repo_widget_tree_clickable.h"
#include "../workers/repo_worker_roles.h"
#include "repo_dialog_role.h"

//------------------------------------------------------------------------------
// CORE
#include <repo/core/model/bson/repo_bson_role.h>

Q_DECLARE_METATYPE(repo::core::model::RepoRole)

namespace repo {
namespace widgets {

class RepoWidgetManagerRoles : public RepoWidgetTreeEditable
{
    Q_OBJECT

    static const QString COLUMNS_SETTINGS;

    enum class Columns { ROLE, DATABASE, ACCESS_RIGHTS, PRIVILEGES, INHERITED_ROLES };

public:

    explicit RepoWidgetManagerRoles(QWidget *parent = 0);

    ~RepoWidgetManagerRoles();

public slots:

    //! Adds role to the roles table (tree widget).
    void addRole(const repo::core::model::RepoRole &role);

    //! Updates selected item.
    virtual void edit();

    //! Updates item based on given model index.
    virtual void edit(const QModelIndex &index);

    //! Returns currently selected role.
    repo::core::model::RepoRole getRole() const;

    //! Returns role based on given model index.
    repo::core::model::RepoRole getRole(const QModelIndex &index) const;

    //! Refreshes the current list.
    virtual void refresh()
    { refresh(repo::core::model::RepoRole(),
              repo::worker::RepoWorkerRoles::Command::INSERT); }

    //! Refreshes the current list.
    void refresh(
            const core::model::RepoRole &role,
            repo::worker::RepoWorkerRoles::Command command);

    //! Removes item and refreshes the DB if necessary.
    virtual void removeItem();

    //! Shows edit dialog.
    virtual void showEditDialog() { showEditDialog(repo::core::model::RepoRole()); }

    virtual void showEditDialog(const repo::core::model::RepoRole &role);

    void setDatabasesWithProjects(const std::map<std::string, std::list<std::string> > &rdwp);

public :

    void setDBConnection(repo::RepoController *controller,
            const repo::RepoToken* token,
            const std::string& database);

private :

    const repo::RepoToken* token;

    std::string database;

    repo::RepoController *controller;

    std::map<std::string, std::list<std::string> > databasesWithProjects;

};

} // widgets
} // repo

