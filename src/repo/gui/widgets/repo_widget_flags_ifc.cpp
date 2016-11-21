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


#include "repo_widget_flags_ifc.h"
#include "ui_repo_widget_flags_ifc.h"

using namespace repo::gui::widget;

//------------------------------------------------------------------------------

IFCFlagsWidget::IFCFlagsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IFCFlagsWidget)
{
    ui->setupUi(this);
    settings = new repo::settings::RepoSettings();
    //--------------------------------------------------------------------------

    ui->useIFCOpenShellGroupBox->setChecked(settings->getUseIFCOpenShell());


    //--------------------------------------------------------------------------

//    QObject::connect(ui->calculateTangentSpaceCheckBox, SIGNAL(toggled(bool)),
//                     ui->calculateTangentSpaceDoubleSpinBox, SLOT(setEnabled(bool)));

//    QObject::connect(ui->improveCacheLocalityCheckBox, SIGNAL(toggled(bool)),
//                     ui->improveCacheLocalitySpinBox, SLOT(setEnabled(bool)));

//    QObject::connect(ui->splitByBoneCountCheckBox, SIGNAL(toggled(bool)),
//                     ui->splitByBoneCountSpinBox, SLOT(setEnabled(bool)));

//    QObject::connect(ui->limitBoneWeightsCheckBox, SIGNAL(toggled(bool)),
//                     ui->limitBoneWeightsSpinBox, SLOT(setEnabled(bool)));

//    QObject::connect(ui->generateNormalsSmoothRadioButton, SIGNAL(toggled(bool)),
//                     ui->generateNormalsSmoothDoubleSpinBox, SLOT(setEnabled(bool)));

//    QObject::connect(ui->generateNormalsGroupBox, SIGNAL(toggled(bool)),
//                     this, SLOT(setCreaseAngleEnabled(bool)));

//    QObject::connect(ui->preDefinedSettingsComboBox, SIGNAL(currentIndexChanged(int)),
//                     this, SLOT(setPredefinedSettings(int)));
}

IFCFlagsWidget::~IFCFlagsWidget()
{
	delete settings;
    delete ui;
}

void IFCFlagsWidget::apply()
{    
    settings->setUseIFCOpenShell(ui->useIFCOpenShellGroupBox->isChecked());
}

void IFCFlagsWidget::reset()
{
    ui->useIFCOpenShellGroupBox->setChecked(false);
}


void IFCFlagsWidget::setPredefinedSettings(int selection)
{
    switch (selection)
    {
    case DEFAULT :
        reset();
        break;    
    }
}
