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
#include "../../dialogs/repoprojectsettingsdialog.h"
#include "../workers/repo_worker_project_settings.h"
#include <repo/core/model/bson/repo_bson_project_settings.h>

Q_DECLARE_METATYPE(repo::core::model::RepoProjectSettings)

namespace repo {
namespace widgets {

class RepoWidgetManagerProjects : public RepoWidgetTreeEditable
{
    Q_OBJECT

    static const QString COLUMNS_SETTINGS;

    enum class Columns { PROJECT, DESCRIPTION, OWNER, TYPE };

public:

    explicit RepoWidgetManagerProjects(QWidget *parent = 0);

    ~RepoWidgetManagerProjects();

public slots:

    void addProjectSettings(repo::core::model::RepoProjectSettings);

    //! Updates selected item.
    virtual void edit() {}

    //! Updates item based on model index.
    void edit(const QModelIndex &index) { showEditDialog(getProjectSettings(index)); }

    repo::core::model::RepoProjectSettings getProjectSettings();

    repo::core::model::RepoProjectSettings getProjectSettings(const QModelIndex &index);


    //! Refreshes the current list of users by fetching from a database.
    void refresh()
    {
        refresh(repo::core::model::RepoProjectSettings(), false);
    }

    void refresh(
        const repo::core::model::RepoProjectSettings &settings,
        bool isDelete);

    //! Removes currently selected item if any.
    void removeItem();

    void showEditDialog() { showEditDialog(repo::core::model::RepoProjectSettings()); }

    void showEditDialog(const repo::core::model::RepoProjectSettings &);

public :

    void setDBConnection(repo::RepoController *controller,
            const repo::RepoToken* token,
            const std::string& database);

private :

    const repo::RepoToken* token;

    std::string database;

    repo::RepoController *controller;

};

} // widgets
} // repo


