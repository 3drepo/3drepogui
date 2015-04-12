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

    if (!projectSettings.isEmpty())
    {
        ui->nameLineEdit->setText(QString::fromStdString(projectSettings.getProject()));
        ui->descriptionLineEdit->setText(QString::fromStdString(projectSettings.getDescription()));
        ui->typeComboBox->setCurrentText(QString::fromStdString(projectSettings.getType()));
        ui->ownerComboBox->setCurrentText(QString::fromStdString(projectSettings.getOwner()));
        ui->groupComboBox->setCurrentText(QString::fromStdString(projectSettings.getGroup()));
        updatePermissionsOctal(projectSettings.getPermissionsOctal());
    }
    else
        ui->typeComboBox->setCurrentText(tr("Architectural"));

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

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(unsigned short value)
{
    ui->uidCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::UID_BIT)
                == core::RepoPermissionsBitMask::UID_BIT);
    ui->gidCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::GID_BIT)
                == core::RepoPermissionsBitMask::GID_BIT);
    ui->stickyBitCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::STICKY_BIT)
                == core::RepoPermissionsBitMask::STICKY_BIT);
    //--------------------------------------------------------------------------
    ui->ownerReadCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::OWNER_READ)
                == core::RepoPermissionsBitMask::OWNER_READ);
    ui->ownerWriteCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::OWNER_WRITE)
                == core::RepoPermissionsBitMask::OWNER_WRITE);
    ui->ownerExecuteCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::OWNER_EXECUTE)
                == core::RepoPermissionsBitMask::OWNER_EXECUTE);
    //--------------------------------------------------------------------------
    ui->groupReadCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::GROUP_READ)
                == core::RepoPermissionsBitMask::GROUP_READ);
    ui->groupWriteCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::GROUP_WRITE)
                == core::RepoPermissionsBitMask::GROUP_WRITE);
    ui->groupExecuteCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::GROUP_EXECUTE)
                == core::RepoPermissionsBitMask::GROUP_EXECUTE);
    //--------------------------------------------------------------------------
    ui->publicReadCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::PUBLIC_READ)
                == core::RepoPermissionsBitMask::PUBLIC_READ);
    ui->publicWriteCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::PUBLIC_WRITE)
                == core::RepoPermissionsBitMask::PUBLIC_WRITE);
    ui->publicExecuteCheckBox->setChecked(
                (value & core::RepoPermissionsBitMask::PUBLIC_EXECUTE)
                == core::RepoPermissionsBitMask::PUBLIC_EXECUTE);
}

void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(const QString &text)
{
    int offset = 1;
    for (int i = 0; i < text.size() && i < 4; ++i)
    {
        QChar c = text.at(i);
        if (c.isDigit())
            updatePermissionsOctal(i + offset, c.digitValue());
    }
}


void repo::gui::RepoProjectSettingsDialog::updatePermissionsOctal(int index, int value)
{
    switch (index)
    {
    case BITS :
        setOctal(ui->uidCheckBox,
                 ui->gidCheckBox,
                 ui->stickyBitCheckBox, value);
        break;
    case OWNER :
        setOctal(ui->ownerReadCheckBox,
                 ui->ownerWriteCheckBox,
                 ui->ownerExecuteCheckBox, value);
        break;
    case GROUP :
        setOctal(ui->groupReadCheckBox,
                 ui->groupWriteCheckBox,
                 ui->groupExecuteCheckBox, value);
        break;
    case PUBLIC :
        setOctal(ui->publicReadCheckBox,
                 ui->publicWriteCheckBox,
                 ui->publicExecuteCheckBox, value);
        break;
    }
}


unsigned int repo::gui::RepoProjectSettingsDialog::getOctal(
        const QCheckBox *r,
        const QCheckBox *w,
        const QCheckBox *x)
{
    unsigned int value = 0;
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

void repo::gui::RepoProjectSettingsDialog::connectPermissionsOctal(
        const QCheckBox *four,
        const QCheckBox *two,
        const QCheckBox *one)
{
    QObject::connect(four, SIGNAL(toggled(bool)),
                     this, SLOT(updatePermissionsOctal()));
    QObject::connect(two, SIGNAL(toggled(bool)),
                     this, SLOT(updatePermissionsOctal()));
    QObject::connect(one, SIGNAL(toggled(bool)),
                     this, SLOT(updatePermissionsOctal()));
}
