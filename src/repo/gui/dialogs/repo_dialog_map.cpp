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


#include "repo_dialog_map.h"
#include "ui_repo_dialog_map.h"

#include <repo/core/model/bson/repo_bson_factory.h>

using namespace repo::gui::dialog;

MapDialog::MapDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MapDialog)
{
    ui->setupUi(this);

    this->setWindowIcon(repo::gui::primitive::RepoFontAwesome::getMapTilesIcon());
}

MapDialog::~MapDialog()
{
    delete ui;
}

repo::core::model::MapNode MapDialog::getMap(){

    repo_vector_t centrePoint = { (float)ui->xSpinBox->value(), (float)ui->ySpinBox->value(), (float)ui->zSpinBox->value() };

   return repo::core::model::RepoBSONFactory::makeMapNode(
                ui->widthSpinBox->value(),
				ui->zoomSpinBox->value(),
				ui->tiltSpinBox->value(),
				ui->tileSpinBox->value(),
				ui->longSpinBox->value(),
				ui->latSpinBox->value(),
				centrePoint,
                ui->apiKeyLineEdit->text().toStdString(),
				ui->nameLineEdit->text().toStdString()
				);

}

