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
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_GENERATE_NORMALS_SMOOTH_CREASE_ANGLE = "RepoWidgetAssimpFlags/generateNormalsSmoothCreaseAngle";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_IMPROVE_CACHE_LOCALITY = "RepoWidgetAssimpFlags/improveCacheLocality";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE = "RepoWidgetAssimpFlags/improveCacheLocalityVertexCacheSize";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_JOIN_IDENTICAL_VERTICES = "RepoWidgetAssimpFlags/joinIdenticalVertices";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_LIMIT_BONE_WEIGHTS = "RepoWidgetAssimpFlags/limitBoneWeights";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_LIMIT_BONE_WEIGHTS_MAX_WEIGHTS = "RepoWidgetAssimpFlags/limitBoneWeightsMaxWeight";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_MAKE_LEFT_HANDED = "RepoWidgetAssimpFlags/makeLeftHanded";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_OPTIMIZE_MESHES = "RepoWidgetAssimpFlags/optimizeMeshes";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_PRE_TRANSFORM_UV_COORDINATES = "RepoWidgetAssimpFlags/preTransformUVCoordinates";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_PRE_TRANSFORM_VERTICES = "RepoWidgetAssimpFlags/preTransformVertices";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_PRE_TRANSFORM_VERTICES_NORMALIZE = "RepoWidgetAssimpFlags/preTransformVerticesNormalize";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS = "RepoWidgetAssimpFlags/removeComponents";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_ANIMATIONS = "RepoWidgetAssimpFlags/removeComponentsAnimations";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_BI_TANGENTS = "RepoWidgetAssimpFlags/removeComponentsBiTangents";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_BONE_WEIGHTS = "RepoWidgetAssimpFlags/removeComponentsBoneWeights";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_CAMERAS = "RepoWidgetAssimpFlags/removeComponentsCameras";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_COLORS = "RepoWidgetAssimpFlags/removeComponentsColors";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_LIGHTS = "RepoWidgetAssimpFlags/removeComponentsLights";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_MATERIALS = "RepoWidgetAssimpFlags/removeComponentsMaterials";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_MESHES = "RepoWidgetAssimpFlags/removeComponentsMeshes";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_NORMALS = "RepoWidgetAssimpFlags/removeComponentsNormals";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURES = "RepoWidgetAssimpFlags/removeComponentsTextures";
const QString repo::gui::RepoWidgetAssimpFlags::REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURE_COORDINATES = "RepoWidgetAssimpFlags/removeComponentsTextureCoordinates";

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

    QObject::connect(ui->generateNormalsGroupBox, SIGNAL(toggled(bool)),
                     this, SLOT(setCreaseAngleEnabled(bool)));

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

    ui->generateNormalsSmoothDoubleSpinBox->setValue(getGenerateNormalsSmoothCreaseAngle());

    ui->improveCacheLocalityCheckBox->setChecked(isImproveCacheLocalityChecked());

    ui->improveCacheLocalitySpinBox->setValue(getImproveCacheLocalityVertexCacheSize());

    ui->joinIdenticalVerticesCheckBox->setChecked(isJoinIdenticalVerticesChecked());

    ui->limitBoneWeightsCheckBox->setChecked(isLimitBoneWeightsChecked());

    ui->limitBoneWeightsSpinBox->setValue(getLimitBoneWeightsMaxWeight());

    ui->makeLeftHandedCheckBox->setChecked(isMakeLeftHandedChecked());

    ui->optimizeMeshesCheckBox->setChecked(isOptimizeMeshesChecked());

    ui->preTransformUVCoordinatesCheckBox->setChecked(isPreTransformUVCoordinatesChecked());

    ui->preTransformVerticesGroupBox->setChecked(isPreTransformVerticesChecked());

    ui->preTransformVerticesNormalizeCheckBox->setChecked(isPreTransformVerticesNormalizeChecked());

    ui->removeComponentsGroupBox->setChecked(isRemoveComponentsChecked());

    ui->removeComponentsAnimationsCheckBox->setChecked(isRemoveComponentsAnimationsChecked());
    ui->removeComponentsBiTangentsCheckBox->setChecked(isRemoveComponentsBiTangentsChecked());
    ui->removeComponentsBoneWeightsCheckBox->setChecked(isRemoveComponentsBoneWeightsChecked());
    ui->removeComponentsCamerasCheckBox->setChecked(isRemoveComponentsCamerasChecked());
    ui->removeComponentsColorsCheckBox->setChecked(isRemoveComponentsColorsChecked());
    ui->removeComponentsLightsCheckBox->setChecked(isRemoveComponentsLightsChecked());
    ui->removeComponentsMaterialsCheckBox->setChecked(isRemoveComponentsMaterialsChecked());
    ui->removeComponentsMeshesCheckBox->setChecked(isRemoveComponentsMeshesChecked());
    ui->removeComponentsNormalsCheckBox->setChecked(isRemoveComponentsNormalsChecked());
    ui->removeComponentsTexturesCheckBox->setChecked(isRemoveComponentsTexturesChecked());
    ui->removeComponentsTextureCoordinatesCheckBox->setChecked(isRemoveComponentsTextureCoordinatesChecked());
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

    settings.setValue(REPO_SETTINGS_GENERATE_NORMALS_SMOOTH_CREASE_ANGLE,
                      ui->generateNormalsSmoothDoubleSpinBox->value());

    settings.setValue(REPO_SETTINGS_IMPROVE_CACHE_LOCALITY,
                      ui->improveCacheLocalityCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE,
                      ui->improveCacheLocalitySpinBox->value());

    settings.setValue(REPO_SETTINGS_JOIN_IDENTICAL_VERTICES,
                      ui->joinIdenticalVerticesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_LIMIT_BONE_WEIGHTS,
                      ui->limitBoneWeightsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_LIMIT_BONE_WEIGHTS_MAX_WEIGHTS,
                      ui->limitBoneWeightsSpinBox->value());

    settings.setValue(REPO_SETTINGS_MAKE_LEFT_HANDED,
                      ui->makeLeftHandedCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_OPTIMIZE_MESHES,
                      ui->optimizeMeshesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_PRE_TRANSFORM_UV_COORDINATES,
                      ui->preTransformUVCoordinatesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_PRE_TRANSFORM_VERTICES,
                      ui->preTransformVerticesGroupBox->isChecked());

    settings.setValue(REPO_SETTINGS_PRE_TRANSFORM_VERTICES_NORMALIZE,
                      ui->preTransformVerticesNormalizeCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS,
                      ui->removeComponentsGroupBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_ANIMATIONS,
                      ui->removeComponentsAnimationsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_BI_TANGENTS,
                      ui->removeComponentsBiTangentsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_BONE_WEIGHTS,
                      ui->removeComponentsBoneWeightsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_CAMERAS,
                      ui->removeComponentsCamerasCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_COLORS,
                      ui->removeComponentsColorsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_LIGHTS,
                      ui->removeComponentsLightsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_MATERIALS,
                      ui->removeComponentsMaterialsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_MESHES,
                      ui->removeComponentsMeshesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_NORMALS,
                      ui->removeComponentsNormalsCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURES,
                      ui->removeComponentsTexturesCheckBox->isChecked());

    settings.setValue(REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURE_COORDINATES,
                      ui->removeComponentsTextureCoordinatesCheckBox->isChecked());
}

void repo::gui::RepoWidgetAssimpFlags::setCreaseAngleEnabled(bool on)
{
    ui->generateNormalsSmoothDoubleSpinBox->setEnabled(
                on &&
                ui->generateNormalsSmoothRadioButton->isChecked());
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

    // Crease angle

    if (isImproveCacheLocalityChecked())
        flag |= aiProcess_ImproveCacheLocality;

    // Vertex cache size

    if (isJoinIdenticalVerticesChecked())
        flag |= aiProcess_JoinIdenticalVertices;

    if (isLimitBoneWeightsChecked())
        flag |= aiProcess_LimitBoneWeights;

    // Max bone weights

    if (isMakeLeftHandedChecked())
        flag |= aiProcess_MakeLeftHanded;

    if (isOptimizeMeshesChecked())
        flag |= aiProcess_OptimizeMeshes;

    if (isPreTransformUVCoordinatesChecked())
        flag |= aiProcess_TransformUVCoords;

    if (isPreTransformVerticesChecked())
        flag |= aiProcess_PreTransformVertices;

    // Normalize

    if (isRemoveComponentsChecked())
        flag |= aiProcess_RemoveComponent;

    // !individual components!


//    if (actionTriangulate->isChecked())
//        flag |= aiProcess_Triangulate;
//    if (actionGenerate_Normals->isChecked())
//        flag |= aiProcess_GenNormals;
//    if (actionGenerate_Smooth_Normals->isChecked())
//        flag |= aiProcess_GenSmoothNormals;
//    if (actionSplit_Large_Meshes->isChecked())
//        flag |= aiProcess_SplitLargeMeshes;
//    if (actionLimit_Bone_Weights->isChecked())
//        flag |= aiProcess_LimitBoneWeights;
//    if (actionValidate_Data_Structure->isChecked())
//        flag |= aiProcess_ValidateDataStructure;
//    if (actionRemove_Redundant_Materials->isChecked())
//        flag |= aiProcess_RemoveRedundantMaterials;
//    if (actionSort_by_P_Type->isChecked())
//        flag |= aiProcess_SortByPType;
//    if (actionFind_Degenerates->isChecked())
//        flag |= aiProcess_FindDegenerates;
//    if (actionOptimize_Graph->isChecked())
//        flag |= aiProcess_OptimizeGraph;
//    if (actionSplit_by_Bone_Count->isChecked())
//        flag |= aiProcess_SplitByBoneCount;

    return flag;
}
