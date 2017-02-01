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

using namespace repo::gui::dialog;

ProjectDialog::ProjectDialog(
        const repo::core::model::RepoProjectSettings &projectSettings,
        bool isCopy,
        QWidget *parent) :
    QDialog(parent),
    projectSettings(projectSettings),
    ui(new Ui::ProjectDialog)
{
    ui->setupUi(this);

    if (!projectSettings.isEmpty())
    {
        QString projectName = QString::fromStdString(projectSettings.getProjectName());

        if (isCopy)
            projectName += " " + tr("(Copy)");

        ui->nameLineEdit->setText(projectName);
        ui->projectNameGroupBox->setChecked(isCopy && !projectSettings.isEmpty());

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

ProjectDialog::~ProjectDialog()
{
    delete ui;
}


repo::core::model::RepoProjectSettings ProjectDialog::getSettings() const
{

    auto projectChanges =  repo::core::model::RepoBSONFactory::makeRepoProjectSettings(
                ui->nameLineEdit->text().toStdString(),
                ui->ownerComboBox->currentText().toStdString(),
                projectSettings.isFederate(),
                ui->typeComboBox->currentText().toStdString(),
                ui->descriptionPlainTextEdit->toPlainText().toStdString(),
                ui->pinSizeDoubleSpinBox->value(),
                ui->avatarHeightDoubleSpinBox->value(),
                ui->visibilityLimitDoubleSpinBox->value(),
                ui->speedDoubleSpinBox->value(),
                ui->zNearDoubleSpinBox->value(),
                ui->zFarDoubleSpinBox->value());

    return projectSettings.cloneAndMergeProjectSettings(projectChanges);
}

void ProjectDialog::explainDefaultValue(QLabel *label, double value)
{
    QLocale locale;
    label->setText(
                label->text() +
                " " +
                tr("Default is") +
                " " +
                locale.toString(value) + ".");
}
