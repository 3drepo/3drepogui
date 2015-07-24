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


#ifndef REPO_PROJECT_MANAGER_DIALOG_H
#define REPO_PROJECT_MANAGER_DIALOG_H

#include <QObject>
#include <QMessageBox>

#include "repoabstractmanagerdialog.h"
//#include "../workers/repoworkerprojectsettings.h"
#include "../primitives/repoidbcache.h"

//Q_DECLARE_METATYPE(repo::core::RepoProjectSettings)

namespace repo {
namespace gui {

class RepoProjectManagerDialog : public RepoAbstractManagerDialog
{    
    Q_OBJECT

    enum Columns { PROJECT, DESCRIPTION, OWNER, GROUP, PERMISSIONS, TYPE, USERS };

public:

    RepoProjectManagerDialog(
            const RepoIDBCache *cache,
            QWidget *parent = 0);

    ~RepoProjectManagerDialog();

public slots :

    //void addProjectSettings(/*core::RepoProjectSettings*/);

    void clear(bool resizeColumns = false);

    void edit() { showEditDialog(/*getProjectSettings()*/); }

    void edit(const QModelIndex &index) { showEditDialog(/*getProjectSettings(index)*/); }

    /*core::RepoProjectSettings getProjectSettings();

    core::RepoProjectSettings getProjectSettings(const QModelIndex &index);*/

    void refresh(/*const core::RepoBSON &command = core::RepoBSON()*/);

    //! Removes currently selected item if any.
    void removeItem();

    void showEditDialog() {  }

    //void showEditDialog(/*const repo::core::RepoProjectSettings &*/);
};

} // end namespace gui
} // end namespace repo

#endif // REPO_PROJECT_MANAGER_DIALOG_H
