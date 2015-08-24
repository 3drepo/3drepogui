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

repo::gui::RepoProjectSettingsDialog::RepoProjectSettingsDialog(
	repo::core::model::RepoProjectSettings projectSettings,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoProjectSettingsDialog)
{
    ui->setupUi(this);

    checkboxes << ui->uidCheckBox << ui->gidCheckBox << ui->stickyBitCheckBox;
    checkboxes << ui->ownerReadCheckBox << ui->ownerWriteCheckBox << ui->ownerExecuteCheckBox;
    checkboxes << ui->groupReadCheckBox << ui->groupWriteCheckBox << ui->groupExecuteCheckBox;
    checkboxes << ui->publicReadCheckBox << ui->publicWriteCheckBox << ui->publicExecuteCheckBox;

    connectPermissionsOctal();
    //--------------------------------------------------------------------------
    QObject::connect(ui->octalLineEdit, SIGNAL(textEdited(const QString &)),
                     this, SLOT(updatePermissionsOctal(const QString &)));

    if (!projectSettings.isEmpty())
    {
        ui->nameLineEdit->setText(QString::fromStdString(projectSettings.getProjectName()));
        ui->descriptionLineEdit->setText(QString::fromStdString(projectSettings.getDescription()));
        ui->typeComboBox->setCurrentText(QString::fromStdString(projectSettings.getType()));
        ui->ownerComboBox->setCurrentText(QString::fromStdString(projectSettings.getOwner()));
        ui->groupComboBox->setCurrentText(QString::fromStdString(projectSettings.getGroup()));
        updatePermissionsOctal(projectSettings.getPermissionsBoolean());
    }
    else
    {
        ui->typeComboBox->setCurrentText(tr("Architectural"));
        ui->ownerReadCheckBox->setChecked(true);
        ui->ownerWriteCheckBox->setChecked(true);
        ui->ownerExecuteCheckBox->setChecked(true);
    }
}

repo::gui::RepoProjectSettingsDialog::~RepoProjectSettingsDialog()
{
    delete ui;
}

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal()
{
    unsigned short bits = getOctal(
                ui->uidCheckBox,
                ui->gidCheckBox,
                ui->stickyBitCheckBox);
    unsigned short owner = getOctal(
                ui->ownerReadCheckBox,
                ui->ownerWriteCheckBox,
                ui->ownerExecuteCheckBox);
    unsigned short group = getOctal(
                ui->groupReadCheckBox,
                ui->groupWriteCheckBox,
                ui->groupExecuteCheckBox);
    unsigned short publ = getOctal(
                ui->publicReadCheckBox,
                ui->publicWriteCheckBox,
                ui->publicExecuteCheckBox);

    ui->octalLineEdit->setText(QString::number(bits) +
                               QString::number(owner) +
                               QString::number(group) +
                               QString::number(publ));
}

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(const std::vector<bool> perm)
{

	uint32_t ind = 0;


	//NOTE: Order is important here.

	//uid/gid/sticky.... not even used at the moment
	ui->uidCheckBox->setChecked(perm[ind++]);
	ui->gidCheckBox->setChecked(perm[ind++]);
	ui->stickyBitCheckBox->setChecked(perm[ind++]);

	//Owner
	ui->ownerReadCheckBox->setChecked(perm[ind++]);
	ui->ownerWriteCheckBox->setChecked(perm[ind++]);
	ui->ownerExecuteCheckBox->setChecked(perm[ind++]);

	//group
	ui->groupReadCheckBox->setChecked(perm[ind++]);
	ui->groupWriteCheckBox->setChecked(perm[ind++]);
	ui->groupExecuteCheckBox->setChecked(perm[ind++]);

	//public
	ui->publicReadCheckBox->setChecked(perm[ind++]);
	ui->publicWriteCheckBox->setChecked(perm[ind++]);
	ui->publicExecuteCheckBox->setChecked(perm[ind]);
}

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(const QString &value)
{
    disconnectPermissionsOctal();

    std::string octal = value.toStdString();
    for (int i = 0; i < 4 - value.size(); ++i)
        octal += "0";
    updatePermissionsOctal(repo::core::model::RepoProjectSettings::stringToPermissionsBool(octal));
    connectPermissionsOctal();
}

unsigned short repo::gui::RepoProjectSettingsDialog::getOctal(
        const QCheckBox *r,
        const QCheckBox *w,
        const QCheckBox *x)
{
    unsigned short value = 0;
	value += r->isChecked() ? repo::core::model::RepoProjectSettings::READVAL : 0;
	value += w->isChecked() ? repo::core::model::RepoProjectSettings::WRITEVAL : 0;
	value += x->isChecked() ? repo::core::model::RepoProjectSettings::EXECUTEVAL : 0;
    return value;
}

void repo::gui::RepoProjectSettingsDialog::setOctal(
        QCheckBox *r,
        QCheckBox *w,
        QCheckBox *x,
        int value)
{
	r->setChecked(value & repo::core::model::RepoProjectSettings::READVAL);
	w->setChecked(value & repo::core::model::RepoProjectSettings::WRITEVAL);
	x->setChecked(value & repo::core::model::RepoProjectSettings::EXECUTEVAL);
}


repo::core::model::RepoProjectSettings repo::gui::RepoProjectSettingsDialog::getSettings() const
{
	repo::core::model::RepoProjectSettings projectSettings =
		repo::core::model::RepoBSONFactory::makeRepoProjectSettings(
                ui->nameLineEdit->text().toStdString(),
                ui->ownerComboBox->currentText().toStdString(),
                ui->groupComboBox->currentText().toStdString(),
                ui->typeComboBox->currentText().toStdString(),
                ui->descriptionLineEdit->text().toStdString(),
                getOctal(ui->ownerReadCheckBox, ui->ownerWriteCheckBox, ui->ownerExecuteCheckBox),
                getOctal(ui->groupReadCheckBox, ui->groupWriteCheckBox, ui->groupExecuteCheckBox),
                getOctal(ui->publicReadCheckBox, ui->publicWriteCheckBox, ui->publicExecuteCheckBox));

    return projectSettings;
}

void repo::gui::RepoProjectSettingsDialog::connectPermissionsOctal()
{
    for (QCheckBox *checkBox : checkboxes)
        QObject::connect(checkBox, SIGNAL(toggled(bool)),
                         this, SLOT(updatePermissionsOctal()));
}

void repo::gui::RepoProjectSettingsDialog::disconnectPermissionsOctal()
{
    for (QCheckBox *checkBox : checkboxes)
        QObject::disconnect(checkBox, SIGNAL(toggled(bool)),
                         this, SLOT(updatePermissionsOctal()));
}

void repo::gui::RepoProjectSettingsDialog::setPermissionsCheckBoxes(
        unsigned short octal,
        QCheckBox *r, unsigned short rMask,
        QCheckBox *w, unsigned short wMask,
        QCheckBox *x, unsigned short xMask)
{
    r->setChecked((octal & rMask) == rMask);
    w->setChecked((octal & wMask) == wMask);
    x->setChecked((octal & xMask) == xMask);
}
