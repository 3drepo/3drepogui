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


#include "repo_dialog_access_manager.h"
#include "ui_repo_dialog_access_manager.h"

using namespace repo::widgets;

RepoDialogAccessManager::RepoDialogAccessManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoDialogAccessManager)
{
    ui->setupUi(this);
}

RepoDialogAccessManager::~RepoDialogAccessManager()
{
    delete ui;
}
