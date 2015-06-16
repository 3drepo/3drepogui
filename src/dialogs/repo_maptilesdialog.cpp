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


#include "repo_maptilesdialog.h"
#include "ui_repo_maptilesdialog.h"

repo::gui::RepoMapTilesDialog::RepoMapTilesDialog(QWidget *parent,
        const core::RepoMap &map)
    : QDialog(parent)
    , map(map)
    , ui(new Ui::RepoMapTilesDialog)
{
    ui->setupUi(this);
}

repo::gui::RepoMapTilesDialog::~RepoMapTilesDialog()
{
    delete ui;
}

repo::core::RepoMap repo::gui::RepoMapTilesDialog::getMap()
{
    // TODO: populate map object with values from the UI
    // Remember, BSON is immutable.
    return map;
}
