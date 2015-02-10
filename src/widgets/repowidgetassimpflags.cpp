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

//------------------------------------------------------------------------------

repo::gui::RepoWidgetAssimpFlags::RepoWidgetAssimpFlags(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepoWidgetAssimpFlags)
{
    ui->setupUi(this);

    //--------------------------------------------------------------------------

    ui->calculateTangentSpaceCheckBox->setChecked(settings.getCalculateTangentSpace());

    ui->convertToUVCoordinatesCheckBox->setChecked(settings.getConvertToUVCoordinates());

    ui->degeneratesToPointsLinesCheckBox->setChecked(settings.getDegeneratesToPointsLines());

    ui->deboneGroupBox->setChecked(settings.getDebone());

    ui->deboneThresholdDoubleSpinBox->setValue(settings.getDeboneThreshold());

    ui->deboneIfAndOnlyIfCheckBox->setChecked(settings.getDeboneOnlyIfAll());

    ui->findInstancesCheckBox->setChecked(settings.getFindInstances());

    ui->findInvalidDataGroupBox->setChecked(settings.getFindInvalidData());

    ui->findInvalidDataAnimationAccuracyDoubleSpinBox->setValue(settings.getFindInvalidDataAnimationAccuracy());

    ui->fixInfacingNormalsCheckBox->setChecked(settings.getFixInfacingNormals());

    ui->flipUVCoordinatesCheckBox->setChecked(settings.getFlipUVCoordinates());

    ui->flipWindingOrderCheckBox->setChecked(settings.getFlipWindingOrder());

    ui->generateNormalsGroupBox->setChecked(settings.getGenerateNormals());

    ui->generateNormalsFlatRadioButton->setChecked(settings.getGenerateNormalsFlat());

    ui->generateNormalsSmoothRadioButton->setChecked(settings.getGenerateNormalsSmooth());

    ui->generateNormalsSmoothDoubleSpinBox->setValue(settings.getGenerateNormalsSmoothCreaseAngle());

    ui->improveCacheLocalityCheckBox->setChecked(settings.getImproveCacheLocality());

    ui->improveCacheLocalitySpinBox->setValue(settings.getImproveCacheLocalityVertexCacheSize());

    ui->joinIdenticalVerticesCheckBox->setChecked(settings.getJoinIdenticalVertices());

    ui->limitBoneWeightsCheckBox->setChecked(settings.getLimitBoneWeights());

    ui->limitBoneWeightsSpinBox->setValue(settings.getLimitBoneWeightsMaxWeight());

    ui->makeLeftHandedCheckBox->setChecked(settings.getMakeLeftHanded());

    ui->optimizeMeshesCheckBox->setChecked(settings.getOptimizeMeshes());

    ui->preTransformUVCoordinatesCheckBox->setChecked(settings.getPreTransformUVCoordinates());

    ui->preTransformVerticesGroupBox->setChecked(settings.getPreTransformVertices());

    ui->preTransformVerticesNormalizeCheckBox->setChecked(settings.getPreTransformVerticesNormalize());

    ui->removeComponentsGroupBox->setChecked(settings.getRemoveComponents());

    ui->removeComponentsAnimationsCheckBox->setChecked(settings.getRemoveComponentsAnimations());

    ui->removeComponentsBiTangentsCheckBox->setChecked(settings.getRemoveComponentsBiTangents());

    ui->removeComponentsBoneWeightsCheckBox->setChecked(settings.getRemoveComponentsBoneWeights());

    ui->removeComponentsCamerasCheckBox->setChecked(settings.getRemoveComponentsCameras());

    ui->removeComponentsColorsCheckBox->setChecked(settings.getRemoveComponentsColors());

    ui->removeComponentsLightsCheckBox->setChecked(settings.getRemoveComponentsLights());

    ui->removeComponentsMaterialsCheckBox->setChecked(settings.getRemoveComponentsMaterials());

    ui->removeComponentsMeshesCheckBox->setChecked(settings.getRemoveComponentsMeshes());

    ui->removeComponentsNormalsCheckBox->setChecked(settings.getRemoveComponentsNormals());

    ui->removeComponentsTexturesCheckBox->setChecked(settings.getRemoveComponentsTextures());

    ui->removeComponentsTextureCoordinatesCheckBox->setChecked(settings.getRemoveComponentsTextureCoordinates());

    ui->removeRedundantMaterialsGroupBox->setChecked(settings.getRemoveRedundantMaterials());

    ui->removeRedundantMaterialsSkipLineEdit->setText(settings.getRemoveRedundantMaterialsSkip());

    ui->removeRedundantNodesGroupBox->setChecked(settings.getRemoveRedundantNodes());

    ui->removeRedundantNodesSkipLineEdit->setText(settings.getRemoveRedundantNodesSkip());

    ui->sortAndRemoveGroupBox->setChecked(settings.getSortAndRemove());

    ui->sortAndRemovePointsCheckBox->setChecked(settings.getSortAndRemovePoints());

    ui->sortAndRemoveLinesCheckBox->setChecked(settings.getSortAndRemoveLines());

    ui->sortAndRemoveTrianglesCheckBox->setChecked(settings.getSortAndRemoveTriangles());

    ui->sortAndRemovePolygonsCheckBox->setChecked(settings.getSortAndRemovePolygons());

    ui->splitByBoneCountCheckBox->setChecked(settings.getSplitByBoneCount());

    ui->splitByBoneCountSpinBox->setValue(settings.getSplitByBoneCountMaxBones());

    ui->splitLargeMeshesGroupBox->setChecked(settings.getSplitLargeMeshes());

    ui->splitLargeMeshesTriangleLimitSpinBox->setValue(settings.getSplitLargeMeshesTriangleLimit());

    ui->splitLargeMeshesVertexLimitSpinBox->setValue(settings.getSplitLargeMeshesVertexLimit());

    ui->triangulateCheckBox->setChecked(settings.getTriangulate());

    ui->validateDataStructuresCheckBox->setChecked(settings.getValidateDataStructures());

    //--------------------------------------------------------------------------

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
}

repo::gui::RepoWidgetAssimpFlags::~RepoWidgetAssimpFlags()
{
    delete ui;
}

void repo::gui::RepoWidgetAssimpFlags::apply()
{
    settings.setCalculateTangentSpace(ui->calculateTangentSpaceCheckBox->isChecked());

    settings.setConvertToUVCoordinates(ui->convertToUVCoordinatesCheckBox->isChecked());

    settings.setDegeneratesToPointsLines(ui->degeneratesToPointsLinesCheckBox->isChecked());

    settings.setDebone(ui->deboneGroupBox->isChecked());

    settings.setDeboneThreshold(ui->deboneThresholdDoubleSpinBox->value());

    settings.setDeboneOnlyIfAll(ui->deboneIfAndOnlyIfCheckBox->isChecked());

    settings.setFindInstances(ui->findInstancesCheckBox->isChecked());

    settings.setFindInvalidData(ui->findInvalidDataGroupBox->isChecked());

    settings.setFindInvalidDataAnimationAccuracy(
                      ui->findInvalidDataAnimationAccuracyDoubleSpinBox->value());

    settings.setFixInfacingNormals(ui->fixInfacingNormalsCheckBox->isChecked());

    settings.setFlipUVCoordinates(ui->flipUVCoordinatesCheckBox->isChecked());

    settings.setFlipWindingOrder(ui->flipWindingOrderCheckBox->isChecked());

    settings.setGenerateNormals(ui->generateNormalsGroupBox->isChecked());

    settings.setGenerateNormalsFlat(ui->generateNormalsFlatRadioButton->isChecked());

    settings.setGenerateNormalsSmooth(ui->generateNormalsSmoothRadioButton->isChecked());

    settings.setGenerateNormalsSmoothCreaseAngle(
                      ui->generateNormalsSmoothDoubleSpinBox->value());

    settings.setImproveCacheLocality(ui->improveCacheLocalityCheckBox->isChecked());

    settings.setImproveCacheLocalityCacheSize(
                      ui->improveCacheLocalitySpinBox->value());

    settings.setJoinIdenticalVertices(ui->joinIdenticalVerticesCheckBox->isChecked());

    settings.setLimitBoneWeights(
                      ui->limitBoneWeightsCheckBox->isChecked());

    settings.setLimitBoneWeightsMaxWeights(
                      ui->limitBoneWeightsSpinBox->value());

    settings.setMakeLeftHanded(
                      ui->makeLeftHandedCheckBox->isChecked());

    settings.setOptimizeMeshes(
                      ui->optimizeMeshesCheckBox->isChecked());

    settings.setPreTransformUVCoordinates(
                      ui->preTransformUVCoordinatesCheckBox->isChecked());

    settings.setPreTransformVertices(
                      ui->preTransformVerticesGroupBox->isChecked());

    settings.setPreTransforVerticesNormalize(
                      ui->preTransformVerticesNormalizeCheckBox->isChecked());

    settings.setRemoveComponents(
                      ui->removeComponentsGroupBox->isChecked());

    settings.setRemoveComponentsAnimations(
                      ui->removeComponentsAnimationsCheckBox->isChecked());

    settings.setRemoveComponentsBiTangents(
                      ui->removeComponentsBiTangentsCheckBox->isChecked());

    settings.setRemoveComponentsBoneWeights(
                      ui->removeComponentsBoneWeightsCheckBox->isChecked());

    settings.setRemoveComponentsCameras(
                      ui->removeComponentsCamerasCheckBox->isChecked());

    settings.setRemoveComponentsColors(
                      ui->removeComponentsColorsCheckBox->isChecked());

    settings.setRemoveComponentsLights(
                      ui->removeComponentsLightsCheckBox->isChecked());

    settings.setRemoveComponentsMaterials(
                      ui->removeComponentsMaterialsCheckBox->isChecked());

    settings.setRemoveComponentsMeshes(
                      ui->removeComponentsMeshesCheckBox->isChecked());

    settings.setRemoveComponentsNormals(
                      ui->removeComponentsNormalsCheckBox->isChecked());

    settings.setRemoveComponentsTextureCoordinates(
                      ui->removeComponentsTextureCoordinatesCheckBox->isChecked());

    settings.setRemoveComponentsTextures(
                      ui->removeComponentsTexturesCheckBox->isChecked());

    settings.setRemoveRedundantMaterials(
                      ui->removeRedundantMaterialsGroupBox->isChecked());

    settings.setRemoveRedundantMaterialsSkip(
                      ui->removeRedundantMaterialsSkipLineEdit->text());

    settings.setRemoveRedundantNodes(
                      ui->removeRedundantNodesGroupBox->isChecked());

    settings.setRemoveRedundantNodesSkip(
                      ui->removeRedundantNodesSkipLineEdit->text());

    settings.setSortAndRemove(
                      ui->sortAndRemoveGroupBox->isChecked());

    settings.setSortAndRemoveLines(
                      ui->sortAndRemoveLinesCheckBox->isChecked());

    settings.setSortAndRemovePoints(
                      ui->sortAndRemovePointsCheckBox->isChecked());

    settings.setSortAndRemovePolygons(
                      ui->sortAndRemovePolygonsCheckBox->isChecked());

    settings.setSortAndRemoveTriangles(
                      ui->sortAndRemoveTrianglesCheckBox->isChecked());

    settings.setSplitLargeMeshes(
                      ui->splitLargeMeshesGroupBox->isChecked());

    settings.setSplitLargeMeshesTriangleLimit(
                       ui->splitLargeMeshesTriangleLimitSpinBox->value());

    settings.setSplitLargeMeshesVertexLimit(
                       ui->splitLargeMeshesVertexLimitSpinBox->value());

    settings.setSplitByBoneCount(
                      ui->splitByBoneCountCheckBox->isChecked());

    settings.setSplitByBoneCountMaxBones(
                      ui->splitByBoneCountSpinBox->value());

    settings.setTriangulate(ui->triangulateCheckBox->isChecked());

    settings.setValidateDataStructures(
                      ui->validateDataStructuresCheckBox->isChecked());
}

void repo::gui::RepoWidgetAssimpFlags::setCreaseAngleEnabled(bool on)
{
    ui->generateNormalsSmoothDoubleSpinBox->setEnabled(
                on &&
                ui->generateNormalsSmoothRadioButton->isChecked());
}

