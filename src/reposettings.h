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


#ifndef REPOSETTINGS_H
#define REPOSETTINGS_H

#include <QSettings>
//#include <assimp/postprocess.h>

namespace repo {
namespace gui {

class RepoSettings : public QSettings
{

public:

    RepoSettings() {}

    ~RepoSettings() {}

public :

    //! Returns a bit-wise or of currently set Assimp post-processing flags.
    unsigned int getAssimpPostProcessingFlags(unsigned int flags = 0) const;

    /*!
     * Returns true if calculate tangent space is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getCalculateTangentSpace() const
    { return value("", false).toBool(); }

    /*!
     * Returns the maximum smoothing angle for calc tangent space. Defaults to
     * 45.
     */
    float getCalculateTangentSpaceMaxSmoothingAngle() const
    { return value("",
	/*core::AssimpWrapper::getDefaultCalculateTangentSpaceMaxSmoothingAngle()*/1).toFloat();
	}

    /*!
     * Returns true if convert to UV coords is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getConvertToUVCoordinates() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if degenerates to points/lines is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getDegeneratesToPointsLines() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if debone is checked in settings, false otherwise. Defaults
     * to false.
     */
    bool getDebone() const
    { return value("", false).toBool(); }

    /*!
     * Returns the debone threshold set in settings. Defaults to Assimp's
     * AI_DEBONE_THRESHOLD = 1.0f
     */
    float getDeboneThreshold() const
    { return value("",
	/*core::AssimpWrapper::getDefaultDeboneThreshold()*/1).toFloat();
	}

    /*!
     * Returns true if debone only if all is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getDeboneOnlyIfAll() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if find instances is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getFindInstances() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if find invalid data is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFindInvalidData() const
    { return value("", false).toBool(); }

    /*!
     * Returns the animation accuracy for find invalid data set in settings.
     * Defaults to 0.0f == all comparisons are exact.
     * See http://assimp.sourceforge.net/lib_html/config_8h.html#ad223c5e7e63d2937685cc704a181b950
     */
    float getFindInvalidDataAnimationAccuracy() const
    { return value("",
	/*core::AssimpWrapper::getDefaultFindInvalidDataAnimationAccuracy()*/1).toFloat();
	}

    /*!
     * Returns true if fix infacing normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFixInfacingNormals() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if flip UV coordinates is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFlipUVCoordinates() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if flip winding order is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFlipWindingOrder() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if generate normals is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getGenerateNormals() const
    { return value("", false).toBool(); }

    /*!
     * Returns true if generate flat normals is checked in settings, false
     * oterwise. Defaults to true.
     */
    bool getGenerateNormalsFlat() const
    { return value("", true).toBool(); }

    /*!
     * Returns true if generate smooth normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getGenerateNormalsSmooth() const
    { return value("", false).toBool(); }

    /*!
     * Returns the crease angle for generate smooth normals set in settings.
     * Defaults to 175.0f.
     */
    float getGenerateNormalsSmoothCreaseAngle() const
    { return value("",
	/* core::AssimpWrapper::getDefaultGenerateSmoothNormalsCreaseAngle()*/1).toFloat();
	}

    /*!
     * Returns true if improve cache locality is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getImproveCacheLocality() const
    { return value("", false).toBool(); }

    /*!
     * Returns the vertex cache size for improve cache locality set in settings.
     * Defaults to PP_ICL_PTCACHE_SIZE = 12.
     */
    int getImproveCacheLocalityVertexCacheSize() const
    { return value("",
	/*core::AssimpWrapper::getDefaultImproveCacheLocalityVertexCacheSize()*/1).toInt();
	}

    /*!
     * Returns true if join identical vertices is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getJoinIdenticalVertices() const
    { return value(JOIN_IDENTICAL_VERTICES, false).toBool(); }

    /*!
     * Returns true if limit bone weights is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getLimitBoneWeights() const
    { return value(LIMIT_BONE_WEIGHTS, false).toBool(); }

    /*!
     * Returns the max weight for limit bone weights set in settings. Defaults
     * to AI_LMW_MAX_WEIGHTS = 4.
     */
    int getLimitBoneWeightsMaxWeight() const
    { return value(LIMIT_BONE_WEIGHTS_MAX_WEIGHTS,
	/*core::AssimpWrapper::getDefaultBoneWeightsMaxWeights()*/1).toInt();
	}

    /*!
     * Returns true if make left handed is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getMakeLeftHanded() const
    { return value(MAKE_LEFT_HANDED, false).toBool(); }

    /*!
     * Returns true if optimize meshes is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getOptimizeMeshes() const
    { return value(OPTIMIZE_MESHES, false).toBool(); }

    /*!
     * Returns true if pre-transform UV coordinates is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getPreTransformUVCoordinates() const
    { return value(PRE_TRANSFORM_UV_COORDINATES, false).toBool(); }

    /*!
     * Returns true if pre-transform vertices is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getPreTransformVertices() const
    { return value(PRE_TRANSFORM_VERTICES, false).toBool(); }

    /*!
     * Returns true if normalize pre-transform vertices is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getPreTransformVerticesNormalize() const
    { return value(PRE_TRANSFORM_VERTICES_NORMALIZE, false).toBool(); }

    /*!
     * Returns true if remove components is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getRemoveComponents() const
    { return value(REMOVE_COMPONENTS, false).toBool(); }

    /*!
     * Returns true if remove components animations is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getRemoveComponentsAnimations() const
    { return value(REMOVE_COMPONENTS_ANIMATIONS, false).toBool(); }

    /*!
     * Returns true if remove components bi/tangents is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getRemoveComponentsBiTangents() const
    { return value(REMOVE_COMPONENTS_BI_TANGENTS, false).toBool(); }

    /*!
     * Returns true if remove components bone weights is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getRemoveComponentsBoneWeights() const
    { return value(REMOVE_COMPONENTS_BONE_WEIGHTS, false).toBool(); }

    /*!
     * Returns true if remove components cameras is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsCameras() const
    { return value(REMOVE_COMPONENTS_CAMERAS, false).toBool(); }

    /*!
     * Returns true if remove components colors is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsColors() const
    { return value(REMOVE_COMPONENTS_COLORS, false).toBool(); }

    /*!
     * Returns true if remove components lights is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsLights() const
    { return value(REMOVE_COMPONENTS_LIGHTS, false).toBool(); }

    /*!
     * Returns true if remove components materials is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsMaterials() const
    { return value(REMOVE_COMPONENTS_MATERIALS, false).toBool(); }

    /*!
     * Returns true if remove components meshes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsMeshes() const
    { return value(REMOVE_COMPONENTS_MESHES, false).toBool(); }

    /*!
     * Returns true if remove components normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsNormals() const
    { return value(REMOVE_COMPONENTS_NORMALS, false).toBool(); }

    /*!
     * Returns true if remove components textures is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsTextures() const
    { return value(REMOVE_COMPONENTS_TEXTURES, false).toBool(); }

    /*!
     * Returns true if remove components texture coordinates is checked in
     * settings, false otherwise. Defaults to false.
     */
    bool getRemoveComponentsTextureCoordinates() const
    { return value(REMOVE_COMPONENTS_TEXTURE_COORDINATES, false).toBool(); }

    /*!
     * Returns true if remove redundant materials is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveRedundantMaterials() const
    { return value(REMOVE_REDUNDANT_MATERIALS, false).toBool(); }

    /*!
     * Returns a space-separated string of materials names to be skipped set in
     * settings. Defaults to empty string.
     */
    QString getRemoveRedundantMaterialsSkip() const
    { return value(REMOVE_REDUNDANT_MATERIALS_SKIP, "").toString(); }

    /*!
     * Returns true if remove redundant nodes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveRedundantNodes() const
    { return value(REMOVE_REDUNDANT_NODES, false).toBool(); }

    /*!
     * Returns a space-separated string of node names to be skipped set in
     * settings. Defaults to empty string.
     */
    QString getRemoveRedundantNodesSkip() const
    { return value(REMOVE_REDUNDANT_NODES_SKIP, "").toString(); }

    /*!
     * Returns ture if sort and remove is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getSortAndRemove() const
    { return value(SORT_AND_REMOVE, false).toBool(); }

    /*!
     * Returns true if sort and remove lines is checked in settings, false
     * otherwise. Defaults to true.
     */
    bool getSortAndRemoveLines() const
    { return value(SORT_AND_REMOVE_LINES, true).toBool(); }

    /*!
     * Returns true if sort and remove points is checked in settings, false
     * otherwise. Defaults to true.
     */
    bool getSortAndRemovePoints() const
    { return value(SORT_AND_REMOVE_POINTS, true).toBool(); }

    /*!
     * Returns true if sort and remove points is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSortAndRemovePolygons() const
    { return value(SORT_AND_REMOVE_POLYGONS, false).toBool(); }

    /*!
     * Returns true if sort and remove triangles is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSortAndRemoveTriangles() const
    { return value(SORT_AND_REMOVE_TRIANGLES, false).toBool(); }

    /*!
     * Returns true if split by bone count is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSplitByBoneCount() const
    { return value(SPLIT_BY_BONE_COUNT, false).toBool(); }

    /*!
     * Returns a split by bones count max bones set in settings. Defaults to
     * AI_SBBC_DEFAULT_MAX_BONES = 60.
     */
    int getSplitByBoneCountMaxBones() const
    { return value(SPLIT_BY_BONE_COUNT_MAX_BONES,
	/*core::AssimpWrapper::getDefaultSplitByBoneCountMaxBones()*/1).toInt();
	}

    /*!
     * Returns true if split large meshes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSplitLargeMeshes() const
    { return value(SPLIT_LARGE_MESHES, false).toBool(); }

    /*!
     * Returns a split large meshes triangle limit set in settings. Defaults to
     * AI_SLM_DEFAULT_MAX_TRIANGLES = 1,000,000.
     */
    int getSplitLargeMeshesTriangleLimit() const
    { return value(SPLIT_LARGE_MESHES_TRIANGLE_LIMIT,
       /* core::AssimpWrapper::getDefaultSplitLargeMeshesTriangleLimit()*/1).toInt(); }

    /*!
     * Returns a split large meshes triangle limit set in settings. Defaults to
     * AI_SLM_DEFAULT_MAX_VERTICES = 1,000,000.
     */
    int getSplitLargeMeshesVertexLimit() const
    { return value(SPLIT_LARGE_MESHES_VERTEX_LIMIT,
                   /*core::AssimpWrapper::getDefaultSplitLargeMeshesVertexLimit()*/1).toInt(); }

    /*!
     * Returns true if triangulate is checked in settings, false otherwise.
     * Defaults to true.
     */
    bool getTriangulate() const
    { return value(TRIANGULATE, true).toBool(); }

    /*!
     * Returns true if validate data structures is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getValidateDataStructures() const
    { return value(VALIDATE_DATA_STRUCTURES, false).toBool(); }

public :


    //! Sets the calculate tangent space to settings.
    void setCalculateTangentSpace(bool on)
    { setValue(CALCULATE_TANGENT_SPACE, on); }

    void setCalculateTangentSpaceMaxSmoothingAngle(float angle)
    { setValue(CALCULATE_TANGENT_SPACE_MAX_SMOOTHING_ANGLE, angle); }

    //! Sets the convert to UV coordinates to settings.
    void setConvertToUVCoordinates(bool on)
    { setValue(CONVERT_TO_UV_COORDINATES, on); }

    //! Sets the degenerates to points and lines to settings.
    void setDegeneratesToPointsLines(bool on)
    { setValue(DEGENERATES_TO_POINTS_LINES, on); }

    //! Sets the debone to settings.
    void setDebone(bool on) { setValue(DEBONE, on); }

    //! Sets the debone threshold to settings.
    void setDeboneThreshold(float t)
    { setValue(DEBONE_THRESHOLD, t); }

    void setDeboneOnlyIfAll(bool on)
    { setValue(DEBONE_ONLY_IF_ALL, on); }

    void setFindInstances(bool on)
    { setValue(FIND_INSTANCES, on); }

    void setFindInvalidData(bool on)
    { setValue(FIND_INAVLID_DATA, on); }

    void setFindInvalidDataAnimationAccuracy(float f)
    { setValue(FIND_INAVLID_DATA_ANIMATION_ACCURACY, f); }

    void setFixInfacingNormals(bool on)
    { setValue(FIX_INFACING_NORMALS, on); }

    void setFlipUVCoordinates(bool on)
    { setValue(FLIP_UV_COORDINATES, on); }

    void setFlipWindingOrder(bool on)
    { setValue(FLIP_WINDING_ORDER, on); }

    void setGenerateNormals(bool on)
    { setValue(GENERATE_NORMALS, on); }

    void setGenerateNormalsFlat(bool on)
    { setValue(GENERATE_NORMALS_FLAT, on); }

    void setGenerateNormalsSmooth(bool on)
    { setValue(GENERATE_NORMALS_SMOOTH, on); }

    void setGenerateNormalsSmoothCreaseAngle(float f)
    { setValue(GENERATE_NORMALS_SMOOTH_CREASE_ANGLE, f); }

    void setImproveCacheLocality(bool on)
    { setValue(IMPROVE_CACHE_LOCALITY, on); }

    void setImproveCacheLocalityCacheSize(int vertexCount)
    { setValue(IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE, vertexCount); }

    void setJoinIdenticalVertices(bool on)
    { setValue(JOIN_IDENTICAL_VERTICES, on); }

    void setLimitBoneWeights(bool on)
    { setValue(LIMIT_BONE_WEIGHTS, on); }

    void setLimitBoneWeightsMaxWeights(int i)
    { setValue(LIMIT_BONE_WEIGHTS_MAX_WEIGHTS, i); }

    void setMakeLeftHanded(bool on)
    { setValue(MAKE_LEFT_HANDED, on); }

    void setOptimizeMeshes(bool on)
    { setValue(OPTIMIZE_MESHES, on); }

    void setPreTransformUVCoordinates(bool on)
    { setValue(PRE_TRANSFORM_UV_COORDINATES, on); }

    void setPreTransformVertices(bool on)
    { setValue(PRE_TRANSFORM_VERTICES, on); }

    void setPreTransforVerticesNormalize(bool on)
    { setValue(PRE_TRANSFORM_VERTICES_NORMALIZE, on); }

    void setRemoveComponents(bool on)
    { setValue(REMOVE_COMPONENTS, on); }

    void setRemoveComponentsAnimations(bool on)
    { setValue(REMOVE_COMPONENTS_ANIMATIONS, on); }

    void setRemoveComponentsBiTangents(bool on)
    { setValue(REMOVE_COMPONENTS_BI_TANGENTS, on); }

    void setRemoveComponentsBoneWeights(bool on)
    { setValue(REMOVE_COMPONENTS_BONE_WEIGHTS, on); }

    void setRemoveComponentsCameras(bool on)
    { setValue(REMOVE_COMPONENTS_CAMERAS, on); }

    void setRemoveComponentsColors(bool on)
    { setValue(REMOVE_COMPONENTS_COLORS, on); }

    void setRemoveComponentsLights(bool on)
    { setValue(REMOVE_COMPONENTS_LIGHTS, on); }

    void setRemoveComponentsMaterials(bool on)
    { setValue(REMOVE_COMPONENTS_MATERIALS, on); }

    void setRemoveComponentsMeshes(bool on)
    { setValue(REMOVE_COMPONENTS_MESHES, on); }

    void setRemoveComponentsNormals(bool on)
    { setValue(REMOVE_COMPONENTS_NORMALS, on); }

    void setRemoveComponentsTextures(bool on)
    { setValue(REMOVE_COMPONENTS_TEXTURES, on); }

    void setRemoveComponentsTextureCoordinates(bool on)
    { setValue(REMOVE_COMPONENTS_TEXTURE_COORDINATES, on); }

    void setRemoveRedundantMaterials(bool on)
    { setValue(REMOVE_REDUNDANT_MATERIALS, on); }

    void setRemoveRedundantMaterialsSkip(const QString& skip)
    { setValue(REMOVE_REDUNDANT_MATERIALS_SKIP, skip); }

    void setRemoveRedundantNodes(bool on)
    { setValue(REMOVE_REDUNDANT_NODES, on); }

    void setRemoveRedundantNodesSkip(const QString& skip)
    { setValue(REMOVE_REDUNDANT_NODES_SKIP, skip); }

    void setSortAndRemove(bool on)
    { setValue(SORT_AND_REMOVE, on); }

    void setSortAndRemoveLines(bool on)
    { setValue(SORT_AND_REMOVE_LINES, on); }

    void setSortAndRemovePoints(bool on)
    { setValue(SORT_AND_REMOVE_POINTS, on); }

    void setSortAndRemovePolygons(bool on)
    { setValue(SORT_AND_REMOVE_POLYGONS, on); }

    void setSortAndRemoveTriangles(bool on)
    { setValue(SORT_AND_REMOVE_TRIANGLES, on); }

    void setSplitByBoneCount(bool on)
    { setValue(SPLIT_BY_BONE_COUNT, on); }

    void setSplitByBoneCountMaxBones(int max)
    { setValue(SPLIT_BY_BONE_COUNT_MAX_BONES, max); }

    void setSplitLargeMeshes(bool on)
    { setValue(SPLIT_LARGE_MESHES, on); }

    void setSplitLargeMeshesTriangleLimit(int limit)
    { setValue(SPLIT_LARGE_MESHES_TRIANGLE_LIMIT, limit); }

    void setSplitLargeMeshesVertexLimit(int limit)
    { setValue(SPLIT_LARGE_MESHES_VERTEX_LIMIT, limit); }

    void setTriangulate(bool on)
    { setValue(TRIANGULATE, on); }

    void setValidateDataStructures(bool on)
    { setValue(VALIDATE_DATA_STRUCTURES, on); }

private :

    static const QString CALCULATE_TANGENT_SPACE;

    static const QString CALCULATE_TANGENT_SPACE_MAX_SMOOTHING_ANGLE;

    static const QString CONVERT_TO_UV_COORDINATES;

    static const QString DEGENERATES_TO_POINTS_LINES;

    static const QString DEBONE;

    static const QString DEBONE_THRESHOLD;

    static const QString DEBONE_ONLY_IF_ALL;

    static const QString FIND_INSTANCES;

    static const QString FIND_INAVLID_DATA;

    static const QString FIND_INAVLID_DATA_ANIMATION_ACCURACY;

    static const QString FIX_INFACING_NORMALS;

    static const QString FLIP_UV_COORDINATES;

    static const QString FLIP_WINDING_ORDER;

    static const QString GENERATE_NORMALS;

    static const QString GENERATE_NORMALS_FLAT;

    static const QString GENERATE_NORMALS_SMOOTH;

    static const QString GENERATE_NORMALS_SMOOTH_CREASE_ANGLE;

    static const QString IMPROVE_CACHE_LOCALITY;

    static const QString IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE;

    static const QString JOIN_IDENTICAL_VERTICES;

    static const QString LIMIT_BONE_WEIGHTS;

    static const QString LIMIT_BONE_WEIGHTS_MAX_WEIGHTS;

    static const QString MAKE_LEFT_HANDED;

    static const QString OPTIMIZE_MESHES;

    static const QString PRE_TRANSFORM_UV_COORDINATES;

    static const QString PRE_TRANSFORM_VERTICES;

    static const QString PRE_TRANSFORM_VERTICES_NORMALIZE;

    static const QString REMOVE_COMPONENTS;

    static const QString REMOVE_COMPONENTS_ANIMATIONS;

    static const QString REMOVE_COMPONENTS_BI_TANGENTS;

    static const QString REMOVE_COMPONENTS_BONE_WEIGHTS;

    static const QString REMOVE_COMPONENTS_CAMERAS;

    static const QString REMOVE_COMPONENTS_COLORS;

    static const QString REMOVE_COMPONENTS_LIGHTS;

    static const QString REMOVE_COMPONENTS_MATERIALS;

    static const QString REMOVE_COMPONENTS_MESHES;

    static const QString REMOVE_COMPONENTS_NORMALS;

    static const QString REMOVE_COMPONENTS_TEXTURES;

    static const QString REMOVE_COMPONENTS_TEXTURE_COORDINATES;

    static const QString REMOVE_REDUNDANT_MATERIALS;

    static const QString REMOVE_REDUNDANT_MATERIALS_SKIP;

    static const QString REMOVE_REDUNDANT_NODES;

    static const QString REMOVE_REDUNDANT_NODES_SKIP;

    static const QString SORT_AND_REMOVE;

    static const QString SORT_AND_REMOVE_POINTS;

    static const QString SORT_AND_REMOVE_LINES;

    static const QString SORT_AND_REMOVE_TRIANGLES;

    static const QString SORT_AND_REMOVE_POLYGONS;

    static const QString SPLIT_BY_BONE_COUNT;

    static const QString SPLIT_BY_BONE_COUNT_MAX_BONES;

    static const QString SPLIT_LARGE_MESHES;

    static const QString SPLIT_LARGE_MESHES_TRIANGLE_LIMIT;

    static const QString SPLIT_LARGE_MESHES_VERTEX_LIMIT;

    static const QString TRIANGULATE;

    static const QString VALIDATE_DATA_STRUCTURES;

};


} // end namespace gui
} // end namespace repo


#endif // REPOSETTINGS_H
