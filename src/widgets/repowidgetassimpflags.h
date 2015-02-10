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


#ifndef REPO_WIDGET_ASSIMP_FLAGS_H
#define REPO_WIDGET_ASSIMP_FLAGS_H

//------------------------------------------------------------------------------
// Qt
#include <QWidget>
#include <QSettings>

//------------------------------------------------------------------------------
// Core
#include <RepoWrapperAssimp>
#include <assimp\postprocess.h>

namespace Ui {
class RepoWidgetAssimpFlags;
}

namespace repo {
namespace gui {

//! See http://assimp.sourceforge.net/lib_html/postprocess_8h.html
class RepoWidgetAssimpFlags : public QWidget
{
    Q_OBJECT

public:

    explicit RepoWidgetAssimpFlags(QWidget *parent = 0);

    ~RepoWidgetAssimpFlags();

public slots :

    //! Saves the current state of the flags into settings.
    void apply();

    //! Sets the crease angle enabled or disabled based on the radio button.
    void setCreaseAngleEnabled(bool on);

public :

    /*!
     * Returns the debone threshold set in settings. Defaults to Assimp's
     * AI_DEBONE_THRESHOLD = 1.0f
     */
    float getDeboneThreshold() const
    { return settings.value(REPO_SETTINGS_DEBONE_THRESHOLD,
                            AI_DEBONE_THRESHOLD).toFloat(); }

    //! Returns a bit-wise or of currently set Assimp post-processing flags.
    unsigned int getPostProcessingFlags(unsigned int flag = 0) const;

    /*!
     * Returns the animation accuracy for find invalid data set in settings.
     * Defaults to 0.0f == all comparisons are exact.
     * See http://assimp.sourceforge.net/lib_html/config_8h.html#ad223c5e7e63d2937685cc704a181b950
     */
    float getFindInvalidDataAnimationAccuracy() const
    { return settings.value(REPO_SETTINGS_FIND_INAVLID_DATA_ANIMATION_ACCURACY,
                            0).toFloat(); }

    /*!
     * Returns the crease angle for generate smooth normals set in settings.
     * Defaults to 175.0f.
     */
    float getGenerateNormalsSmoothCreaseAngle() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS_SMOOTH_CREASE_ANGLE,
                            175.0).toFloat(); }

    /*!
     * Returns the vertex cache size for improve cache locality set in settings.
     * Defaults to PP_ICL_PTCACHE_SIZE = 12.
     */
    int getImproveCacheLocalityVertexCacheSize() const
    { return settings.value(REPO_SETTINGS_IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE,
                            PP_ICL_PTCACHE_SIZE).toInt(); }

    /*!
     * Returns the max weight for limit bone weights set in settings. Defaults
     * to AI_LMW_MAX_WEIGHTS = 4.
     */
    int getLimitBoneWeightsMaxWeight() const
    { return settings.value(REPO_SETTINGS_LIMIT_BONE_WEIGHTS_MAX_WEIGHTS,
                    AI_LMW_MAX_WEIGHTS).toInt(); }

    /*!
     * Returns a space-separated string of materials names to be skipped set in
     * settings. Defaults to empty string.
     */
    QString getRemoveRedundantMaterialsSkip() const
    { return settings.value(REPO_SETTINGS_REMOVE_REDUNDANT_MATERIALS_SKIP,
                           "").toString(); }

    /*!
     * Returns a space-separated string of node names to be skipped set in
     * settings. Defaults to empty string.
     */
    QString getRemoveRedundantNodesSkip() const
    { return settings.value(REPO_SETTINGS_REMOVE_REDUNDANT_NODES_SKIP,
                            "").toString(); }

    /*!
     * Returns a split by bones count max bones set in settings. Defaults to
     * AI_SBBC_DEFAULT_MAX_BONES = 60.
     */
    int getSplitByBoneCountMaxBones() const
    { return settings.value(REPO_SETTINGS_SPLIT_BY_BONE_COUNT_MAX_BONES,
                            AI_SBBC_DEFAULT_MAX_BONES).toInt(); }

    /*!
     * Returns a split large meshes triangle limit set in settings. Defaults to
     * AI_SLM_DEFAULT_MAX_TRIANGLES = 1,000,000.
     */
    int getSplitLargeMeshesTriangleLimit() const
    { return settings.value(REPO_SETTINGS_SPLIT_LARGE_MESHES_TRIANGLE_LIMIT,
                            AI_SLM_DEFAULT_MAX_TRIANGLES).toInt(); }

    /*!
     * Returns a split large meshes triangle limit set in settings. Defaults to
     * AI_SLM_DEFAULT_MAX_VERTICES = 1,000,000.
     */
    int getSplitLargeMeshesVertexLimit() const
    { return settings.value(REPO_SETTINGS_SPLIT_LARGE_MESHES_VERTEX_LIMIT,
                            AI_SLM_DEFAULT_MAX_VERTICES).toInt(); }


    //--------------------------------------------------------------------------

    /*!
     * Returns true if calculate tangent space is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isCalculateTangentSpaceChecked() const
    { return settings.value(REPO_SETTINGS_CALCULATE_TANGENT_SPACE,
                            false).toBool();}

    /*!
     * Returns true if convert to UV coords is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isConvertToUVCoordinatesChecked() const
    { return settings.value(REPO_SETTINGS_CONVERT_TO_UV_COORDINATES,
                            false).toBool(); }

    /*!
     * Returns true if degenerates to points/lines is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isDegeneratesToPointsLinesChecked() const
    { return  settings.value(REPO_SETTINGS_DEGENERATES_TO_POINTS_LINES,
                             false).toBool(); }

    /*!
     * Returns true if debone is checked in settings, false otherwise. Defaults
     * to false.
     */
    bool isDeboneChecked() const
    { return settings.value(REPO_SETTINGS_DEBONE, false).toBool(); }

    /*!
     * Returns true if debone only if all is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isDeboneOnlyIfAllChecked() const
    { return settings.value(REPO_SETTINGS_DEBONE_ONLY_IF_ALL, false).toBool(); }

    /*!
     * Returns true if find instances is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isFindInstancesChecked() const
    { return settings.value(REPO_SETTINGS_FIND_INSTANCES, false).toBool(); }

    /*!
     * Returns true if find invalid data is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFindInvalidDataChecked() const
    { return settings.value(REPO_SETTINGS_FIND_INAVLID_DATA, false).toBool(); }

    /*!
     * Returns true if fix infacing normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFixInfacingNormalsChecked() const
    { return settings.value(REPO_SETTINGS_FIX_INFACING_NORMALS, false).toBool(); }

    /*!
     * Returns true if flip UV coordinates is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFlipUVCoordinatesChecked() const
    { return settings.value(REPO_SETTINGS_FLIP_UV_COORDINATES, false).toBool(); }

    /*!
     * Returns true if flip winding order is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFlipWindingOrderChecked() const
    { return settings.value(REPO_SETTINGS_FLIP_WINDING_ORDER, false).toBool(); }

    /*!
     * Returns true if generate normals is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isGenerateNormalsChecked() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS, false).toBool(); }

    /*!
     * Returns true if generate flat normals is checked in settings, false
     * oterwise. Defaults to true.
     */
    bool isGenerateNormalsFlatChecked() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS_FLAT, true).toBool(); }

    /*!
     * Returns true if generate smooth normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isGenerateNormalsSmoothChecked() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS_SMOOTH, false).toBool(); }

    /*!
     * Returns true if improve cache locality is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isImproveCacheLocalityChecked() const
    { return settings.value(REPO_SETTINGS_IMPROVE_CACHE_LOCALITY, false).toBool(); }

    /*!
     * Returns true if join identical vertices is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isJoinIdenticalVerticesChecked() const
    { return settings.value(REPO_SETTINGS_JOIN_IDENTICAL_VERTICES, false).toBool(); }

    /*!
     * Returns true if limit bone weights is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isLimitBoneWeightsChecked() const
    { return settings.value(REPO_SETTINGS_LIMIT_BONE_WEIGHTS, false).toBool(); }

    /*!
     * Returns true if make left handed is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isMakeLeftHandedChecked() const
    { return settings.value(REPO_SETTINGS_MAKE_LEFT_HANDED, false).toBool(); }

    /*!
     * Returns true if optimize meshes is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isOptimizeMeshesChecked() const
    { return settings.value(REPO_SETTINGS_OPTIMIZE_MESHES, false).toBool(); }

    /*!
     * Returns true if pre-transform UV coordinates is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool isPreTransformUVCoordinatesChecked() const
    { return settings.value(REPO_SETTINGS_PRE_TRANSFORM_UV_COORDINATES, false).toBool(); }

    /*!
     * Returns true if pre-transform vertices is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isPreTransformVerticesChecked() const
    { return settings.value(REPO_SETTINGS_PRE_TRANSFORM_VERTICES, false).toBool(); }

    /*!
     * Returns true if normalize pre-transform vertices is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool isPreTransformVerticesNormalizeChecked() const
    { return settings.value(REPO_SETTINGS_PRE_TRANSFORM_VERTICES_NORMALIZE, false).toBool(); }

    /*!
     * Returns true if remove components is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isRemoveComponentsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS, false).toBool(); }

    /*!
     * Returns true if remove components animations is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool isRemoveComponentsAnimationsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_ANIMATIONS, false).toBool(); }

    /*!
     * Returns true if remove components bi/tangents is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool isRemoveComponentsBiTangentsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_BI_TANGENTS, false).toBool(); }

    /*!
     * Returns true if remove components bone weights is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool isRemoveComponentsBoneWeightsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_BONE_WEIGHTS, false).toBool(); }

    /*!
     * Returns true if remove components cameras is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsCamerasChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_CAMERAS, false).toBool(); }

    /*!
     * Returns true if remove components colors is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsColorsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_COLORS, false).toBool(); }

    /*!
     * Returns true if remove components lights is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsLightsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_LIGHTS, false).toBool(); }

    /*!
     * Returns true if remove components materials is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsMaterialsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_MATERIALS, false).toBool(); }

    /*!
     * Returns true if remove components meshes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsMeshesChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_MESHES, false).toBool(); }

    /*!
     * Returns true if remove components normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsNormalsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_NORMALS, false).toBool(); }

    /*!
     * Returns true if remove components textures is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveComponentsTexturesChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURES, false).toBool(); }

    /*!
     * Returns true if remove components texture coordinates is checked in
     * settings, false otherwise. Defaults to false.
     */
    bool isRemoveComponentsTextureCoordinatesChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURE_COORDINATES, false).toBool(); }

    /*!
     * Returns true if remove redundant materials is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveRedundantMaterialsChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_REDUNDANT_MATERIALS, false).toBool(); }

    /*!
     * Returns true if remove redundant nodes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isRemoveRedundantNodesChecked() const
    { return settings.value(REPO_SETTINGS_REMOVE_REDUNDANT_NODES, false).toBool(); }

    /*!
     * Returns ture if sort and remove is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isSortAndRemoveChecked() const
    { return settings.value(REPO_SETTINGS_SORT_AND_REMOVE, false).toBool(); }

    /*!
     * Returns true if sort and remove points is checked in settings, false
     * otherwise. Defaults to true.
     */
    bool isSortAndRemovePointsChecked() const
    { return settings.value(REPO_SETTINGS_SORT_AND_REMOVE_POINTS, true).toBool(); }

    /*!
     * Returns true if sort and remove lines is checked in settings, false
     * otherwise. Defaults to true.
     */
    bool isSortAndRemoveLinesChecked() const
    { return settings.value(REPO_SETTINGS_SORT_AND_REMOVE_LINES, true).toBool(); }

    /*!
     * Returns true if sort and remove triangles is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isSortAndRemoveTrianglesChecked() const
    { return settings.value(REPO_SETTINGS_SORT_AND_REMOVE_TRIANGLES, false).toBool(); }

    /*!
     * Returns true if sort and remove points is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isSortAndRemovePolygonsChecked() const
    { return settings.value(REPO_SETTINGS_SORT_AND_REMOVE_POLYGONS, false).toBool(); }


    /*!
     * Returns true if split by bone count is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isSplitByBoneCountChecked() const
    { return settings.value(REPO_SETTINGS_SPLIT_BY_BONE_COUNT, false).toBool(); }

    /*!
     * Returns true if split large meshes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isSplitLargeMeshesChecked() const
    { return settings.value(REPO_SETTINGS_SPLIT_LARGE_MESHES, false).toBool(); }

    /*!
     * Returns true if triangulate is checked in settings, false otherwise.
     * Defaults to true.
     */
    bool isTriangulateChecked() const
    { return settings.value(REPO_SETTINGS_TRIANGULATE, true).toBool(); }

    /*!
     * Returns true if validate data structures is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isValidateDataStructuresChecked() const
    { return settings.value(REPO_SETTINGS_VALIDATE_DATA_STRUCTURES, false).toBool(); }

public :

    static const QString REPO_SETTINGS_CALCULATE_TANGENT_SPACE;

    static const QString REPO_SETTINGS_CONVERT_TO_UV_COORDINATES;

    static const QString REPO_SETTINGS_DEGENERATES_TO_POINTS_LINES;

    static const QString REPO_SETTINGS_DEBONE;

    static const QString REPO_SETTINGS_DEBONE_THRESHOLD;

    static const QString REPO_SETTINGS_DEBONE_ONLY_IF_ALL;

    static const QString REPO_SETTINGS_FIND_INSTANCES;

    static const QString REPO_SETTINGS_FIND_INAVLID_DATA;

    static const QString REPO_SETTINGS_FIND_INAVLID_DATA_ANIMATION_ACCURACY;

    static const QString REPO_SETTINGS_FIX_INFACING_NORMALS;

    static const QString REPO_SETTINGS_FLIP_UV_COORDINATES;

    static const QString REPO_SETTINGS_FLIP_WINDING_ORDER;

    static const QString REPO_SETTINGS_GENERATE_NORMALS;

    static const QString REPO_SETTINGS_GENERATE_NORMALS_FLAT;

    static const QString REPO_SETTINGS_GENERATE_NORMALS_SMOOTH;

    static const QString REPO_SETTINGS_GENERATE_NORMALS_SMOOTH_CREASE_ANGLE;

    static const QString REPO_SETTINGS_IMPROVE_CACHE_LOCALITY;

    static const QString REPO_SETTINGS_IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE;

    static const QString REPO_SETTINGS_JOIN_IDENTICAL_VERTICES;

    static const QString REPO_SETTINGS_LIMIT_BONE_WEIGHTS;

    static const QString REPO_SETTINGS_LIMIT_BONE_WEIGHTS_MAX_WEIGHTS;

    static const QString REPO_SETTINGS_MAKE_LEFT_HANDED;

    static const QString REPO_SETTINGS_OPTIMIZE_MESHES;

    static const QString REPO_SETTINGS_PRE_TRANSFORM_UV_COORDINATES;

    static const QString REPO_SETTINGS_PRE_TRANSFORM_VERTICES;

    static const QString REPO_SETTINGS_PRE_TRANSFORM_VERTICES_NORMALIZE;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_ANIMATIONS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_BI_TANGENTS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_BONE_WEIGHTS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_CAMERAS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_COLORS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_LIGHTS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_MATERIALS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_MESHES;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_NORMALS;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURES;

    static const QString REPO_SETTINGS_REMOVE_COMPONENTS_TEXTURE_COORDINATES;

    static const QString REPO_SETTINGS_REMOVE_REDUNDANT_MATERIALS;

    static const QString REPO_SETTINGS_REMOVE_REDUNDANT_MATERIALS_SKIP;

    static const QString REPO_SETTINGS_REMOVE_REDUNDANT_NODES;

    static const QString REPO_SETTINGS_REMOVE_REDUNDANT_NODES_SKIP;

    static const QString REPO_SETTINGS_SORT_AND_REMOVE;

    static const QString REPO_SETTINGS_SORT_AND_REMOVE_POINTS;

    static const QString REPO_SETTINGS_SORT_AND_REMOVE_LINES;

    static const QString REPO_SETTINGS_SORT_AND_REMOVE_TRIANGLES;

    static const QString REPO_SETTINGS_SORT_AND_REMOVE_POLYGONS;

    static const QString REPO_SETTINGS_SPLIT_BY_BONE_COUNT;

    static const QString REPO_SETTINGS_SPLIT_BY_BONE_COUNT_MAX_BONES;

    static const QString REPO_SETTINGS_SPLIT_LARGE_MESHES;

    static const QString REPO_SETTINGS_SPLIT_LARGE_MESHES_TRIANGLE_LIMIT;

    static const QString REPO_SETTINGS_SPLIT_LARGE_MESHES_VERTEX_LIMIT;

    static const QString REPO_SETTINGS_TRIANGULATE;

    static const QString REPO_SETTINGS_VALIDATE_DATA_STRUCTURES;

private:

    //! UI var.
    Ui::RepoWidgetAssimpFlags *ui;

    //! Application settings.
    QSettings settings;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_WIDGET_ASSIMP_FLAGS_H
