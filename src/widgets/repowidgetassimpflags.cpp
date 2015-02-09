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




const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_CALCULATE_TANGENT_SPACE = "RepoWidgetAssimpFlags/calculateTangentSpace";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_CONVERT_TO_UV_COORDINATES = "RepoWidgetAssimpFlags/convertToUVCoordinates";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_DEGENERATES_TO_POINTS_LINES = "RepoWidgetAssimpFlags/degeneratesToPoinstLines";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_DEBONE = "RepoWidgetAssimpFlags/debone";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_DEBONE_THRESHOLD = "RepoWidgetAssimpFlags/deboneThreshold";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_DEBONE_ONLY_IF_ALL = "RepoWidgetAssimpFlags/deboneOnlyIfAll";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_FIND_INSTANCES = "RepoWidgetAssimpFlags/findInstances";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_FIND_INAVLID_DATA = "RepoWidgetAssimpFlags/findInvalidData";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_FIND_INAVLID_DATA_ANIMATION_ACCURACY = "RepoWidgetAssimpFlags/findInvalidDataAnimationAccuracy";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_FIX_INFACING_NORMALS = "RepoWidgetAssimpFlags/fixInfacingNormals";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_FLIP_UV_COORDINATES = "RepoWidgetAssimpFlags/flipUVCoordinates";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_FLIP_WINDING_ORDER = "RepoWidgetAssimpFlags/flipWindingOrder";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_GENERATE_NORMALS = "RepoWidgetAssimpFlags/generateNormals";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_GENERATE_NORMALS_FLAT = "RepoWidgetAssimpFlags/generateNormalsFlat";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_GENERATE_NORMALS_SMOOTH = "RepoWidgetAssimpFlags/generateNormalsSmooth";

repo::gui::RepoWidgetAssimpFlags::RepoWidgetAssimpFlags(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RepoWidgetAssimpFlags)
{
    ui->setupUi(this);

    //--------------------------------------------------------------------------

    QObject::connect(ui->improveCacheLocalityCheckBox, SIGNAL(toggled(bool)),
                     ui->improveCacheLocalitySpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->splitByBoneCountCheckBox, SIGNAL(toggled(bool)),
                     ui->splitByBoneCountSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->limitBoneWeightsCheckBox, SIGNAL(toggled(bool)),
                     ui->limitBoneWeightsSpinBox, SLOT(setEnabled(bool)));

    QObject::connect(ui->generateNormalsSmoothRadioButton, SIGNAL(toggled(bool)),
                     ui->generateNormalsSmoothDoubleSpinBox, SLOT(setEnabled(bool)));

    //--------------------------------------------------------------------------

    ui->calculateTangentSpaceCheckBox->setChecked(isCalculateTangentSpaceChecked());

    ui->convertToUVCoordinatesCheckBox->setChecked(isConvertToUVCoordinatesChecked());

    ui->degeneratesToPointsLinesCheckBox->setChecked(isDegeneratesToPointsLinesChecked());

    ui->deboneGroupBox->setChecked(isDeboneChecked());

    ui->deboneThresholdDoubleSpinBox->setValue(getDeboneThreshold());

    ui->deboneIfAndOnlyIfCheckBox->setChecked(isDeboneOnlyIfAllChecked());

    ui->findInstancesCheckBox->setChecked(isFindInstancesChecked());

    ui->findInvalidDataGroupBox->setChecked(isFindInvalidDataChecked());

    ui->findInvalidDataAnimationAccuracyDoubleSpinBox->setValue(getFindInvalidDataAnimationAccuracy());

    ui->fixInfacingNormalsCheckBox->setChecked(isFixInfacingNormalsChecked());

    ui->flipUVCoordinatesCheckBox->setChecked(isFlipUVCoordinatesChecked());

    ui->flipWindingOrderCheckBox->setChecked(isFlipWindingOrderChecked());

    ui->generateNormalsGroupBox->setChecked(isGenerateNormalsChecked());

    ui->generateNormalsFlatRadioButton->setChecked(isGenerateNormalsFlatChecked());

    ui->generateNormalsSmoothRadioButton->setChecked(isGenerateNormalsSmoothChecked());
}

repo::gui::RepoWidgetAssimpFlags::~RepoWidgetAssimpFlags()
{
    delete ui;
}

void repo::gui::RepoWidgetAssimpFlags::apply()
{
    settings.setValue(REPO_SETTINGS_CALCULATE_TANGENT_SPACE,
                      ui->calculateTangentSpaceCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_CONVERT_TO_UV_COORDINATES,
                      ui->convertToUVCoordinatesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_DEGENERATES_TO_POINTS_LINES,
                      ui->degeneratesToPointsLinesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_DEBONE,
                      ui->deboneGroupBox->isChecked());

    settings.setValue(REPO_SETTINGS_DEBONE_THRESHOLD,
                      ui->deboneThresholdDoubleSpinBox->value());

    settings.setValue(REPO_SETTINGS_DEBONE_ONLY_IF_ALL,
                      ui->deboneIfAndOnlyIfCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_FIND_INSTANCES,
                      ui->findInstancesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_FIND_INAVLID_DATA,
                      ui->findInvalidDataGroupBox->isChecked());

    settings.setValue(REPO_SETTINGS_FIND_INAVLID_DATA_ANIMATION_ACCURACY,
                      ui->findInvalidDataAnimationAccuracyDoubleSpinBox->value());

    settings.setValue(REPO_SETTINGS_FIX_INFACING_NORMALS,
                      ui->fixInfacingNormalsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_FLIP_UV_COORDINATES,
                      ui->flipUVCoordinatesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_FLIP_WINDING_ORDER,
                      ui->flipWindingOrderCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_GENERATE_NORMALS,
                      ui->generateNormalsGroupBox->isChecked());

    settings.setValue(REPO_SETTINGS_GENERATE_NORMALS_FLAT,
                      ui->generateNormalsFlatRadioButton->isChecked());

    settings.setValue(REPO_SETTINGS_GENERATE_NORMALS_SMOOTH,
                      ui->generateNormalsSmoothRadioButton->isChecked());

  //  settings.setValue(REPO_SETTINGS_GENERATE_NORMALS_SMOOTH_CREASE_ANGLE)
}


unsigned int repo::gui::RepoWidgetAssimpFlags::getPostProcessingFlags(
        unsigned int flag) const
{
    if (isCalculateTangentSpaceChecked())
        flag |= aiProcess_CalcTangentSpace;

    if (isConvertToUVCoordinatesChecked())
        flag |= aiProcess_GenUVCoords;

    if (isDegeneratesToPointsLinesChecked())
        flag |= aiProcess_FindDegenerates;

    if (isDeboneChecked())
        flag |= aiProcess_Debone;

    // Debone threshold
    // Debone only if all

    if (isFindInstancesChecked())
        flag |= aiProcess_FindInstances;

    if (isFindInvalidDataChecked())
        flag |= aiProcess_FindInvalidData;

    // Animation accuracy

    if (isFixInfacingNormalsChecked())
        flag |= aiProcess_FixInfacingNormals;

    if (isFlipUVCoordinatesChecked())
        flag |= aiProcess_FlipUVs;

    if (isFlipWindingOrderChecked())
        flag |= aiProcess_FlipWindingOrder;

    if (isGenerateNormalsChecked() && isGenerateNormalsFlatChecked())
        flag |= aiProcess_GenNormals;

    if (isGenerateNormalsChecked() && isGenerateNormalsSmoothChecked())
        flag |= aiProcess_GenSmoothNormals;


//    if (actionJoin_Identical_Vertices->isChecked())
//        flag |= aiProcess_JoinIdenticalVertices;
//    if (actionMake_Left_Handed->isChecked())
//        flag |= aiProcess_MakeLeftHanded;
//    if (actionTriangulate->isChecked())
//        flag |= aiProcess_Triangulate;
//    if (actionRemove_Component->isChecked())
//        flag |= aiProcess_RemoveComponent;
//    if (actionGenerate_Normals->isChecked())
//        flag |= aiProcess_GenNormals;
//    if (actionGenerate_Smooth_Normals->isChecked())
//        flag |= aiProcess_GenSmoothNormals;
//    if (actionSplit_Large_Meshes->isChecked())
//        flag |= aiProcess_SplitLargeMeshes;
//    if (actionPre_transform_Vertices->isChecked())
//        flag |= aiProcess_PreTransformVertices;
//    if (actionLimit_Bone_Weights->isChecked())
//        flag |= aiProcess_LimitBoneWeights;
//    if (actionValidate_Data_Structure->isChecked())
//        flag |= aiProcess_ValidateDataStructure;
//    if (actionImprove_Cache_Locality->isChecked())
//        flag |= aiProcess_ImproveCacheLocality;
//    if (actionRemove_Redundant_Materials->isChecked())
//        flag |= aiProcess_RemoveRedundantMaterials;
//    if (actionSort_by_P_Type->isChecked())
//        flag |= aiProcess_SortByPType;
//    if (actionFind_Degenerates->isChecked())
//        flag |= aiProcess_FindDegenerates;

//    if (actionTransform_UV_Coordinates->isChecked())
//        flag |= aiProcess_TransformUVCoords;
//    if (actionFind_Instances->isChecked())
//        flag |= aiProcess_OptimizeMeshes;
//    if (actionOptimize_Graph->isChecked())
//        flag |= aiProcess_OptimizeGraph;
//    if (actionSplit_by_Bone_Count->isChecked())
//        flag |= aiProcess_SplitByBoneCount;

    return flag;
}
