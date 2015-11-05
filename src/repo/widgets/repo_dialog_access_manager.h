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

#include "../../primitives/repo_fontawesome.h"
#include "../../primitives/repoidbcache.h"

namespace Ui {
class RepoDialogAccessManager;
}

namespace repo {
namespace widgets {

class RepoDialogAccessManager : public QDialog
{
    Q_OBJECT

public:

    explicit RepoDialogAccessManager(
            const repo::gui::RepoIDBCache *dbCache,
            repo::RepoController *controller,
            QWidget *parent = 0);

    ~RepoDialogAccessManager();

public slots :

    virtual int exec();

    void refresh();

public :

    repo::RepoToken* getToken() const;

    std::string getDatabase() const;

private:

    Ui::RepoDialogAccessManager *ui;

    const repo::gui::RepoIDBCache *dbCache;

    repo::RepoController *controller;

};

} // widgets
} // repo
