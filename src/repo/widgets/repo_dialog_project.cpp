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


#include "repo_dialog_project.h"
#include "ui_repo_dialog_project.h"

#include <repo/core/model/bson/repo_bson_factory.h>

using namespace repo::widgets;

RepoDialogProject::RepoDialogProject(
        repo::core::model::RepoProjectSettings projectSettings,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoDialogProject)
{
    ui->setupUi(this);

    if (!projectSettings.isEmpty())
    {
        ui->nameLineEdit->setText(QString::fromStdString(projectSettings.getProjectName()));
        ui->descriptionPlainTextEdit->setPlainText(QString::fromStdString(projectSettings.getDescription()));
        ui->typeComboBox->setCurrentText(QString::fromStdString(projectSettings.getType()));
        ui->ownerComboBox->setCurrentText(QString::fromStdString(projectSettings.getOwner()));


        ui->pinSizeDoubleSpinBox->setValue(projectSettings.getPinSize());
        ui->avatarHeightDoubleSpinBox->setValue(projectSettings.getAvatarHeight());
        ui->visibilityLimitDoubleSpinBox->setValue(projectSettings.getVisibilityLimit());
        ui->speedDoubleSpinBox->setValue(projectSettings.getSpeed());
        ui->zFarDoubleSpinBox->setValue(projectSettings.getZFar());
        ui->zNearDoubleSpinBox->setValue(projectSettings.getZNear());
    }

    explainDefaultValue(ui->pinSizeExplanationLabel, REPO_DEFAULT_PROJECT_PIN_SIZE);
    explainDefaultValue(ui->avatarHeightExplanationLabel, REPO_DEFAULT_PROJECT_AVATAR_HEIGHT);
    explainDefaultValue(ui->visibilityLimitExplanationLabel, REPO_DEFAULT_PROJECT_VISIBILITY_LIMIT);
    explainDefaultValue(ui->speedExplanationLabel, REPO_DEFAULT_PROJECT_SPEED);
    explainDefaultValue(ui->zFarExplanationLabel, REPO_DEFAULT_PROJECT_ZFAR);
    explainDefaultValue(ui->zNearExplanationLabel, REPO_DEFAULT_PROJECT_ZNEAR);
}

RepoDialogProject::~RepoDialogProject()
{
    delete ui;
}


repo::core::model::RepoProjectSettings RepoDialogProject::getSettings() const
{
    // TODO: add some kind of validation to input fields.

    // TODO: remove group, octal settings and add new properties
    // subbson: pinSize, avatarHeight, visibilityLimit, speed, zNear, zFar
    return repo::core::model::RepoBSONFactory::makeRepoProjectSettings(
                ui->nameLineEdit->text().toStdString(),
                ui->ownerComboBox->currentText().toStdString(),
                ui->typeComboBox->currentText().toStdString(),
                ui->descriptionPlainTextEdit->toPlainText().toStdString(),
                ui->pinSizeDoubleSpinBox->value(),
                ui->avatarHeightDoubleSpinBox->value(),
                ui->visibilityLimitDoubleSpinBox->value(),
                ui->speedDoubleSpinBox->value(),
                ui->zNearDoubleSpinBox->value(),
                ui->zFarDoubleSpinBox->value());
}

void RepoDialogProject::explainDefaultValue(QLabel *label, double value)
{
    QLocale locale;
    label->setText(
                label->text() +
                " " +
                tr("Default is") +
                " " +
                locale.toString(value) + ".");
}
