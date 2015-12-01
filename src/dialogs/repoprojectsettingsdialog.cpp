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


#include "repoprojectsettingsdialog.h"
#include "ui_repoprojectsettingsdialog.h"

#include <repo/core/model/bson/repo_bson_factory.h>

using namespace repo::gui;

RepoProjectSettingsDialog::RepoProjectSettingsDialog(
	repo::core::model::RepoProjectSettings projectSettings,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoProjectSettingsDialog)
{
    ui->setupUi(this);

    if (!projectSettings.isEmpty())
    {
        ui->nameLineEdit->setText(QString::fromStdString(projectSettings.getProjectName()));
        ui->descriptionPlainTextEdit->setPlainText(QString::fromStdString(projectSettings.getDescription()));
        ui->typeComboBox->setCurrentText(QString::fromStdString(projectSettings.getType()));
        ui->ownerComboBox->setCurrentText(QString::fromStdString(projectSettings.getOwner()));


        // pinSize
        // AvatarHeight
        // VisibilityLimit
        // Speed
        // zNear
        // zFar
    }
}

RepoProjectSettingsDialog::~RepoProjectSettingsDialog()
{
    delete ui;
}


repo::core::model::RepoProjectSettings repo::gui::RepoProjectSettingsDialog::getSettings() const
{
    // TODO: add some kind of validation to input fields.

    // TODO: remove group, octal settings and add new properties
    // subbson: pinSize, avatarHeight, visibilityLimit, speed, zNear, zFar
    return repo::core::model::RepoBSONFactory::makeRepoProjectSettings(
                ui->nameLineEdit->text().toStdString(),
                ui->ownerComboBox->currentText().toStdString(),
                std::string(),
                ui->typeComboBox->currentText().toStdString(),
                ui->descriptionPlainTextEdit->toPlainText().toStdString());
}
