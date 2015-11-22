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
// GUI
#include "repo_widget_tree_editable.h"
#include "repo_widget_tree_clickable.h"
#include "../workers/repo_worker_roles.h"

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

    enum class Columns { ROLE, COLLECTION, PRIVILEGES, PROJECT_ACCESS_RIGHTS, INHERITED_ROLES };

public:

    explicit RepoWidgetManagerRoles(QWidget *parent = 0);

    ~RepoWidgetManagerRoles();

public slots:

    void addRole(const repo::core::model::RepoRole &role);

    //! Updates selected item.
    virtual void edit() {}

    //! Updates item based on model index.
    virtual void edit(const QModelIndex &index) {}

    //! Refreshes the current list
    virtual void refresh();

    //! Removes item and refreshes the DB if necessary.
    virtual void removeItem() {}

    //! Shows edit dialog.
    virtual void showEditDialog() {}

public :

    void setDBConnection(
            repo::RepoController *controller,
            const repo::RepoToken* token,
            const std::string& database);

private :

    const repo::RepoToken* token;

    std::string database;

    repo::RepoController *controller;

};

} // widgets
} // repo

