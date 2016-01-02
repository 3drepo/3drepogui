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
#include "../primitives/repo_fontawesome.h"
#include "../../settings/repo_settings_credentials.h"

namespace Ui {
class ConnectManagerDialog;
}

namespace repo{
namespace gui {
namespace dialog {


class ConnectManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectManagerDialog(repo::RepoController *controller, QWidget *parent = 0);
    ~ConnectManagerDialog();

public slots :

    int exec();

    void refresh();

    repo::RepoCredentials getConnection();

private:
    Ui::ConnectManagerDialog *ui;

    repo::RepoController *controller;
};

} // end namespace widget
}
} // end namespace repo
