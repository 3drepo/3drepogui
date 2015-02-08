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


#include "repowidgetassimpflags.h"
#include "ui_repowidgetassimpflags.h"

repo::gui::RepoWidgetAssimpFlags::RepoWidgetAssimpFlags(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepoWidgetAssimpFlags)
{
    ui->setupUi(this);

    QObject::connect(ui->improveCacheLocalityCheckBox, SIGNAL(toggled(bool)),
                     ui->improveCacheLocalitySpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->splitByBoneCountCheckBox, SIGNAL(toggled(bool)),
                     ui->splitByBoneCountSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->limitBoneWeightsCheckBox, SIGNAL(toggled(bool)),
                     ui->limitBoneWeightsSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->generateNormalsSmoothRadioButton, SIGNAL(toggled(bool)),
                     ui->generateNormalsSmoothDoubleSpinBox, SLOT(setEnabled(bool)));
}

repo::gui::RepoWidgetAssimpFlags::~RepoWidgetAssimpFlags()
{
    delete ui;
}
