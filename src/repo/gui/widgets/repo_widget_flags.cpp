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


#include "repo_widget_flags.h"
#include "ui_repo_widget_flags.h"

using namespace repo::gui::widget;

//------------------------------------------------------------------------------

FlagsWidget::FlagsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlagsWidget)
{
    ui->setupUi(this);
    settings = new repo::settings::RepoSettings();
    //--------------------------------------------------------------------------

    ui->calculateTangentSpaceCheckBox->setChecked(settings->getCalculateTangentSpace());

    ui->calculateTangentSpaceDoubleSpinBox->setValue(settings->getCalculateTangentSpaceMaxSmoothingAngle());

    ui->convertToUVCoordinatesCheckBox->setChecked(settings->getConvertToUVCoordinates());

    ui->degeneratesToPointsLinesCheckBox->setChecked(settings->getDegeneratesToPointsLines());

    ui->deboneGroupBox->setChecked(settings->getDebone());

    ui->deboneThresholdDoubleSpinBox->setValue(settings->getDeboneThreshold());

    ui->deboneIfAndOnlyIfCheckBox->setChecked(settings->getDeboneOnlyIfAll());

    ui->findInstancesCheckBox->setChecked(settings->getFindInstances());

    ui->findInvalidDataGroupBox->setChecked(settings->getFindInvalidData());

    ui->findInvalidDataAnimationAccuracyDoubleSpinBox->setValue(settings->getFindInvalidDataAnimationAccuracy());

    ui->fixInfacingNormalsCheckBox->setChecked(settings->getFixInfacingNormals());

    ui->flipUVCoordinatesCheckBox->setChecked(settings->getFlipUVCoordinates());

    ui->flipWindingOrderCheckBox->setChecked(settings->getFlipWindingOrder());

    ui->generateNormalsGroupBox->setChecked(settings->getGenerateNormals());

    ui->generateNormalsFlatRadioButton->setChecked(settings->getGenerateNormalsFlat());

    ui->generateNormalsSmoothRadioButton->setChecked(settings->getGenerateNormalsSmooth());

    ui->generateNormalsSmoothDoubleSpinBox->setValue(settings->getGenerateNormalsSmoothCreaseAngle());

    ui->improveCacheLocalityCheckBox->setChecked(settings->getImproveCacheLocality());

    ui->improveCacheLocalitySpinBox->setValue(settings->getImproveCacheLocalityVertexCacheSize());

    ui->joinIdenticalVerticesCheckBox->setChecked(settings->getJoinIdenticalVertices());

    ui->limitBoneWeightsCheckBox->setChecked(settings->getLimitBoneWeights());

    ui->limitBoneWeightsSpinBox->setValue(settings->getLimitBoneWeightsMaxWeight());

    ui->makeLeftHandedCheckBox->setChecked(settings->getMakeLeftHanded());

    ui->optimizeMeshesCheckBox->setChecked(settings->getOptimizeMeshes());

    ui->preTransformUVCoordinatesCheckBox->setChecked(settings->getPreTransformUVCoordinates());

    ui->preTransformVerticesGroupBox->setChecked(settings->getPreTransformVertices());

    ui->preTransformVerticesNormalizeCheckBox->setChecked(settings->getPreTransformVerticesNormalize());

    ui->removeComponentsGroupBox->setChecked(settings->getRemoveComponents());

    ui->removeComponentsAnimationsCheckBox->setChecked(settings->getRemoveComponentsAnimations());

    ui->removeComponentsBiTangentsCheckBox->setChecked(settings->getRemoveComponentsBiTangents());

    ui->removeComponentsBoneWeightsCheckBox->setChecked(settings->getRemoveComponentsBoneWeights());

    ui->removeComponentsCamerasCheckBox->setChecked(settings->getRemoveComponentsCameras());

    ui->removeComponentsColorsCheckBox->setChecked(settings->getRemoveComponentsColors());

    ui->removeComponentsLightsCheckBox->setChecked(settings->getRemoveComponentsLights());

    ui->removeComponentsMaterialsCheckBox->setChecked(settings->getRemoveComponentsMaterials());

    ui->removeComponentsMeshesCheckBox->setChecked(settings->getRemoveComponentsMeshes());

    ui->removeComponentsNormalsCheckBox->setChecked(settings->getRemoveComponentsNormals());

    ui->removeComponentsTexturesCheckBox->setChecked(settings->getRemoveComponentsTextures());

    ui->removeComponentsTextureCoordinatesCheckBox->setChecked(settings->getRemoveComponentsTextureCoordinates());

    ui->removeRedundantMaterialsGroupBox->setChecked(settings->getRemoveRedundantMaterials());

    ui->removeRedundantMaterialsSkipLineEdit->setText(QString(settings->getRemoveRedundantMaterialsSkip().c_str()));

    ui->removeRedundantNodesGroupBox->setChecked(settings->getRemoveRedundantNodes());

    ui->removeRedundantNodesSkipLineEdit->setText(QString(settings->getRemoveRedundantNodesSkip().c_str()));

    ui->sortAndRemoveGroupBox->setChecked(settings->getSortAndRemove());

    ui->sortAndRemovePointsCheckBox->setChecked(settings->getSortAndRemovePoints());

    ui->sortAndRemoveLinesCheckBox->setChecked(settings->getSortAndRemoveLines());

    ui->sortAndRemoveTrianglesCheckBox->setChecked(settings->getSortAndRemoveTriangles());

    ui->sortAndRemovePolygonsCheckBox->setChecked(settings->getSortAndRemovePolygons());

    ui->splitByBoneCountCheckBox->setChecked(settings->getSplitByBoneCount());

    ui->splitByBoneCountSpinBox->setValue(settings->getSplitByBoneCountMaxBones());

    ui->splitLargeMeshesGroupBox->setChecked(settings->getSplitLargeMeshes());

    ui->splitLargeMeshesTriangleLimitSpinBox->setValue(settings->getSplitLargeMeshesTriangleLimit());

    ui->splitLargeMeshesVertexLimitSpinBox->setValue(settings->getSplitLargeMeshesVertexLimit());

    ui->triangulateCheckBox->setChecked(settings->getTriangulate());

    ui->validateDataStructuresCheckBox->setChecked(settings->getValidateDataStructures());

    //--------------------------------------------------------------------------

    QObject::connect(ui->calculateTangentSpaceCheckBox, SIGNAL(toggled(bool)),
                     ui->calculateTangentSpaceDoubleSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->improveCacheLocalityCheckBox, SIGNAL(toggled(bool)),
                     ui->improveCacheLocalitySpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->splitByBoneCountCheckBox, SIGNAL(toggled(bool)),
                     ui->splitByBoneCountSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->limitBoneWeightsCheckBox, SIGNAL(toggled(bool)),
                     ui->limitBoneWeightsSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->generateNormalsSmoothRadioButton, SIGNAL(toggled(bool)),
                     ui->generateNormalsSmoothDoubleSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->generateNormalsGroupBox, SIGNAL(toggled(bool)),
                     this, SLOT(setCreaseAngleEnabled(bool)));

    QObject::connect(ui->preDefinedSettingsComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(setPredefinedSettings(int)));
}

FlagsWidget::~FlagsWidget()
{
	delete settings;
    delete ui;
}

void FlagsWidget::apply()
{
    settings->setCalculateTangentSpace(ui->calculateTangentSpaceCheckBox->isChecked());

    settings->setCalculateTangentSpaceMaxSmoothingAngle(ui->calculateTangentSpaceDoubleSpinBox->value());

    settings->setConvertToUVCoordinates(ui->convertToUVCoordinatesCheckBox->isChecked());

    settings->setDegeneratesToPointsLines(ui->degeneratesToPointsLinesCheckBox->isChecked());

    settings->setDebone(ui->deboneGroupBox->isChecked());

    settings->setDeboneThreshold(ui->deboneThresholdDoubleSpinBox->value());

    settings->setDeboneOnlyIfAll(ui->deboneIfAndOnlyIfCheckBox->isChecked());

    settings->setFindInstances(ui->findInstancesCheckBox->isChecked());

    settings->setFindInvalidData(ui->findInvalidDataGroupBox->isChecked());

    settings->setFindInvalidDataAnimationAccuracy(
                      ui->findInvalidDataAnimationAccuracyDoubleSpinBox->value());

    settings->setFixInfacingNormals(ui->fixInfacingNormalsCheckBox->isChecked());

    settings->setFlipUVCoordinates(ui->flipUVCoordinatesCheckBox->isChecked());

    settings->setFlipWindingOrder(ui->flipWindingOrderCheckBox->isChecked());

    settings->setGenerateNormals(ui->generateNormalsGroupBox->isChecked());

    settings->setGenerateNormalsFlat(ui->generateNormalsFlatRadioButton->isChecked());

    settings->setGenerateNormalsSmooth(ui->generateNormalsSmoothRadioButton->isChecked());

    settings->setGenerateNormalsSmoothCreaseAngle(
                      ui->generateNormalsSmoothDoubleSpinBox->value());

    settings->setImproveCacheLocality(ui->improveCacheLocalityCheckBox->isChecked());

    settings->setImproveCacheLocalityCacheSize(
                      ui->improveCacheLocalitySpinBox->value());

    settings->setJoinIdenticalVertices(ui->joinIdenticalVerticesCheckBox->isChecked());

    settings->setLimitBoneWeights(
                      ui->limitBoneWeightsCheckBox->isChecked());

    settings->setLimitBoneWeightsMaxWeights(
                      ui->limitBoneWeightsSpinBox->value());

    settings->setMakeLeftHanded(
                      ui->makeLeftHandedCheckBox->isChecked());

    settings->setOptimizeMeshes(
                      ui->optimizeMeshesCheckBox->isChecked());

    settings->setPreTransformUVCoordinates(
                      ui->preTransformUVCoordinatesCheckBox->isChecked());

    settings->setPreTransformVertices(
                      ui->preTransformVerticesGroupBox->isChecked());

    settings->setPreTransforVerticesNormalize(
                      ui->preTransformVerticesNormalizeCheckBox->isChecked());

    settings->setRemoveComponents(
                      ui->removeComponentsGroupBox->isChecked());

    settings->setRemoveComponentsAnimations(
                      ui->removeComponentsAnimationsCheckBox->isChecked());

    settings->setRemoveComponentsBiTangents(
                      ui->removeComponentsBiTangentsCheckBox->isChecked());

    settings->setRemoveComponentsBoneWeights(
                      ui->removeComponentsBoneWeightsCheckBox->isChecked());

    settings->setRemoveComponentsCameras(
                      ui->removeComponentsCamerasCheckBox->isChecked());

    settings->setRemoveComponentsColors(
                      ui->removeComponentsColorsCheckBox->isChecked());

    settings->setRemoveComponentsLights(
                      ui->removeComponentsLightsCheckBox->isChecked());

    settings->setRemoveComponentsMaterials(
                      ui->removeComponentsMaterialsCheckBox->isChecked());

    settings->setRemoveComponentsMeshes(
                      ui->removeComponentsMeshesCheckBox->isChecked());

    settings->setRemoveComponentsNormals(
                      ui->removeComponentsNormalsCheckBox->isChecked());

    settings->setRemoveComponentsTextureCoordinates(
                      ui->removeComponentsTextureCoordinatesCheckBox->isChecked());

    settings->setRemoveComponentsTextures(
                      ui->removeComponentsTexturesCheckBox->isChecked());

    settings->setRemoveRedundantMaterials(
                      ui->removeRedundantMaterialsGroupBox->isChecked());

    settings->setRemoveRedundantMaterialsSkip(
                      ui->removeRedundantMaterialsSkipLineEdit->text());

    settings->setRemoveRedundantNodes(
                      ui->removeRedundantNodesGroupBox->isChecked());

    settings->setRemoveRedundantNodesSkip(
                      ui->removeRedundantNodesSkipLineEdit->text());

    settings->setSortAndRemove(
                      ui->sortAndRemoveGroupBox->isChecked());

    settings->setSortAndRemoveLines(
                      ui->sortAndRemoveLinesCheckBox->isChecked());

    settings->setSortAndRemovePoints(
                      ui->sortAndRemovePointsCheckBox->isChecked());

    settings->setSortAndRemovePolygons(
                      ui->sortAndRemovePolygonsCheckBox->isChecked());

    settings->setSortAndRemoveTriangles(
                      ui->sortAndRemoveTrianglesCheckBox->isChecked());

    settings->setSplitLargeMeshes(
                      ui->splitLargeMeshesGroupBox->isChecked());

    settings->setSplitLargeMeshesTriangleLimit(
                       ui->splitLargeMeshesTriangleLimitSpinBox->value());

    settings->setSplitLargeMeshesVertexLimit(
                       ui->splitLargeMeshesVertexLimitSpinBox->value());

    settings->setSplitByBoneCount(
                      ui->splitByBoneCountCheckBox->isChecked());

    settings->setSplitByBoneCountMaxBones(
                      ui->splitByBoneCountSpinBox->value());

    settings->setTriangulate(ui->triangulateCheckBox->isChecked());

    settings->setValidateDataStructures(
                      ui->validateDataStructuresCheckBox->isChecked());
}

void FlagsWidget::reset()
{
    ui->calculateTangentSpaceCheckBox->setChecked(false);

    ui->calculateTangentSpaceDoubleSpinBox->setValue(repoDefaultCalculateTangentSpaceMaxSmoothingAngle);

    ui->convertToUVCoordinatesCheckBox->setChecked(false);

    ui->degeneratesToPointsLinesCheckBox->setChecked(false);

    ui->deboneGroupBox->setChecked(false);

    ui->deboneThresholdDoubleSpinBox->setValue(repoDefaultDeboneThreshold);

    ui->deboneIfAndOnlyIfCheckBox->setChecked(false);

    ui->findInstancesCheckBox->setChecked(false);

    ui->findInvalidDataGroupBox->setChecked(false);

    ui->findInvalidDataAnimationAccuracyDoubleSpinBox->setValue(repoDefaultFindInvalidDataAnimationAccuracy);

    ui->fixInfacingNormalsCheckBox->setChecked(false);

    ui->flipUVCoordinatesCheckBox->setChecked(false);

    ui->flipWindingOrderCheckBox->setChecked(false);

    ui->generateNormalsGroupBox->setChecked(false);

    ui->generateNormalsFlatRadioButton->setChecked(false);

    ui->generateNormalsSmoothRadioButton->setChecked(false);

    ui->generateNormalsSmoothDoubleSpinBox->setValue(repoDefaultGenerateNormalsSmoothCreaseAngle);

    ui->improveCacheLocalityCheckBox->setChecked(false);

    ui->improveCacheLocalitySpinBox->setValue(repoDefaultImproveCacheLocalityVertexCacheSize);

    ui->joinIdenticalVerticesCheckBox->setChecked(false);

    ui->limitBoneWeightsCheckBox->setChecked(false);

    ui->limitBoneWeightsSpinBox->setValue(repoDefaultLimitBoneWeightsMaxWeight);

    ui->makeLeftHandedCheckBox->setChecked(false);

    ui->optimizeMeshesCheckBox->setChecked(false);

    ui->preTransformUVCoordinatesCheckBox->setChecked(false);

    ui->preTransformVerticesGroupBox->setChecked(false);

    ui->preTransformVerticesNormalizeCheckBox->setChecked(false);

    ui->removeComponentsGroupBox->setChecked(false);

    ui->removeComponentsAnimationsCheckBox->setChecked(false);

    ui->removeComponentsBiTangentsCheckBox->setChecked(false);

    ui->removeComponentsBoneWeightsCheckBox->setChecked(false);

    ui->removeComponentsCamerasCheckBox->setChecked(false);

    ui->removeComponentsColorsCheckBox->setChecked(false);

    ui->removeComponentsLightsCheckBox->setChecked(false);

    ui->removeComponentsMaterialsCheckBox->setChecked(false);

    ui->removeComponentsMeshesCheckBox->setChecked(false);

    ui->removeComponentsNormalsCheckBox->setChecked(false);

    ui->removeComponentsTexturesCheckBox->setChecked(false);

    ui->removeComponentsTextureCoordinatesCheckBox->setChecked(false);

    ui->removeRedundantMaterialsGroupBox->setChecked(false);

    ui->removeRedundantMaterialsSkipLineEdit->setText("");

    ui->removeRedundantNodesGroupBox->setChecked(false);

    ui->removeRedundantNodesSkipLineEdit->setText("");

    ui->sortAndRemoveGroupBox->setChecked(false);

    ui->sortAndRemovePointsCheckBox->setChecked(false);

    ui->sortAndRemoveLinesCheckBox->setChecked(false);

    ui->sortAndRemoveTrianglesCheckBox->setChecked(false);

    ui->sortAndRemovePolygonsCheckBox->setChecked(false);

    ui->splitByBoneCountCheckBox->setChecked(false);

    ui->splitByBoneCountSpinBox->setValue(repoDefaultSplitByBoneCount);

    ui->splitLargeMeshesGroupBox->setChecked(false);

    ui->splitLargeMeshesTriangleLimitSpinBox->setValue(repoDefaultSplitLargeMeshesTriangleLimit);

    ui->splitLargeMeshesVertexLimitSpinBox->setValue(repoDefaultSplitLargeMeshesVertexLimit);

    ui->triangulateCheckBox->setChecked(false);

    ui->validateDataStructuresCheckBox->setChecked(false);
}

void FlagsWidget::setBasic()
{
    ui->calculateTangentSpaceCheckBox->setChecked(true);
    ui->generateNormalsGroupBox->setChecked(true);
    ui->generateNormalsFlatRadioButton->setChecked(true);
    ui->joinIdenticalVerticesCheckBox->setChecked(true);
    ui->triangulateCheckBox->setChecked(true);
    ui->convertToUVCoordinatesCheckBox->setChecked(true);
    ui->sortAndRemoveGroupBox->setChecked(true);
}

void FlagsWidget::setCreaseAngleEnabled(bool on)
{
    ui->generateNormalsSmoothDoubleSpinBox->setEnabled(
                on &&
                ui->generateNormalsSmoothRadioButton->isChecked());
}

void FlagsWidget::setDirect3D()
{
    ui->makeLeftHandedCheckBox->setChecked(true);
    ui->flipUVCoordinatesCheckBox->setChecked(true);
    ui->flipWindingOrderCheckBox->setChecked(true);
}

void FlagsWidget::setExtreme()
{
    setMedium();
    ui->findInstancesCheckBox->setChecked(true);
    ui->validateDataStructuresCheckBox->setChecked(true);
    ui->optimizeMeshesCheckBox->setChecked(true);
    ui->deboneGroupBox->setChecked(true);
}

void FlagsWidget::setMedium()
{
    ui->calculateTangentSpaceCheckBox->setChecked(true);
    ui->generateNormalsGroupBox->setChecked(true);
    ui->generateNormalsSmoothRadioButton->setChecked(true);
    ui->joinIdenticalVerticesCheckBox->setChecked(true);
    ui->improveCacheLocalityCheckBox->setChecked(true);
    ui->limitBoneWeightsCheckBox->setChecked(true);
    ui->removeRedundantMaterialsGroupBox->setChecked(true);
    ui->splitLargeMeshesGroupBox->setChecked(true);
    ui->triangulateCheckBox->setChecked(true);
    ui->convertToUVCoordinatesCheckBox->setChecked(true);
    ui->sortAndRemoveGroupBox->setChecked(true);
    ui->degeneratesToPointsLinesCheckBox->setChecked(true);
    ui->findInvalidDataGroupBox->setChecked(true);
}

void FlagsWidget::setPredefinedSettings(int selection)
{
    switch (selection)
    {
    case DEFAULT :
        reset();
        break;
    case DIRECT_3D :
        reset();
        setDirect3D();
        break;
    case BASIC :
        reset();
        setBasic();
        break;
    case MEDIUM :
        reset();
        setMedium();
        break;
    case EXTREME :
        reset();
        setExtreme();
        break;
    }
}
