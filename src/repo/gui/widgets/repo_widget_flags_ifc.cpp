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
    ui->ElementFilteringGroupBox->setChecked(settings->getUseElementsFiltering());
    ui->excludeElementsRadioButton->setChecked(settings->getIsExclusionFilter());
    ui->includeElementsRadioButton->setChecked(!settings->getIsExclusionFilter());

    ui->elementsFilter->setHeaders({tr("Filtering Keywords")});

    auto keywords = settings->getFilteringKeywords();

    for(const auto &keyword : keywords)
    {
        ui->elementsFilter->addRow({QString::fromStdString(keyword)}, true, true);
    }

    ui->elementsFilter->setNewRowText({tr("<New Keyword>")});


    ui->wieldVerticesCheckBox->setChecked(settings->getWieldVertices());
    ui->useWorldCoordCeckBox->setChecked(settings->getUseWorldCoords());
    ui->convertUnitsCheckBox->setChecked(settings->getConvertUnits());
    ui->useBRepCheckBox->setChecked(settings->getUseBRepData());
    ui->sewShellsCheckBox->setChecked(settings->getSewShells());
    ui->fasterBooleansCheckBox->setChecked(settings->getFasterBooleans());
    ui->disableOpeningSubCheckBox->setChecked(settings->getNoOpeningSubtractions());
    ui->NoTriangulationCheckBox->setChecked(settings->getNoTriangulation());
    ui->applyDefaultCheckBox->setChecked(settings->getUseDefaultMaterials());
    ui->includeCurvesCheckBox->setChecked(settings->getIncludeAllCurves());
    ui->noSolidsSurfacesCheckBox->setChecked(settings->getDisableSolidSurfaces());
    ui->noNormalsCheckBox->setChecked(settings->getNoNormals());
    ui->useElementGUIDCheckBox->setChecked(settings->getUseElementGuids());
    ui->useElementNamesCheckBox->setChecked(settings->getUseElementNames());
    ui->useMatNamesCheckBox->setChecked(settings->getUseMaterialNames());
    ui->centralModelCheckBox->setChecked(settings->getCentreModels());
    ui->genUVCheckBox->setChecked(settings->getGenerateUVs());
    ui->applyLayerSetsCheckBox->setChecked(settings->getApplyLayerSets());


}

IFCFlagsWidget::~IFCFlagsWidget()
{
	delete settings;
    delete ui;
}

void IFCFlagsWidget::apply()
{    
    settings->setUseIFCOpenShell(ui->useIFCOpenShellGroupBox->isChecked());
    settings->setUseElementsFiltering(ui->ElementFilteringGroupBox->isChecked());
    settings->setIsExclusionFilter(ui->excludeElementsRadioButton->isChecked());
    settings->setFilteringKeywords(ui->elementsFilter->getItemsAsVectorOfStrings());

    settings->setWieldVertices(ui->wieldVerticesCheckBox->isChecked());
    settings->setUseWorldCoords(ui->useWorldCoordCeckBox->isChecked());
    settings->setConvertUnits(ui->convertUnitsCheckBox->isChecked());
    settings->setUseBRepData(ui->useBRepCheckBox->isChecked());
    settings->setSewShells(ui->sewShellsCheckBox->isChecked());
    settings->setFasterBooleans(ui->fasterBooleansCheckBox->isChecked());
    settings->setNoOpeningSubtractions(ui->disableOpeningSubCheckBox->isChecked());
    settings->setNoTriangulation(ui->NoTriangulationCheckBox->isChecked());
    settings->setUseDefaultMaterials(ui->applyDefaultCheckBox->isChecked());
    settings->setIncludeAllCurves(ui->includeCurvesCheckBox->isChecked());
    settings->setDisableSolidSurfaces(ui->noSolidsSurfacesCheckBox->isChecked());
    settings->setNoNormals(ui->noNormalsCheckBox->isChecked());
    settings->setUseElementGuids(ui->useElementGUIDCheckBox->isChecked());
    settings->setUseElementNames(ui->useElementNamesCheckBox->isChecked());
    settings->setUseMaterialNames(ui->useMatNamesCheckBox->isChecked());
    settings->setCentreModels(ui->centralModelCheckBox->isChecked());
    settings->setGenerateUVs(ui->genUVCheckBox->isChecked());
    settings->setApplyLayerSets(ui->applyLayerSetsCheckBox->isChecked());
}

void IFCFlagsWidget::reset()
{
    ui->useIFCOpenShellGroupBox->setChecked(false);
    ui->ElementFilteringGroupBox->setChecked(repoDefaultIOSUseFilter);
    ui->excludeElementsRadioButton->setChecked(repoDefaultIsExclusion);
    ui->includeElementsRadioButton->setChecked(!repoDefaultIsExclusion);

    ui->wieldVerticesCheckBox->setChecked(repoDefaultIOSWieldVertices);
    ui->useWorldCoordCeckBox->setChecked(repoDefaultIOSUseWorldCoords);
    ui->convertUnitsCheckBox->setChecked(repoDefaultIOSConvertBackUnits);
    ui->useBRepCheckBox->setChecked(repoDefaultIOSUseBrepData);
    ui->sewShellsCheckBox->setChecked(repoDefaultIOSSewShells);
    ui->fasterBooleansCheckBox->setChecked(repoDefaultIOSFasterBooleans);
    ui->disableOpeningSubCheckBox->setChecked(repoDefaultIOSDisableOpeningSubtractions);
    ui->NoTriangulationCheckBox->setChecked(repoDefaultIOSDisableTriangulate);
    ui->applyDefaultCheckBox->setChecked(repoDefaultIOSApplyDefaultMaterials);
    ui->includeCurvesCheckBox->setChecked(repoDefaultIOSIncludesCurves);
    ui->noSolidsSurfacesCheckBox->setChecked(repoDefaultIOSExcludesSolidsAndSurfaces);
    ui->noNormalsCheckBox->setChecked(repoDefaultIOSNoNormals);
    ui->useElementGUIDCheckBox->setChecked(repoDefaultIOSUseElementGuids);
    ui->useElementNamesCheckBox->setChecked(repoDefaultIOSUseElementNames);
    ui->useMatNamesCheckBox->setChecked(repoDefaultIOSUseMatNames);
    ui->centralModelCheckBox->setChecked(repoDefaultIOSCentreModel);
    ui->genUVCheckBox->setChecked(repoDefaultIOSGenerateUVs);
    ui->applyLayerSetsCheckBox->setChecked(repoDefaultIOSApplyLayerSets);
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
