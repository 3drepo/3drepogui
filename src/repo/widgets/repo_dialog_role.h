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

//------------------------------------------------------------------------------
// CORE
#include <repo/core/model/bson/repo_bson_role.h>

namespace Ui {
class RepoDialogRole;
}

namespace repo {
namespace widgets {

class RepoDialogRole : public QDialog
{
    Q_OBJECT

public:

    explicit RepoDialogRole(
            const repo::core::model::RepoRole &role,
            QWidget *parent = 0);

    ~RepoDialogRole();

private:

    // Ui var
    Ui::RepoDialogRole *ui;

    // Role to edit
    repo::core::model::RepoRole role;
};

} // end widgets
} // end repo


