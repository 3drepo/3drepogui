/**
*  Copyright (C) 2014 3D Repo Ltd
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

//------------------------------------------------------------------------------
// GUI
#include "repo_worker_glc_export.h"

#include "../logger/repo_logger.h"
#include <maths/glc_geomtools.h>

#include <sstream>

using namespace repo::worker;
namespace repoModel = repo::core::model;

//------------------------------------------------------------------------------
// Core

//------------------------------------------------------------------------------


GLCExportWorker::GLCExportWorker(
    repo::core::model::RepoScene* scene,
        const std::vector<double> &offsetVector):
    scene(scene),
    offsetVector(offsetVector)
{
    //use stashGraph if available, otherwise use default
    if (scene && scene->getRoot(repo::core::model::RepoScene::GraphType::OPTIMIZED))
    {
        repoDebug << "stash graph found, visualising with stash graph...";
    }
    else
    {
        repoDebug << "stash graph not found, visualising with default graph...";
    }

    qRegisterMetaType<std::map<QString, GLC_Mesh*>>("std::map<QString, GLC_Mesh*>&");
    qRegisterMetaType<std::map<QString, GLC_Material*>>("std::map<QString, GLC_Material*>&");
    qRegisterMetaType<std::vector<GLC_Light*>>("std::vector<GLC_Light*>&");

}

GLCExportWorker::~GLCExportWorker() {}

void GLCExportWorker::run()
{
	repoLog("Running GLC Export worker..");

	if (!cancelled && scene && scene->getRoot(scene->getViewGraph()))
    {
        repo::core::model::RepoScene::GraphType repoViewGraph = scene->getViewGraph();
        //-------------------------------------------------------------------------
        // Start
        jobsCount = scene->getItemsInCurrentGraph(repoViewGraph);
        done = 0;
        emit progress(0, 0); // undetermined (moving) progress bar

        std::map<QString, GLC_Mesh*> meshMap;
        std::map<QString, GLC_Material*> matMap;
        std::vector<GLC_Light*> lights;

        GLC_World *glcWorld = createGLCWorld(scene, meshMap, matMap, lights);

        //--------------------------------------------------------------------------
        emit progress(jobsCount, jobsCount);
        QString rootName;

        if (scene)
            rootName = QString(scene->getRoot(repoViewGraph)->getName().c_str());

        if (glcWorld)
        {
            glcWorld->setRootName(rootName);
            //---------------------------------------------------------------------
            // Clean and update positions
            glcWorld->rootOccurrence()->removeEmptyChildren();
            glcWorld->rootOccurrence()->updateChildrenAbsoluteMatrix();
        }

        GLC_World wholeGraph = glcWorld ? GLC_World(*glcWorld) : GLC_World();
        if (glcWorld)
        {
            glcWorld->clear();
            delete glcWorld;
        }
		else
		{
			repoLogError("GLC World is null! The widget will fail to render this model");
		}
        //--------------------------------------------------------------------------

		if (!cancelled)
        {
            repoLog("Completed world. emitting signals... #lights: " + std::to_string(lights.size()));
            emit finished(wholeGraph, meshMap, matMap, lights);
        }
    }
    else{
        repoLog("Trying to produce a GLC representation with a nullptr to scene!");
    }
    emit RepoAbstractWorker::finished();
}

GLC_Light* GLCExportWorker::convertGLCLight(
         const repo::core::model::LightNode *light,
        const unsigned int                  &ind
        )
{
    GLC_Light::LightType type;

    if(light)
    {
        switch(light->getLightType())
        {
            //FIXME: Point and ambient  = position?
            case repo::core::model::LightNode::LightType::AMBIENT:
            case repo::core::model::LightNode::LightType::POINT:
                type = GLC_Light::LightType::LightPosition;
                break;
            case repo::core::model::LightNode::LightType::DIRECTIONAL:
                type = GLC_Light::LightType::LightDirection;
                break;
            case repo::core::model::LightNode::LightType::SPOT:
                type = GLC_Light::LightType::LightSpot;
                break;
            default:
                repoLogError("Unknown light type!");
        }
        GLC_Light *resLight = new GLC_Light(type);

        auto ambientColor = light->getAmbientColor();
        QColor amb, dif, spec;

        amb.setRgbF(ambientColor.r, ambientColor.g, ambientColor.b);
        resLight->setAmbientColor(amb);
        auto diffuseColor = light->getDiffuseColor();

        dif.setRgbF(diffuseColor.r, diffuseColor.g, diffuseColor.b);
        resLight->setDiffuseColor(dif);

        auto specularColor = light->getSpecularColor();
        spec.setRgbF(specularColor.r, specularColor.g, specularColor.b);
        resLight->setSpecularColor(spec);

        auto position = light->getPosition();
        resLight->setPosition(GLC_Point3d(position.x, position.y, position.z));

        auto direction = light->getDirection();
        resLight->setSpotDirection(GLC_Point3d(direction.x, direction.y, direction.z));

        resLight->setConstantAttenuation(light->getConstantAttenuation());
        resLight->setLinearAttenuation(light->getLinearAttenuation());
        resLight->setQuadraticAttenuation(light->getQuadraticAttenuation());

        auto angle = light->getInnerConeAngle();

        resLight->setSpotCutoffAngle(angle);
        resLight->setSpotEponent(light->getSpotExponent());
        return resLight;
    }
    else
        return nullptr;



}

void GLCExportWorker::createGLCLights(
    repo::core::model::RepoScene     *scene,
     std::vector<GLC_Light*>          &lights
        )
{
    auto repoLights = scene->getAllLights(scene->getViewGraph());
    if(repoLights.size())
        repoLog("Generating GLC lights...");
    unsigned int ind = 1;
    for(auto &lightNode : repoLights)
    {     
        auto light = static_cast<repo::core::model::LightNode*>(lightNode);
        auto parents = light->getParentIDs();
        repo::core::model::RepoNode* currentNode = light;
        repo::core::model::LightNode currentLight = *light;
        while (parents.size())
        {
            currentNode = scene->getNodeBySharedID(scene->getViewGraph(), parents[0]);
            auto trans = dynamic_cast<repo::core::model::TransformationNode*>(currentNode);
            if (trans)
            {
                currentLight = currentLight.cloneAndApplyTransformation(trans->getTransMatrix(false));
            }
            else
            {
                repoLogError("Unexpected node type: ancestors of light node should always be a transformation, type is: " + currentNode->getType());
            }
            parents = currentNode->getParentIDs();
            if (parents.size() > 1)
                repoLogError("Light is instanced. This is unexpected!");
        }

        //now that we're sure the light node is transformed, create GLCLight
        if(auto lightptr = convertGLCLight(light, ind++))
            lights.push_back(lightptr);

    }
}

GLC_World* GLCExportWorker::createGLCWorld(
    repo::core::model::RepoScene     *scene,
    std::map<QString, GLC_Mesh*>     &meshMap,
    std::map<QString, GLC_Material*> &matMap,
     std::vector<GLC_Light*> &lights
        )
{

    //-----
    GLC_World* glcWorld = nullptr;
    if (!cancelled && scene && scene->hasRoot(scene->getViewGraph())){
        auto occ = convertSceneToOccurance(scene, meshMap, matMap, offsetVector);
        if (occ)
            glcWorld = new GLC_World(occ);
		else
		{
			repoLogError("Unable to create GLC world : Null pointer to occurence");
		}

        createGLCLights(scene, lights);
    }

    return glcWorld;
}


GLC_StructOccurrence* GLCExportWorker::convertSceneToOccurance(
    repo::core::model::RepoScene *scene,
    std::map<QString, GLC_Mesh*>     &meshMap,
    std::map<QString, GLC_Material*> &matMap,
    const std::vector<double> &offsetVector)
{

    repo::core::model::RepoScene::GraphType repoViewGraph = scene->getViewGraph();

    //------------------------------------------------------------------
    // Allocate Textures
    std::map<repoUUID, std::vector<GLC_Texture*>> parentToGLCTexture;

    repoModel::RepoNodeSet textures = scene->getAllTextures(repoViewGraph);

    for (auto &texture : textures)
    {
		if (texture && !cancelled)
        {
            GLC_Texture* glcTexture = convertGLCTexture((repoModel::TextureNode*)texture);
            if (glcTexture)
            {
                std::vector<repoUUID> parents = texture->getParentIDs();
                for (auto &parent : parents)
                {

                    if (parentToGLCTexture.find(parent) == parentToGLCTexture.end())
                    {
                        parentToGLCTexture[parent] = std::vector<GLC_Texture*>();
                    }
                    //Map the texture to all parent UUIDs
                    parentToGLCTexture[parent].push_back(glcTexture);
                }
            }

        }
    }

    //------------------------------------------------------------------
    // Allocate Materials
    std::map<repoUUID, std::vector<GLC_Material*>> parentToGLCMaterial;

    repoModel::RepoNodeSet materials = scene->getAllMaterials(repoViewGraph);

    for (auto &material : materials)
    {
		if (material && !cancelled)
        {
            GLC_Material* glcMat = convertGLCMaterial(
                (repoModel::MaterialNode*)material, parentToGLCTexture);
            if (glcMat)
            {
				if (repoViewGraph == repo::core::model::RepoScene::GraphType::DEFAULT)
				{
					std::vector<repoUUID> parents = material->getParentIDs();
					for (auto &parent : parents)
					{
						if (parentToGLCMaterial.find(parent) == parentToGLCMaterial.end())
						{
							parentToGLCMaterial[parent] = std::vector<GLC_Material*>();
						}
						//Map the material to all parent UUIDs
						parentToGLCMaterial[parent].push_back(glcMat);
					}
				}
				else
				{
					//if stash, use its own unique ID as mapping
					parentToGLCMaterial[material->getUniqueID()] = std::vector<GLC_Material*>();
					parentToGLCMaterial[material->getUniqueID()].push_back(glcMat);
				}
                
            }

        }
    }

    //-------------------------------------------------------------------------
    // Allocate Meshes

    repoModel::RepoNodeSet meshes = scene->getAllMeshes(repoViewGraph);
    std::map<repoUUID, std::vector<GLC_3DRep*>> parentToGLCMeshes;
    for (auto &mesh : meshes)
    {
		if (mesh && !cancelled)
        {
            GLC_3DRep* glcMesh = convertGLCMesh(
                (repoModel::MeshNode*)mesh, parentToGLCMaterial, matMap);
            if (glcMesh)
            {
                std::vector<repoUUID> parents = mesh->getParentIDs();
                for (auto &parent : parents)
                {
                    if (parentToGLCMeshes.find(parent) == parentToGLCMeshes.end())
                    {
                        parentToGLCMeshes[parent] = std::vector<GLC_3DRep*>();
                    }
                    //Map the mesh to all parent UUIDs
                    parentToGLCMeshes[parent].push_back(glcMesh);
                }

                for (int i = 0; i < glcMesh->numberOfBody(); ++i)
                {
                    GLC_Mesh * meshObj = dynamic_cast<GLC_Mesh*>(glcMesh->geomAt(i));

                    if (meshObj)
                    {
                        meshMap[glcMesh->name()] = meshObj;
                    }
                }

            }
        }
    }

    //-------------------------------------------------------------------------
    // Allocate cameras
  //  repoModel::RepoNodeSet cameras = scene->getAllCameras(repoViewGraph);
    std::map<repoUUID, std::vector<GLC_3DRep*>> parentToGLCCameras;
  //  for (auto &camera : cameras)
  //  {
		//if (camera && !cancelled)
  //      {
  //          //FIXME: cameras don't really work. Disabled from visualisation for now.
  //          //GLC_3DRep* glcCamera = convertGLCCamera(
  //          //	(repoModel::CameraNode*)camera);
  //          //if (glcCamera)
  //          //{
  //          //	std::vector<repoUUID> parents = camera->getParentIDs();
  //          //	for (auto &parent : parents)
  //          //	{
  //          //		//Map the material to all parent UUIDs
  //          //		if (parentToGLCCameras.find(parent) == parentToGLCCameras.end())
  //          //		{
  //          //			parentToGLCCameras[parent] = std::vector<GLC_3DRep*>();
  //          //		}
  //          //		//Map the mesh to all parent UUIDs
  //          //		parentToGLCCameras[parent].push_back(glcCamera);
  //          //	}
  //          //}

  //      }
  //  }

    auto rootNode = scene->getRoot(repoViewGraph);
    if(rootNode && offsetVector.size())
    {
        //need to offset the model by the current world coordinates

        auto sceneOffset = scene->getWorldOffset();
        std::vector<double> dOffset = {sceneOffset[0] - offsetVector[0],
                                       sceneOffset[1] - offsetVector[1], sceneOffset[2] - offsetVector[2]};

        std::vector<float> transMat = { 1, 0, 0, (float)dOffset[0],
                                        0, 1, 0, (float)dOffset[1],
                                        0, 0, 1, (float)dOffset[2],
                                        0, 0, 0, 1};
        auto transFormedRoot = rootNode->cloneAndApplyTransformation(transMat);

        repoLogDebug("offsetVector present, shifting the model by "
                 + std::to_string(dOffset[0]) + ", "
                                + std::to_string(dOffset[1]) + ", "
                                + std::to_string(dOffset[2]));

        return createOccurrenceFromNode(scene, &transFormedRoot, parentToGLCMeshes, parentToGLCCameras, meshMap, matMap);

    }
    else
        return createOccurrenceFromNode(scene, rootNode, parentToGLCMeshes, parentToGLCCameras, meshMap, matMap);
}

GLC_StructOccurrence* GLCExportWorker::createOccurrenceFromNode(
    repo::core::model::RepoScene                     *scene,
    const repo::core::model::RepoNode                *node,
    std::map<repoUUID, std::vector<GLC_3DRep*>>      &glcMeshesMap,
    std::map<repoUUID, std::vector<GLC_3DRep*>>      &glcCamerasMap,
    std::map<QString, GLC_Mesh*>                     &meshMap,
    std::map<QString, GLC_Material*>                 &matMap,
            const bool                               &countJob)
{
    /*
    * There are a number of assumptions within this function
    * - Materials/Textures of a mesh lives in the same scene (not in a referenced scene)
    * - Materials are immediate children to the mesh that references it
    * - Textures are immediate children of the material that references it
    * - Meshes are immediate children of transformations
    */

    GLC_StructInstance* instance = nullptr;
    GLC_StructOccurrence* occurrence = nullptr;
    if (node)
    {
        repoModel::NodeType type = node->getTypeAsEnum();
        repoUUID sharedID = node->getSharedID();

        QString name(node->getName().c_str());
        switch (type)
        {
            /**
            * Base on the assumption, there are only 2 types of nodes that we need to deal with
            * TRANSFORMATION - find all its children meshes/cameras and add them into the instance
            * REFERENCE - Create another GLC world out of that referenced scene
            * Meshes/Materials/Textures should be already set up to be dealt with at it's
            * parent transformation.
            * Anything else is (probably) not rendering related(?) and should be ignored
            */
        case repoModel::NodeType::TRANSFORMATION:
        {
            GLC_3DRep* pRep = nullptr;
            std::map<repoUUID, std::vector<GLC_3DRep*>>::iterator it = glcMeshesMap.find(sharedID);
            if (it != glcMeshesMap.end())
            {
                //has meshes
                for (auto &glcMesh : it->second)
                {
					if (glcMesh&& !cancelled)
                    {
                        if (pRep)
                            // instead of merging pRep
                            pRep->merge(glcMesh);
                        else
                            pRep = new GLC_3DRep(*glcMesh);
                    }
                }


                if (pRep)
                {
                    pRep->clean();
                    if (pRep->isEmpty())
                    {
                        repoLog("Empty geometry in node " + name.toStdString());
                        delete pRep;
                        pRep = 0;
                        instance = new GLC_StructInstance(new GLC_StructReference(name));
                    }
                    else{
                        instance = new GLC_StructInstance(new GLC_StructReference(pRep));
                    }
                }
                else
                {
					repoLogError("NULL geometry in node " + name.toStdString());
                    instance = new GLC_StructInstance(new GLC_StructReference(name));
                }
            }
            //else
            //{
            //	//FIXME: If there is a camera AND mesh then this wouldn't support that.
            //	it = glcCamerasMap.find(sharedID);
            //	if (it != glcCamerasMap.end())
            //	{
            //		//has cameras
            //		for (auto &glcCamera : it->second)
            //		{
            //			if (glcCamera)
            //			{

            //				instance = new GLC_StructInstance(new GLC_StructReference(new GLC_3DRep(*glcCamera)));
            //				//if (pRep)
            //				//	// instead of merging pRep
            //				//	pRep->merge(glcCamera);
            //				//else
            //				//	pRep = new GLC_3DRep(*glcCamera);
            //			}

            //		}
            //	}
            //}
            //

            if (!instance)
            {
                instance = new GLC_StructInstance(new GLC_StructReference(name));
            }

            //-------------------------------------------------------------------------
            // Transformation
            auto trans = (repoModel::TransformationNode*) node;
            auto mat = trans->getTransMatrix(true);

			if (mat.size() != 16)
			{
				repoLogError("Matrix size is not 16!!!");
			}
			else
			{				
				GLC_Matrix4x4 transMat(mat.data());
				instance->move(transMat);
			}


            occurrence = new GLC_StructOccurrence(instance);
            occurrence->setName(name);

            break;
        }            
        case repoModel::NodeType::REFERENCE:

            repo::core::model::RepoScene *refScene = scene->getSceneFromReference(scene->getViewGraph(),
        		node->getSharedID());
            repoLog("loading reference scene : " + ((repo::core::model::ReferenceNode*)node)->getProjectName());
            if (refScene &&( (refScene->getAllMeshes(repo::core::model::RepoScene::GraphType::DEFAULT).size() > 0
                || refScene->getAllReferences(repo::core::model::RepoScene::GraphType::DEFAULT).size() > 0)
                || (refScene->getAllMeshes(repo::core::model::RepoScene::GraphType::OPTIMIZED).size() > 0
				|| refScene->getAllReferences(repo::core::model::RepoScene::GraphType::OPTIMIZED).size() > 0)))
        	{
                //There is nothing to visualise if there are no meshes
                occurrence = convertSceneToOccurance(refScene, meshMap, matMap);
        	}
            else
            {
                repoLog("Referenced scene has no referenced nodes or meseh nodes. Skipping...");
            }

        	break;
        }//switch

        //-------------------------------------------------------------------------
        // Children

        for (auto child : scene->getChildrenAsNodes(scene->getViewGraph(), node->getSharedID()))
        {

            GLC_StructOccurrence *childOccurance = createOccurrenceFromNode(
                scene,
                child,
                glcMeshesMap,
                glcCamerasMap,
                meshMap,
                matMap,
                countJob);

            if (childOccurance)
            {
                if (occurrence)
                    occurrence->addChild(childOccurance);
                else
                    occurrence = childOccurance;
            }
        }

    }

    if (countJob)
        emit progressRangeChanged(++done, jobsCount);
    return occurrence;
}

GLC_3DRep* GLCExportWorker::convertGLCCamera(
    const repo::core::model::CameraNode *camera)
{
    GLC_3DRep * glcCamera = nullptr;
//    if (camera)
//    {
//        repo_vector_t position = camera->getPosition();
//        GLC_Point3d glcPosition(position.x, position.y, position.z);

//        GLC_Matrix4x4 glcMatrix(&camera->getCameraMatrix(false).at(0));

//        float halfWidth = 0.5; //std::tan(assimpCamera->mHorizontalFOV) / depth;
//        float aspectRatio = camera->getAspectRatio();

//        float halfHeight = halfWidth / (aspectRatio ? aspectRatio : 1);
//        float depth = halfWidth * 2 / std::tan(camera->getHorizontalFOV());

//        // Camera wireframe representation
//        RepoGLCCamera *repoGLCCam = new RepoGLCCamera(glcPosition, depth, halfWidth, halfHeight, glcMatrix);


//        glcCamera = new GLC_3DRep(repoGLCCam);

//        glcCamera->setName(QString::fromStdString(camera->getName().c_str()));
//    }

    return glcCamera;

}

GLC_Material* GLCExportWorker::convertGLCMaterial(
    const repo::core::model::MaterialNode   *material,
    std::map<repoUUID, std::vector<GLC_Texture*>> &mapTexture)
{
    GLC_Material* glcMat = nullptr;

    if (material)
    {
        glcMat = new GLC_Material();
        glcMat->setName(QString(material->getName().c_str()));
        repo_material_t repoMat = material->getMaterialStruct();

        //-------------------------------------------------------------------------
        // Ambient
        if (repoMat.ambient.size() >= 3)
        {
            QColor ambientCol = toQColor(repoMat.ambient);
            glcMat->setAmbientColor(ambientCol);
        }

        //-------------------------------------------------------------------------
        // Diffuse
        if (repoMat.diffuse.size() >= 3)
        {
            QColor diffuse = toQColor(repoMat.diffuse);
            glcMat->setDiffuseColor(diffuse);
        }

        //-------------------------------------------------------------------------
        // Emissive
        if (repoMat.emissive.size() >= 3)
        {
            QColor emissive = toQColor(repoMat.emissive);
            glcMat->setEmissiveColor(emissive);
        }

        //-------------------------------------------------------------------------
        // Shininess
        if (repoMat.shininess == repoMat.shininess)
        {

            float shininess = std::min(repoMat.shininess / 128.0, 1.0);
            glcMat->setShininess(shininess);

        }

        //-------------------------------------------------------------------------
        // Specular
        float shininessStrength = 1.0;
        if (repoMat.shininessStrength == repoMat.shininessStrength)
            shininessStrength = repoMat.shininessStrength;
        if (repoMat.specular.size() >= 3)
        {
            std::vector<float> specCol = std::vector<float>(repoMat.specular);
            specCol.push_back(shininessStrength);
            QColor specular = toQColor(specCol);
            glcMat->setSpecularColor(specular);
        }

        //-------------------------------------------------------------------------
        // Opacity
        if (repoMat.opacity == repoMat.opacity)
        {
            glcMat->setOpacity(repoMat.opacity);
        }


        //-------------------------------------------------------------------------
        // Diffuse texture
        std::map<repoUUID, std::vector<GLC_Texture*>>::iterator mapIt = mapTexture.find(material->getSharedID());
        if (mapIt != mapTexture.end())
        {
            //FIXME: only takes in 1 texture.
            glcMat->setTexture((GLC_Texture*)mapIt->second.at(0));
        }
    }

    return glcMat;
}

GLC_3DRep* GLCExportWorker::convertGLCMesh(
    const repo::core::model::MeshNode        *mesh,
    std::map<repoUUID, std::vector<GLC_Material*>> &mapMaterials,
    std::map<QString, GLC_Material*> &matMap)
{

	GLC_Mesh * glcMesh = new GLC_Mesh;
    if (mesh)
    {
		std::string name = mesh->getName();
		glcMesh->setName(QString::fromStdString(UUIDtoString(mesh->getUniqueID())));

        std::vector<repo_vector_t> vector3d;
		//Vertices
		vector3d = mesh->getVertices();
		QVector<GLfloat> glcVec = createGLCVector(vector3d);
		if (glcVec.size() > 0)
			glcMesh->addVertice(glcVec);

		//Normals
        std::vector<repo_vector_t> normal3d = mesh->getNormals();
		QVector<GLfloat> glcNorm = createGLCVector(normal3d);
		if (glcNorm.size() > 0)
			glcMesh->addNormals(glcNorm);

		//Colors
        std::vector<repo_color4d_t> colors;
		colors = mesh->getColors();
		QVector<GLfloat> glcCol = createGLCVector(colors);
		if (glcCol.size() > 0)
		{
			glcMesh->setColorPearVertex(true);
			glcMesh->addColors(glcCol);
		}

		//faces
        std::vector<repo_face_t> faces;
		faces = mesh->getFaces();

		auto mapping = mesh->getMeshMapping();
		if (mapping.size() > 0)
		{
			for (const repo_mesh_mapping_t &map : mapping)
			{
				
				QList<GLuint> glcFaces = createGLCFaceList(faces, glcVec, map.triFrom, map.triTo);

				GLC_Material* material = nullptr;
                QString meshId = QString::fromStdString(UUIDtoString(map.mesh_id));
				std::map<repoUUID, std::vector<GLC_Material*>>::iterator mapIt =
					mapMaterials.find(map.material_id);
                if(matMap.find(meshId) != matMap.end())
                {
                    //We've seen this meshId before -> an instance.
                    //apply the same GLC_Material instance
                    material = matMap[meshId];
                }
                else
                {
                    if (mapIt != mapMaterials.end())
                    {
                        material = new GLC_Material(*mapIt->second.at(0));

                        material->setId(glc::GLC_GenID());
                    }
                    else
                    {
                        material = new GLC_Material();
                    }
                    material->setName(meshId);
                    matMap[meshId] = material;
                }

				glcMesh->addTriangles(material, glcFaces);

			}
		}
		else
		{


			QList<GLuint> glcFaces = createGLCFaceList(faces, glcVec);

            QString meshUniqueID = QString::fromStdString(UUIDtoString(mesh->getUniqueID()));
			if (glcFaces.size() > 0)
			{
				GLC_Material* material = nullptr;
				std::map<repoUUID, std::vector<GLC_Material*>>::iterator mapIt =
					mapMaterials.find(mesh->getSharedID());
				if (mapIt != mapMaterials.end())
				{
                    material = new GLC_Material(*mapIt->second.at(0));
				}
                else
                {
                    material = new GLC_Material();

                }
                material->setName(meshUniqueID);
				glcMesh->addTriangles(material, glcFaces);
                matMap[meshUniqueID] = material;
			}
		}


		//---------------------------------------------------------------------
		// Wireframe
		// Since GLC_Lib renders only triangles, the wireframe for polygon
		// faces has to be created separately.
		GLfloatVector faceVertices;
        for (auto &face : faces)
		{
            for (uint32_t j = 0; j < face.size(); ++j)
			{
				//FIXME: this is assuming order in assimp's mVertice = vector3d's order
                repo_vector_t vertex = vector3d[face[j]];
				faceVertices << vertex.x << vertex.y << vertex.z;
			}

			glcMesh->addVerticeGroup(faceVertices);
			faceVertices.clear();
		}
		

        std::vector<repo_vector2d_t> uvVectors = mesh->getUVChannels();
		QVector<GLfloat> glcUVVec = createGLCVector(uvVectors);

		if (glcUVVec.size() > 0)
		{
			glcMesh->addTexels(glcUVVec);
		}

		glcMesh->finish();

    }

	GLC_3DRep* pRep = new GLC_3DRep(glcMesh);
	glcMesh = NULL;
	pRep->clean();
	return pRep;
}


QList<GLuint> GLCExportWorker::createGLCFaceList(
    const std::vector<repo_face_t> &faces,
    const QVector<GLfloat>         &vertices,
	const int32_t &start,
	const int32_t &end)
{
    QList<GLuint> glcList;
    if (faces.size())
    {
		int32_t startInd = start == -1 ? 0 : start;
        int32_t endInd = end == -1 ? faces.size() : end;
		for (int i = startInd; i < endInd; ++i)
        {
            QList<GLuint> glcFaceIndices;
            glcFaceIndices.reserve(faces[i].size());
            //---------------------------------------------------------------------
            // Copy all assimp indices of a single face to a QList
            std::copy(
                faces[i].data(),
                faces[i].data() + faces[i].size(),
                std::back_inserter(glcFaceIndices));


            //---------------------------------------------------------------------
            // GLC 2.5.0 can render only up to triangles,
             //hence triangulate all non-tri faces
            if (glcFaceIndices.size() > 3)
                glc::triangulatePolygon(&glcFaceIndices, vertices.toList());

            glcList.append(glcFaceIndices);

        }
    }

    return glcList;
}

GLC_Texture* GLCExportWorker::convertGLCTexture(
    const repo::core::model::TextureNode *texture)
{
    GLC_Texture* glcTexture = nullptr;

    if (texture)
    {
        std::vector<char> data = texture->getRawData();

        if (data.size())
        {
            QImage image = QImage::fromData((uchar*)data.data(), data.size()*sizeof(char));
            glcTexture = new GLC_Texture(image, QString(texture->getName().c_str()));
        }
    }

    return glcTexture;
}

QVector<GLfloat> GLCExportWorker::createGLCVector(
    const std::vector<repo_color4d_t> &col
    )
{
    QVector<GLfloat> glcVector;

    if (col.size())
    {
        glcVector.resize(col.size() * 4); //repo_color_t always have 4 values
        int ind = 0;
        for (auto &mem : col)
        {
            glcVector[ind++] = (GLfloat)mem.r;
            glcVector[ind++] = (GLfloat)mem.g;
            glcVector[ind++] = (GLfloat)mem.b;
            glcVector[ind++] = (GLfloat)mem.a;
        }
    }

    return glcVector;
}

QVector<GLfloat> GLCExportWorker::createGLCVector(
    const std::vector<repo_vector_t> &vec
    )
{
    QVector<GLfloat> glcVector;

    if (vec.size())
    {
		
		//FIXME: since repo_vector_t is a struct, can't we just memcpy?
        glcVector.resize(vec.size() * 3); //repo_vector_t always have 3 values
        int ind = 0;
        for (const auto &v : vec)
        {
            glcVector[ind++] = (GLfloat)v.x;
			glcVector[ind++] = (GLfloat)v.y;
			glcVector[ind++] = (GLfloat)v.z;
        }
    }

    return glcVector;
}


QVector<GLfloat> GLCExportWorker::createGLCVector(
    const std::vector<repo_vector2d_t> &vec
    )
{
    QVector<GLfloat> glcVector;

    if (vec.size())
    {
        glcVector.resize(vec.size() * 2); //repo_vector_t always have 3 values
        int ind = 0;
        for (const auto &v : vec)
        {
			glcVector[ind++] = (GLfloat)v.x;
			glcVector[ind++] = (GLfloat)v.y;
        }
    }

    return glcVector;
}

QColor GLCExportWorker::toQColor(const std::vector<float> &c, float scale)
{
    if (c.size() == 3)
        return QColor(
            std::min(int(c[0] * scale * 255), 255),
            std::min(int(c[1] * scale * 255), 255),
            std::min(int(c[2] * scale * 255), 255));
    if (c.size() == 4)
        return QColor(
        std::min(int(c[0] * scale * 255), 255),
        std::min(int(c[1] * scale * 255), 255),
        std::min(int(c[2] * scale * 255), 255),
        std::min(int(c[3] * scale * 255), 255));

    return QColor();

}
