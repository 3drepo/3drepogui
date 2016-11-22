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


#pragma once

#include <QSettings>
#include <repo/manipulator/modelconvertor/import/repo_model_import_config.h>
#include <repo/manipulator/modelconvertor/import/repo_model_import_config_default_values.h>


namespace repo {
namespace settings {

class RepoSettings : public QSettings, public repo::manipulator::modelconvertor::ModelImportConfig
{

public:

    RepoSettings() : QSettings() {}

    ~RepoSettings() {}

public :

    /*!
     * Returns true if calculate tangent space is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getCalculateTangentSpace() const
	{
		return value(QString(CALCULATE_TANGENT_SPACE.c_str()), repoDefaultCalculateTangentSpace).toBool();
	}

    /*!
     * Returns the maximum smoothing angle for calc tangent space. Defaults to
     * 45.
     */
    float getCalculateTangentSpaceMaxSmoothingAngle() const
	{
		return value(QString(CALCULATE_TANGENT_SPACE_MAX_SMOOTHING_ANGLE.c_str()),
	repoDefaultCalculateTangentSpaceMaxSmoothingAngle).toFloat();
	}

    /*!
     * Returns true if convert to UV coords is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getConvertToUVCoordinates() const
	{
		return value(QString(CONVERT_TO_UV_COORDINATES.c_str()), 
			repoDefaultConvertToUVCoordinates).toBool();
	}

    /*!
     * Returns true if degenerates to points/lines is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getDegeneratesToPointsLines() const
	{
		return value(QString(DEGENERATES_TO_POINTS_LINES.c_str()), repoDefaultDegeneratesToPointsLines).toBool();
	}

    /*!
     * Returns true if debone is checked in settings, false otherwise. Defaults
     * to false.
     */
    bool getDebone() const
	{
		return value(QString(DEBONE.c_str()), repoDefaultDebone).toBool();
	}

    /*!
     * Returns the debone threshold set in settings. Defaults to Assimp's
     * AI_DEBONE_THRESHOLD = 1.0f
     */
    float getDeboneThreshold() const
	{
		return value(QString(DEBONE_THRESHOLD.c_str()),
		repoDefaultDeboneThreshold).toFloat();
	}

    /*!
     * Returns true if debone only if all is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getDeboneOnlyIfAll() const
	{
		return value(QString(DEBONE_ONLY_IF_ALL.c_str()), repoDefaultDeboneOnlyIfAll).toBool();
	}

    /*!
     * Returns true if find instances is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getFindInstances() const
	{
		return value(QString(FIND_INSTANCES.c_str()), repoDefaultFindInstances).toBool();
	}

    /*!
     * Returns true if find invalid data is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFindInvalidData() const
	{
		return value(QString(FIND_INAVLID_DATA.c_str()), repoDefaultFindInvalidData).toBool();
	}

    /*!
     * Returns the animation accuracy for find invalid data set in settings.
     * Defaults to 0.0f == all comparisons are exact.
     * See http://assimp.sourceforge.net/lib_html/config_8h.html#ad223c5e7e63d2937685cc704a181b950
     */
    float getFindInvalidDataAnimationAccuracy() const
	{
		return value(QString(FIND_INAVLID_DATA_ANIMATION_ACCURACY.c_str()),
			repoDefaultFindInvalidDataAnimationAccuracy).toFloat();
	}

    /*!
     * Returns true if fix infacing normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFixInfacingNormals() const
	{
		return value(QString(FIX_INFACING_NORMALS.c_str()), repoDefaultFixInfacingNormals).toBool();
	}

    /*!
     * Returns true if flip UV coordinates is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFlipUVCoordinates() const
	{
		return value(QString(FLIP_UV_COORDINATES.c_str()), repoDefaultFlipUVCoordinates).toBool();
	}

    /*!
     * Returns true if flip winding order is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getFlipWindingOrder() const
	{
		return value(QString(FLIP_WINDING_ORDER.c_str()), repoDefaultFlipWindingOrder).toBool();
	}

    /*!
     * Returns true if generate normals is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getGenerateNormals() const
	{
		return value(QString(GENERATE_NORMALS.c_str()), repoDefaultGenerateNormals).toBool();
	}

    /*!
     * Returns true if generate flat normals is checked in settings, false
     * oterwise. Defaults to true.
     */
    bool getGenerateNormalsFlat() const
	{
		return value(QString(GENERATE_NORMALS_FLAT.c_str()), repoDefaultGenerateNormalsFlat).toBool();
	}

    /*!
     * Returns true if generate smooth normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getGenerateNormalsSmooth() const
	{
		return value(QString(GENERATE_NORMALS_SMOOTH.c_str()), repoDefaultGenerateNormalsSmooth).toBool();
	}

    /*!
     * Returns the crease angle for generate smooth normals set in settings.
     * Defaults to 175.0f.
     */
    float getGenerateNormalsSmoothCreaseAngle() const
	{
		return value(QString(GENERATE_NORMALS_SMOOTH_CREASE_ANGLE.c_str()),
			repoDefaultGenerateNormalsSmoothCreaseAngle).toFloat();
	}

    /*!
     * Returns true if improve cache locality is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getImproveCacheLocality() const
	{
		return value(QString(IMPROVE_CACHE_LOCALITY.c_str()), repoDefaultImproveCacheLocality).toBool();
	}

    /*!
     * Returns the vertex cache size for improve cache locality set in settings.
     * Defaults to PP_ICL_PTCACHE_SIZE = 12.
     */
    int getImproveCacheLocalityVertexCacheSize() const
	{
		return value(QString(IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE.c_str()),
			repoDefaultImproveCacheLocalityVertexCacheSize).toInt();
	}

    /*!
     * Returns true if join identical vertices is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getJoinIdenticalVertices() const
	{
		return value(QString(JOIN_IDENTICAL_VERTICES.c_str()), repoDefaultJoinIdenticalVertices).toBool();
	}

    /*!
     * Returns true if limit bone weights is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getLimitBoneWeights() const
	{
		return value(QString(LIMIT_BONE_WEIGHTS.c_str()), repoDefaultLimitBoneWeights).toBool();
	}

    /*!
     * Returns the max weight for limit bone weights set in settings. Defaults
     * to AI_LMW_MAX_WEIGHTS = 4.
     */
    int getLimitBoneWeightsMaxWeight() const
	{
		return value(QString(LIMIT_BONE_WEIGHTS_MAX_WEIGHTS.c_str()),
	repoDefaultLimitBoneWeightsMaxWeight).toInt();
	}

    /*!
     * Returns true if make left handed is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getMakeLeftHanded() const
	{
		return value(QString(MAKE_LEFT_HANDED.c_str()), repoDefaultMakeLeftHanded).toBool();
	}

    /*!
     * Returns true if optimize meshes is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getOptimizeMeshes() const
	{
		return value(QString(OPTIMIZE_MESHES.c_str()), repoDefaultOptimizeMeshes).toBool();
	}

    /*!
     * Returns true if pre-transform UV coordinates is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getPreTransformUVCoordinates() const
	{
		return value(QString(PRE_TRANSFORM_UV_COORDINATES.c_str()), repoDefaultPreTransformUVCoordinates).toBool();
	}

    /*!
     * Returns true if pre-transform vertices is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getPreTransformVertices() const
	{
		return value(QString(PRE_TRANSFORM_VERTICES.c_str()), repoDefaultPreTransformVertices).toBool();
	}

    /*!
     * Returns true if normalize pre-transform vertices is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getPreTransformVerticesNormalize() const
	{
		return value(QString(PRE_TRANSFORM_VERTICES_NORMALIZE.c_str()), repoDefaultPreTransformVerticesNormalize).toBool();
	}

    /*!
     * Returns true if remove components is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getRemoveComponents() const
	{
		return value(QString(REMOVE_COMPONENTS.c_str()), repoDefaultRemoveComponents).toBool();
	}

    /*!
     * Returns true if remove components animations is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getRemoveComponentsAnimations() const
	{
		return value(QString(REMOVE_COMPONENTS_ANIMATIONS.c_str()), repoDefaultRemoveComponents).toBool();
	}

    /*!
     * Returns true if remove components bi/tangents is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getRemoveComponentsBiTangents() const
	{
		return value(QString(REMOVE_COMPONENTS_BI_TANGENTS.c_str()), repoDefaultRemoveComponentsBiTangents).toBool();
	}

    /*!
     * Returns true if remove components bone weights is checked in settings,
     * false otherwise. Defaults to false.
     */
    bool getRemoveComponentsBoneWeights() const
	{
		return value(QString(REMOVE_COMPONENTS_BONE_WEIGHTS.c_str()), repoDefaultRemoveComponentsBoneWeights).toBool();
	}

    /*!
     * Returns true if remove components cameras is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsCameras() const
	{
		return value(QString(REMOVE_COMPONENTS_CAMERAS.c_str()), repoDefaultRemoveComponentsCameras).toBool();
	}

    /*!
     * Returns true if remove components colors is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsColors() const
	{
		return value(QString(REMOVE_COMPONENTS_COLORS.c_str()), repoDefaultRemoveComponentsColors).toBool();
	}

    /*!
     * Returns true if remove components lights is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsLights() const
	{
		return value(QString(REMOVE_COMPONENTS_LIGHTS.c_str()), repoDefaultRemoveComponentsLights).toBool();
	}

    /*!
     * Returns true if remove components materials is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsMaterials() const
	{
		return value(QString(REMOVE_COMPONENTS_MATERIALS.c_str()), repoDefaultRemoveComponentsMaterials).toBool();
	}

    /*!
     * Returns true if remove components meshes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsMeshes() const
	{
		return value(QString(REMOVE_COMPONENTS_MESHES.c_str()), repoDefaultRemoveComponentsMeshes).toBool();
	}

    /*!
     * Returns true if remove components normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsNormals() const
	{
		return value(QString(REMOVE_COMPONENTS_NORMALS.c_str()), repoDefaultRemoveComponentsNormals).toBool();
	}

    /*!
     * Returns true if remove components textures is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveComponentsTextures() const
	{
		return value(QString(REMOVE_COMPONENTS_TEXTURES.c_str()), repoDefaultRemoveComponentsTextures).toBool();
	}

    /*!
     * Returns true if remove components texture coordinates is checked in
     * settings, false otherwise. Defaults to false.
     */
    bool getRemoveComponentsTextureCoordinates() const
    { 
		return value(QString(REMOVE_COMPONENTS_TEXTURE_COORDINATES.c_str()), 
			repoDefaultRemoveComponentsTextureCoordinates).toBool();
	}

    /*!
     * Returns true if remove redundant materials is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveRedundantMaterials() const
    { 
		return value(QString(REMOVE_REDUNDANT_MATERIALS.c_str()), repoDefaultRemoveRedundantMaterials).toBool();
	}

    /*!
     * Returns a space-separated string of materials names to be skipped set in
     * settings. Defaults to empty string.
     */
    std::string getRemoveRedundantMaterialsSkip() const
    { 
		return value(QString(REMOVE_REDUNDANT_MATERIALS_SKIP.c_str()), QString(repoDefaultRemoveRedundantMaterialsSkip.c_str())).toString().toStdString();
	}

    /*!
     * Returns true if remove redundant nodes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getRemoveRedundantNodes() const
	{
		return value(QString(REMOVE_REDUNDANT_NODES.c_str()), repoDefaultRemoveRedundantNodes).toBool();
	}

    /*!
     * Returns a space-separated string of node names to be skipped set in
     * settings. Defaults to empty string.
     */
    std::string getRemoveRedundantNodesSkip() const
    { 
		return value(QString(REMOVE_REDUNDANT_NODES_SKIP.c_str()), QString(repoDefaultRemoveRedundantNodesSkip.c_str())).toString().toStdString();
	}

    /*!
     * Returns true if IFC Space representation should be skipped
     * Defaults to true
     */
    bool getSkipIFCSpaceRepresentation() const
    {
        return value(QString(IFC_SKIP_SPACE_REPRESENTATIONS.c_str()), true).toBool();
    }

    /*!
     * Returns ture if sort and remove is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool getSortAndRemove() const
	{
		return value(QString(SORT_AND_REMOVE.c_str()), repoDefaultSortAndRemove).toBool();
	}

    /*!
     * Returns true if sort and remove lines is checked in settings, false
     * otherwise. Defaults to true.
     */
    bool getSortAndRemoveLines() const
	{
		return value(QString(SORT_AND_REMOVE_LINES.c_str()), repoDefaultSortAndRemoveLines).toBool();
	}

    /*!
     * Returns true if sort and remove points is checked in settings, false
     * otherwise. Defaults to true.
     */
    bool getSortAndRemovePoints() const
	{
		return value(QString(SORT_AND_REMOVE_POINTS.c_str()), repoDefaultSortAndRemovePoints).toBool();
	}

    /*!
     * Returns true if sort and remove points is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSortAndRemovePolygons() const
	{
		return value(QString(SORT_AND_REMOVE_POLYGONS.c_str()), repoDefaultSortAndRemovePolygons).toBool();
	}

    /*!
     * Returns true if sort and remove triangles is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSortAndRemoveTriangles() const
	{
		return value(QString(SORT_AND_REMOVE_TRIANGLES.c_str()), repoDefaultSortAndRemoveTriangles).toBool();
	}

    /*!
     * Returns true if split by bone count is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSplitByBoneCount() const
	{
		return value(QString(SPLIT_BY_BONE_COUNT.c_str()), repoDefaultSplitByBoneCount).toBool();
	}

    /*!
     * Returns a split by bones count max bones set in settings. Defaults to
     * AI_SBBC_DEFAULT_MAX_BONES = 60.
     */
    int getSplitByBoneCountMaxBones() const
	{
		return value(QString(SPLIT_BY_BONE_COUNT_MAX_BONES.c_str()), repoDefaultSplitByBoneCountMaxBones).toInt();
	}

    /*!
     * Returns true if split large meshes is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getSplitLargeMeshes() const
	{
		return value(QString(SPLIT_LARGE_MESHES.c_str()), repoDefaultSplitLargeMeshes).toBool();
	}

    /*!
     * Returns a split large meshes triangle limit set in settings. Defaults to
     * AI_SLM_DEFAULT_MAX_TRIANGLES = 1,000,000.
     */
    int getSplitLargeMeshesTriangleLimit() const
	{
		return value(QString(SPLIT_LARGE_MESHES_TRIANGLE_LIMIT.c_str()), repoDefaultSplitLargeMeshesTriangleLimit).toInt();
	}

    /*!
     * Returns a split large meshes triangle limit set in settings. Defaults to
     * AI_SLM_DEFAULT_MAX_VERTICES = 1,000,000.
     */
    int getSplitLargeMeshesVertexLimit() const
	{ return value(QString(SPLIT_LARGE_MESHES_VERTEX_LIMIT.c_str()),
		repoDefaultSplitLargeMeshesVertexLimit).toInt();
	}

    /*!
     * Returns true if triangulate is checked in settings, false otherwise.
     * Defaults to true.
     */
    bool getTriangulate() const
	{
		return value(QString(TRIANGULATE.c_str()), repoDefaultTriangulate).toBool();
	}

    /*!
     * Returns true if validate data structures is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getValidateDataStructures() const
    {
        return value(QString(VALIDATE_DATA_STRUCTURES.c_str()), repoDefaultValidateDataStructures).toBool();
    }


    /*!
     * Returns true if using IFC open shell is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getUseIFCOpenShell() const
    {
        return value(QString(USE_IFC_OPEN_SHELL.c_str()), false).toBool();
    }

    /*!
     * Returns true if element filtering is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getUseElementsFiltering() const
    {
        return value(QString(IOS_USE_FILTER.c_str()), repoDefaultIOSUseFilter).toBool();
    }


    /*!
     * Returns true if exclusion element filtering is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool getIsExclusionFilter() const
    {
        return value(QString(IOS_FILTER_EXCLUSION.c_str()), repoDefaultIsExclusion).toBool();
    }

    /*!
     * Returns list of keywords being filtered
     */
    std::vector<std::string> getFilteringKeywords() const
    {
        QVariantList list;
        for(const auto &key : repoDefaultIfcOpenShellFilterList)
        {
            list.push_back(QString::fromStdString((key)));
        }

        QVariantList retlist =  value(QString(IOS_FILTER_LIST.c_str()), list).toList();

        std::vector<std::string> vec;
        for(const auto &l : retlist)
        {
            vec.push_back(l.toString().toStdString());
        }
        return vec;

    }


public :


    //! Sets the calculate tangent space to settings.
    void setCalculateTangentSpace(bool on)
    { QSettings::setValue(QString(CALCULATE_TANGENT_SPACE.c_str()), on); }

    void setCalculateTangentSpaceMaxSmoothingAngle(float angle)
	{
		QSettings::setValue(QString(CALCULATE_TANGENT_SPACE_MAX_SMOOTHING_ANGLE.c_str()), angle);
	}

    //! Sets the convert to UV coordinates to settings.
    void setConvertToUVCoordinates(bool on)
	{
		QSettings::setValue(QString(CONVERT_TO_UV_COORDINATES.c_str()), on);
	}

    //! Sets the degenerates to points and lines to settings.
    void setDegeneratesToPointsLines(bool on)
	{
		QSettings::setValue(QString(DEGENERATES_TO_POINTS_LINES.c_str()), on);
	}

    //! Sets the debone to settings.
	void setDebone(bool on) { QSettings::setValue(QString(DEBONE.c_str()), on); }

    //! Sets the debone threshold to settings.
    void setDeboneThreshold(float t)
	{
		QSettings::setValue(QString(DEBONE_THRESHOLD.c_str()), t);
	}

    void setDeboneOnlyIfAll(bool on)
	{
		QSettings::setValue(QString(DEBONE_ONLY_IF_ALL.c_str()), on);
	}

    void setFindInstances(bool on)
	{
		QSettings::setValue(QString(FIND_INSTANCES.c_str()), on);
	}

    void setFindInvalidData(bool on)
	{
		QSettings::setValue(QString(FIND_INAVLID_DATA.c_str()), on);
	}

    void setFindInvalidDataAnimationAccuracy(float f)
	{
		QSettings::setValue(QString(FIND_INAVLID_DATA_ANIMATION_ACCURACY.c_str()), f);
	}

    void setFixInfacingNormals(bool on)
	{
		QSettings::setValue(QString(FIX_INFACING_NORMALS.c_str()), on);
	}

    void setFlipUVCoordinates(bool on)
	{
		QSettings::setValue(QString(FLIP_UV_COORDINATES.c_str()), on);
	}

    void setFlipWindingOrder(bool on)
	{
		QSettings::setValue(QString(FLIP_WINDING_ORDER.c_str()), on);
	}

    void setGenerateNormals(bool on)
	{
		QSettings::setValue(QString(GENERATE_NORMALS.c_str()), on);
	}

    void setGenerateNormalsFlat(bool on)
	{
		QSettings::setValue(QString(GENERATE_NORMALS_FLAT.c_str()), on);
	}

    void setGenerateNormalsSmooth(bool on)
	{
		QSettings::setValue(QString(GENERATE_NORMALS_SMOOTH.c_str()), on);
	}

    void setGenerateNormalsSmoothCreaseAngle(float f)
	{
		QSettings::setValue(QString(GENERATE_NORMALS_SMOOTH_CREASE_ANGLE.c_str()), f);
	}

    void setImproveCacheLocality(bool on)
	{
		QSettings::setValue(QString(IMPROVE_CACHE_LOCALITY.c_str()), on);
	}

    void setImproveCacheLocalityCacheSize(int vertexCount)
	{
		QSettings::setValue(QString(IMPROVE_CACHE_LOCALITY_VERTEX_CACHE_SIZE.c_str()), vertexCount);
	}

    void setJoinIdenticalVertices(bool on)
	{
		QSettings::setValue(QString(JOIN_IDENTICAL_VERTICES.c_str()), on);
	}

    void setLimitBoneWeights(bool on)
	{
		QSettings::setValue(QString(LIMIT_BONE_WEIGHTS.c_str()), on);
	}

    void setLimitBoneWeightsMaxWeights(int i)
	{
		QSettings::setValue(QString(LIMIT_BONE_WEIGHTS_MAX_WEIGHTS.c_str()), i);
	}

    void setMakeLeftHanded(bool on)
	{
		QSettings::setValue(QString(MAKE_LEFT_HANDED.c_str()), on);
	}

    void setOptimizeMeshes(bool on)
	{
		QSettings::setValue(QString(OPTIMIZE_MESHES.c_str()), on);
	}

    void setPreTransformUVCoordinates(bool on)
	{
		QSettings::setValue(QString(PRE_TRANSFORM_UV_COORDINATES.c_str()), on);
	}

    void setPreTransformVertices(bool on)
	{
		QSettings::setValue(QString(PRE_TRANSFORM_VERTICES.c_str()), on);
	}

    void setPreTransforVerticesNormalize(bool on)
	{
		QSettings::setValue(QString(PRE_TRANSFORM_VERTICES_NORMALIZE.c_str()), on);
	}

    void setRemoveComponents(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS.c_str()), on);
	}

    void setRemoveComponentsAnimations(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_ANIMATIONS.c_str()), on);
	}

    void setRemoveComponentsBiTangents(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_BI_TANGENTS.c_str()), on);
	}

    void setRemoveComponentsBoneWeights(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_BONE_WEIGHTS.c_str()), on);
	}

    void setRemoveComponentsCameras(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_CAMERAS.c_str()), on);
	}

    void setRemoveComponentsColors(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_COLORS.c_str()), on);
	}

    void setRemoveComponentsLights(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_LIGHTS.c_str()), on);
	}

    void setRemoveComponentsMaterials(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_MATERIALS.c_str()), on);
	}

    void setRemoveComponentsMeshes(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_MESHES.c_str()), on);
	}

    void setRemoveComponentsNormals(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_NORMALS.c_str()), on);
	}

    void setRemoveComponentsTextures(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_TEXTURES.c_str()), on);
	}

    void setRemoveComponentsTextureCoordinates(bool on)
	{
		QSettings::setValue(QString(REMOVE_COMPONENTS_TEXTURE_COORDINATES.c_str()), on);
	}

    void setRemoveRedundantMaterials(bool on)
	{
		QSettings::setValue(QString(REMOVE_REDUNDANT_MATERIALS.c_str()), on);
	}

    void setRemoveRedundantMaterialsSkip(const QString& skip)
	{
		QSettings::setValue(QString(REMOVE_REDUNDANT_MATERIALS_SKIP.c_str()), skip);
	}

    void setRemoveRedundantNodes(bool on)
	{
		QSettings::setValue(QString(REMOVE_REDUNDANT_NODES.c_str()), on);
	}

    void setRemoveRedundantNodesSkip(const QString& skip)
	{
		QSettings::setValue(QString(REMOVE_REDUNDANT_NODES_SKIP.c_str()), skip);
	}

    void setSkipIFCSpaceRepresentation(bool on)
    {
        QSettings::setValue(QString(IFC_SKIP_SPACE_REPRESENTATIONS.c_str()), on);
    }

    void setSortAndRemove(bool on)
	{
		QSettings::setValue(QString(SORT_AND_REMOVE.c_str()), on);
	}

    void setSortAndRemoveLines(bool on)
	{
		QSettings::setValue(QString(SORT_AND_REMOVE_LINES.c_str()), on);
	}

    void setSortAndRemovePoints(bool on)
	{
		QSettings::setValue(QString(SORT_AND_REMOVE_POINTS.c_str()), on);
	}

    void setSortAndRemovePolygons(bool on)
	{
		QSettings::setValue(QString(SORT_AND_REMOVE_POLYGONS.c_str()), on);
	}

    void setSortAndRemoveTriangles(bool on)
	{
		QSettings::setValue(QString(SORT_AND_REMOVE_TRIANGLES.c_str()), on);
	}

    void setSplitByBoneCount(bool on)
	{
		QSettings::setValue(QString(SPLIT_BY_BONE_COUNT.c_str()), on);
	}

    void setSplitByBoneCountMaxBones(int max)
	{
		QSettings::setValue(QString(SPLIT_BY_BONE_COUNT_MAX_BONES.c_str()), max);
	}

    void setSplitLargeMeshes(bool on)
	{
		QSettings::setValue(QString(SPLIT_LARGE_MESHES.c_str()), on);
	}

    void setSplitLargeMeshesTriangleLimit(int limit)
	{
		QSettings::setValue(QString(SPLIT_LARGE_MESHES_TRIANGLE_LIMIT.c_str()), limit);
	}

    void setSplitLargeMeshesVertexLimit(int limit)
	{
		QSettings::setValue(QString(SPLIT_LARGE_MESHES_VERTEX_LIMIT.c_str()), limit);
	}

    void setTriangulate(bool on)
	{
		QSettings::setValue(QString(TRIANGULATE.c_str()), on);
	}

    void setValidateDataStructures(bool on)
	{
		QSettings::setValue(QString(VALIDATE_DATA_STRUCTURES.c_str()), on);
	}

    //IFC OpenShell Setters
    void setUseIFCOpenShell(bool on)
    {
        QSettings::setValue(QString(USE_IFC_OPEN_SHELL.c_str()), on);
    }

    void setUseElementsFiltering(bool on)
    {
        QSettings::setValue(QString(IOS_USE_FILTER.c_str()), on);
    }

    void setIsExclusionFilter(bool on)
    {
        QSettings::setValue(QString(IOS_FILTER_EXCLUSION.c_str()), on);
    }

    void setFilteringKeywords(const std::vector<std::string> &keywords)
    {
        QVariantList list;
        for (const auto &key : keywords)
        {
            list.push_back(QString::fromStdString(key));
        }
        QSettings::setValue(QString(IOS_FILTER_LIST.c_str()), list);
    }


};


} // end namespace gui
} // end namespace repo

