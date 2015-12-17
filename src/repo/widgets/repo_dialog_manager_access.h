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

#include <QDialog>
#include <QMutex>
#include <QThreadPool>

#include "../../primitives/repo_fontawesome.h"
#include "../../primitives/repoidbcache.h"
#include "../workers/repo_worker_projects.h"
#include "../workers/repo_multithreader.h"

namespace Ui {
class RepoDialogManagerAccess;
}

namespace repo {
namespace widgets {

class RepoDialogManagerAccess
        : public QDialog
        , public repo::worker::RepoMultithreader
{
    Q_OBJECT

    enum class Tab { USERS, ROLES, PROJECTS };

public:

    explicit RepoDialogManagerAccess(
            const repo::gui::RepoIDBCache *dbCache,
            repo::RepoController *controller,
            QWidget *parent = 0);

    ~RepoDialogManagerAccess();


public slots :

    virtual int exec();

    void refresh();

    void updateUsersTabCount(int count);

    void updateRolesTabCount(int count);

    void updateProjectsTabCount(int count);

    void updateTabCount(Tab tab, const QString &title, int count);

public :

    //! Always returns false in order to capture Tab keypress
    bool focusNextPrevChild(bool) { return false; }

    void keyPressEvent(QKeyEvent*);

    repo::RepoToken* getToken() const;

    std::string getDatabase() const;


private:

    Ui::RepoDialogManagerAccess *ui;

    const repo::gui::RepoIDBCache *dbCache;

    repo::RepoController *controller;



};

} // widgets
} // repo
