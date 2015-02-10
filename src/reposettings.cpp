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

#include "reposettings.h"

//------------------------------------------------------------------------------

const QString repo::gui::RepoSettings::CALCULATE_TANGENT_SPACE = "calculateTangentSpace";
const QString repo::gui::RepoSettings::CONVERT_TO_UV_COORDINATES = "convertToUVCoordinates";
const QString repo::gui::RepoSettings::DEGENERATES_TO_POINTS_LINES = "degeneratesToPoinstLines";
const QString repo::gui::RepoSettings::DEBONE = "debone";
const QString repo::gui::RepoSettings::DEBONE_THRESHOLD = "deboneThreshold";
const QString repo::gui::RepoSettings::DEBONE_ONLY_IF_ALL = "deboneOnlyIfAll";
const QString repo::gui::RepoSettings::FIND_INSTANCES = "findInstances";
const QString repo::gui::RepoSettings::FIND_INAVLID_DATA = "findInvalidData";
const QString repo::gui::RepoSettings::FIND_INAVLID_DATA_ANIMATION_ACCURACY = "findInvalidDataAnimationAccuracy";
const QString repo::gui::RepoSettings::FIX_INFACING_NORMALS = "fixInfacingNormals";
const QString repo::gui::RepoSettings::FLIP_UV_COORDINATES = "flipUVCoordinates";
const QString repo::gui::RepoSettings::FLIP_WINDING_ORDER = "flipWindingOrder";
const QString repo::gui::RepoSettings::GENERATE_NORMALS = "generateNormals";
const QString repo::gui::RepoSettings::GENERATE_NORMALS_FLAT = "generateNormalsFlat";
const QString repo::gui::RepoSettings::GENERATE_NORMALS_SMOOTH = "generateNormalsSmooth";
const QString repo::gui::RepoSettings::GENERATE_NORMALS_SMOOTH_CREASE_ANGLE = "generateNormalsSmoothCreaseAngle";
const QString repo::gui::RepoSettings::IMPROVE_CACHE_LOCALITY = "improveCacheLocality";
const QString repo::gui::RepoSettings::IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE = "improveCacheLocalityVertexCacheSize";
const QString repo::gui::RepoSettings::JOIN_IDENTICAL_VERTICES = "joinIdenticalVertices";
const QString repo::gui::RepoSettings::LIMIT_BONE_WEIGHTS = "limitBoneWeights";
const QString repo::gui::RepoSettings::LIMIT_BONE_WEIGHTS_MAX_WEIGHTS = "limitBoneWeightsMaxWeight";
const QString repo::gui::RepoSettings::MAKE_LEFT_HANDED = "makeLeftHanded";
const QString repo::gui::RepoSettings::OPTIMIZE_MESHES = "optimizeMeshes";
const QString repo::gui::RepoSettings::PRE_TRANSFORM_UV_COORDINATES = "preTransformUVCoordinates";
const QString repo::gui::RepoSettings::PRE_TRANSFORM_VERTICES = "preTransformVertices";
const QString repo::gui::RepoSettings::PRE_TRANSFORM_VERTICES_NORMALIZE = "preTransformVerticesNormalize";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS = "removeComponents";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_ANIMATIONS = "removeComponentsAnimations";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_BI_TANGENTS = "removeComponentsBiTangents";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_BONE_WEIGHTS = "removeComponentsBoneWeights";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_CAMERAS = "removeComponentsCameras";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_COLORS = "removeComponentsColors";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_LIGHTS = "removeComponentsLights";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_MATERIALS = "removeComponentsMaterials";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_MESHES = "removeComponentsMeshes";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_NORMALS = "removeComponentsNormals";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_TEXTURES = "removeComponentsTextures";
const QString repo::gui::RepoSettings::REMOVE_COMPONENTS_TEXTURE_COORDINATES = "removeComponentsTextureCoordinates";
const QString repo::gui::RepoSettings::REMOVE_REDUNDANT_MATERIALS = "removeRedundantMaterials";
const QString repo::gui::RepoSettings::REMOVE_REDUNDANT_MATERIALS_SKIP = "removeRedundantMaterialsSkip";
const QString repo::gui::RepoSettings::REMOVE_REDUNDANT_NODES = "removeRedundantNodes";
const QString repo::gui::RepoSettings::REMOVE_REDUNDANT_NODES_SKIP = "removeRedundantNodesSkip";
const QString repo::gui::RepoSettings::SORT_AND_REMOVE = "sortAndRemove";
const QString repo::gui::RepoSettings::SORT_AND_REMOVE_POINTS = "sortAndRemovePoints";
const QString repo::gui::RepoSettings::SORT_AND_REMOVE_LINES = "sortAndRemoveLines";
const QString repo::gui::RepoSettings::SORT_AND_REMOVE_TRIANGLES = "sortAndRemoveTriangles";
const QString repo::gui::RepoSettings::SORT_AND_REMOVE_POLYGONS = "sortAndRemovePolygons";
const QString repo::gui::RepoSettings::SPLIT_BY_BONE_COUNT = "splitByBoneCount";
const QString repo::gui::RepoSettings::SPLIT_BY_BONE_COUNT_MAX_BONES = "splitByBoneCountMaxBones";
const QString repo::gui::RepoSettings::SPLIT_LARGE_MESHES = "splitLargeMeshes";
const QString repo::gui::RepoSettings::SPLIT_LARGE_MESHES_TRIANGLE_LIMIT = "splitLargeMeshesTriangleLimit";
const QString repo::gui::RepoSettings::SPLIT_LARGE_MESHES_VERTEX_LIMIT = "splitLargeMeshesVertexLimit";
const QString repo::gui::RepoSettings::TRIANGULATE = "RepoWdigetAssimpFlags/triangulate";
const QString repo::gui::RepoSettings::VALIDATE_DATA_STRUCTURES = "validateDataStructures";

unsigned int repo::gui::RepoSettings::getAssimpPostProcessingFlags(
        unsigned int flag) const
{
    if (getCalculateTangentSpace())
        flag |= aiProcess_CalcTangentSpace;

    if (getConvertToUVCoordinates())
        flag |= aiProcess_GenUVCoords;

    if (getDegeneratesToPointsLines())
        flag |= aiProcess_FindDegenerates;

    if (getDebone())
        flag |= aiProcess_Debone;

    // Debone threshold
    // Debone only if all

    if (getFindInstances())
        flag |= aiProcess_FindInstances;

    if (getFindInvalidData())
        flag |= aiProcess_FindInvalidData;

    // Animation accuracy

    if (getFixInfacingNormals())
        flag |= aiProcess_FixInfacingNormals;

    if (getFlipUVCoordinates())
        flag |= aiProcess_FlipUVs;

    if (getFlipWindingOrder())
        flag |= aiProcess_FlipWindingOrder;

    if (getGenerateNormals() && getGenerateNormalsFlat())
        flag |= aiProcess_GenNormals;

    if (getGenerateNormals() && getGenerateNormalsSmooth())
        flag |= aiProcess_GenSmoothNormals;

    // Crease angle

    if (getImproveCacheLocality())
        flag |= aiProcess_ImproveCacheLocality;

    // Vertex cache size

    if (getJoinIdenticalVertices())
        flag |= aiProcess_JoinIdenticalVertices;

    if (getLimitBoneWeights())
        flag |= aiProcess_LimitBoneWeights;

    // Max bone weights

    if (getMakeLeftHanded())
        flag |= aiProcess_MakeLeftHanded;

    if (getOptimizeMeshes())
        flag |= aiProcess_OptimizeMeshes;

    if (getPreTransformUVCoordinates())
        flag |= aiProcess_TransformUVCoords;

    if (getPreTransformVertices())
        flag |= aiProcess_PreTransformVertices;

    // Normalize

    if (getRemoveComponents())
        flag |= aiProcess_RemoveComponent;

    // !individual components!

    if (getRemoveRedundantMaterials())
        flag |= aiProcess_RemoveRedundantMaterials;

    // Skip materials

    if (getRemoveRedundantNodes())
        flag |= aiProcess_OptimizeGraph;

    // Skip nodes

    if (getSortAndRemove())
        flag |= aiProcess_SortByPType;

    // Remove types

    if (getSplitByBoneCount())
        flag |= aiProcess_SplitByBoneCount;

    // Max bones

    if (getSplitLargeMeshes())
        flag |= aiProcess_SplitLargeMeshes;

    // Vertex limit
    // Triangle limit

    if (getTriangulate())
        flag |= aiProcess_Triangulate;

    if (getValidateDataStructures())
        flag |= aiProcess_ValidateDataStructure;

    return flag;
}
