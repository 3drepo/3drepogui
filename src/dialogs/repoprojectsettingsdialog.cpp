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

    connectPermissionsOctal(
                    ui->uidCheckBox,
                    ui->gidCheckBox,
                    ui->stickyBitCheckBox);
    //--------------------------------------------------------------------------
    connectPermissionsOctal(
                ui->ownerReadCheckBox,
                ui->ownerWriteCheckBox,
                ui->ownerExecuteCheckBox);
    //--------------------------------------------------------------------------
    connectPermissionsOctal(
                ui->groupReadCheckBox,
                ui->groupWriteCheckBox,
                ui->groupExecuteCheckBox);
    //--------------------------------------------------------------------------
    connectPermissionsOctal(
                ui->publicReadCheckBox,
                ui->publicWriteCheckBox,
                ui->publicExecuteCheckBox);
}

repo::gui::RepoProjectSettingsDialog::~RepoProjectSettingsDialog()
{
    delete ui;
}

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal()
{
    unsigned int bits = getOctal(
                ui->uidCheckBox,
                ui->gidCheckBox,
                ui->stickyBitCheckBox);
    unsigned int owner = getOctal(
                ui->ownerReadCheckBox,
                ui->ownerWriteCheckBox,
                ui->ownerExecuteCheckBox);
    unsigned int group = getOctal(
                ui->groupReadCheckBox,
                ui->groupWriteCheckBox,
                ui->groupExecuteCheckBox);
    unsigned int publ = getOctal(
                ui->publicReadCheckBox,
                ui->publicWriteCheckBox,
                ui->publicExecuteCheckBox);

    ui->octalLineEdit->setText(QString::number(bits) +
                               QString::number(owner) +
                               QString::number(group) +
                               QString::number(publ));
}

unsigned int repo::gui::RepoProjectSettingsDialog::getOctal(
        const QCheckBox *four,
        const QCheckBox *two,
        const QCheckBox *one)
{
    unsigned int value = 0;
    value += four->isChecked() ? 4 : 0;
    value += two->isChecked() ? 2 : 0;
    value += one->isChecked() ? 1 : 0;
    return value;
}

void repo::gui::RepoProjectSettingsDialog::connectPermissionsOctal(
        const QCheckBox *four,
        const QCheckBox *two,
        const QCheckBox *one)
{
    QObject::connect(four, &QCheckBox::toggled,
                     this, &RepoProjectSettingsDialog::updatePermissionsOctal);
    QObject::connect(two, &QCheckBox::toggled,
                     this, &RepoProjectSettingsDialog::updatePermissionsOctal);
    QObject::connect(one, &QCheckBox::toggled,
                     this, &RepoProjectSettingsDialog::updatePermissionsOctal);
}
