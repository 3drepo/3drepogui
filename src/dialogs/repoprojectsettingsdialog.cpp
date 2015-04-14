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

repo::gui::RepoProjectSettingsDialog::RepoProjectSettingsDialog(
        core::RepoProjectSettings projectSettings,
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

    if (!projectSettings.isOk())
    {
        ui->nameLineEdit->setText(QString::fromStdString(projectSettings.getProject()));
        ui->descriptionLineEdit->setText(QString::fromStdString(projectSettings.getDescription()));
        ui->typeComboBox->setCurrentText(QString::fromStdString(projectSettings.getType()));
        ui->ownerComboBox->setCurrentText(QString::fromStdString(projectSettings.getOwner()));
        ui->groupComboBox->setCurrentText(QString::fromStdString(projectSettings.getGroup()));
        updatePermissionsOctal(projectSettings.getPermissionsOctal());
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

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(unsigned short value)
{
    //--------------------------------------------------------------------------
    setPermissionsCheckBoxes(value,
                             ui->uidCheckBox, core::RepoPermissionsBitMask::UID_BIT,
                             ui->gidCheckBox, core::RepoPermissionsBitMask::GID_BIT,
                             ui->stickyBitCheckBox, core::RepoPermissionsBitMask::STICKY_BIT);
    //--------------------------------------------------------------------------
    setPermissionsCheckBoxes(value,
                             ui->ownerReadCheckBox, core::RepoPermissionsBitMask::OWNER_READ,
                             ui->ownerWriteCheckBox, core::RepoPermissionsBitMask::OWNER_WRITE,
                             ui->ownerExecuteCheckBox, core::RepoPermissionsBitMask::OWNER_EXECUTE);
    //--------------------------------------------------------------------------
    setPermissionsCheckBoxes(value,
                             ui->groupReadCheckBox, core::RepoPermissionsBitMask::GROUP_READ,
                             ui->groupWriteCheckBox, core::RepoPermissionsBitMask::GROUP_WRITE,
                             ui->groupExecuteCheckBox, core::RepoPermissionsBitMask::GROUP_EXECUTE);
    //--------------------------------------------------------------------------
    setPermissionsCheckBoxes(value,
                             ui->publicReadCheckBox, core::RepoPermissionsBitMask::PUBLIC_READ,
                             ui->publicWriteCheckBox, core::RepoPermissionsBitMask::PUBLIC_WRITE,
                             ui->publicExecuteCheckBox, core::RepoPermissionsBitMask::PUBLIC_EXECUTE);
}

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(const QString &value)
{
    disconnectPermissionsOctal();

    std::string octal = value.toStdString();
    for (int i = 0; i < 4 - value.size(); ++i)
        octal += "0";
    updatePermissionsOctal(core::RepoProjectSettings::stringToOctal(octal));
    connectPermissionsOctal();
}

unsigned short repo::gui::RepoProjectSettingsDialog::getOctal(
        const QCheckBox *r,
        const QCheckBox *w,
        const QCheckBox *x)
{
    unsigned short value = 0;
    value += r->isChecked() ? core::RepoRWXPermissionsBitMask::READ : 0;
    value += w->isChecked() ? core::RepoRWXPermissionsBitMask::WRITE : 0;
    value += x->isChecked() ? core::RepoRWXPermissionsBitMask::EXECUTE : 0;
    return value;
}

void repo::gui::RepoProjectSettingsDialog::setOctal(
        QCheckBox *r,
        QCheckBox *w,
        QCheckBox *x,
        int value)
{
    r->setChecked(value & core::RepoRWXPermissionsBitMask::READ);
    w->setChecked(value & core::RepoRWXPermissionsBitMask::WRITE);
    x->setChecked(value & core::RepoRWXPermissionsBitMask::EXECUTE);
}


repo::core::RepoBSON repo::gui::RepoProjectSettingsDialog::getCommand() const
{
    core::RepoProjectSettings projectSettings =
            core::RepoProjectSettings(
                ui->nameLineEdit->text().toStdString(),
                ui->descriptionLineEdit->text().toStdString(),
                ui->typeComboBox->currentText().toStdString(),
                ui->ownerComboBox->currentText().toStdString(),
                ui->groupComboBox->currentText().toStdString(),
                getOctal(ui->ownerReadCheckBox, ui->ownerWriteCheckBox, ui->ownerExecuteCheckBox),
                getOctal(ui->groupReadCheckBox, ui->groupWriteCheckBox, ui->groupExecuteCheckBox),
                getOctal(ui->publicReadCheckBox, ui->publicWriteCheckBox, ui->publicExecuteCheckBox));
    return projectSettings.upsert();
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
