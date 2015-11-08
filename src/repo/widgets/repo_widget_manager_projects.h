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

namespace repo {
namespace widgets {

class RepoWidgetManagerProjects : public RepoWidgetTreeEditable
{
    Q_OBJECT

    static const QString COLUMNS_SETTINGS;

    enum class Columns { PROJECT, DESCRIPTION, OWNER, PERMISSIONS, TYPE, USERS };

public:

    explicit RepoWidgetManagerProjects(QWidget *parent = 0);

    ~RepoWidgetManagerProjects();

public slots:

    //! Updates selected item.
    virtual void edit() {}

    //! Updates item based on model index.
    virtual void edit(const QModelIndex &index) {}

    //! Refreshes the current list
    virtual void refresh() {}

    //! Removes item and refreshes the DB if necessary.
    virtual void removeItem() {}

    //! Shows edit dialog.
    virtual void showEditDialog() {}

};

} // widgets
} // repo


