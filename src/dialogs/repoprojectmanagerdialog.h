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

#include <QObject>
#include <QMessageBox>

#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_bson_project_settings.h>

#include "repoabstractmanagerdialog.h"
#include "../primitives/repoidbcache.h"



Q_DECLARE_METATYPE(repo::core::model::RepoProjectSettings)

namespace repo {
namespace gui {

class RepoProjectManagerDialog : public RepoAbstractManagerDialog
{    
    Q_OBJECT

    enum class Columns { PROJECT, DESCRIPTION, OWNER, GROUP, PERMISSIONS, TYPE, USERS };

public:

    RepoProjectManagerDialog(
			repo::RepoController *controller,
            const RepoIDBCache *cache,
            QWidget *parent = 0);

    ~RepoProjectManagerDialog();

public slots :

	void addProjectSettings(repo::core::model::RepoProjectSettings);

    void clear(bool resizeColumns = false);

    void edit() { showEditDialog(getProjectSettings()); }

    void edit(const QModelIndex &index) { showEditDialog(getProjectSettings(index)); }

	repo::core::model::RepoProjectSettings getProjectSettings();

	repo::core::model::RepoProjectSettings getProjectSettings(const QModelIndex &index);

	//! Refreshes the current list of users by fetching from a database.
	void refresh()
	{
		refresh(repo::core::model::RepoProjectSettings(), false);
	}

	void repo::gui::RepoProjectManagerDialog::refresh(
		const repo::core::model::RepoProjectSettings &settings,
		const bool                                         &isDelete);

    //! Removes currently selected item if any.
    void removeItem();

	void showEditDialog() { showEditDialog(repo::core::model::RepoProjectSettings()); }

	void showEditDialog(const repo::core::model::RepoProjectSettings &);

private:
	repo::RepoController *controller;
};

} // end namespace gui
} // end namespace repo

