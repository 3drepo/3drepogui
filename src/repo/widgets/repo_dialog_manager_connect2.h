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


//------------------------------------------------------------------------------
// CORE
#include <repo/repo_controller.h>
#include <repo/repo_credentials.h>

//------------------------------------------------------------------------------
// GUI
#include "repo_dialog_connect.h"
#include "../settings/repo_settings_credentials.h"
#include "../../primitives/repo_fontawesome.h"

namespace Ui {
class RepoDialogManagerConnect2;
}

namespace repo {
namespace widgets {


class RepoDialogManagerConnect2 : public QDialog
{
    Q_OBJECT

public:
    explicit RepoDialogManagerConnect2(repo::RepoController *controller, QWidget *parent = 0);
    ~RepoDialogManagerConnect2();

public slots :

    int exec();

    void refresh();

private:
    Ui::RepoDialogManagerConnect2 *ui;

    repo::RepoController *controller;
};

} // end namespace widgets
} // end namespace repo
