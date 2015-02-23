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

#include "repo_transcoder_graph_scene.h"
#include "repo_transcoder_helpers.h"
#include <iostream>
#include <glc_factory.h>
#include "../primitives/repo_glccamera.h"

namespace repo {
namespace gui {

//-----------------------------------------------------------------------------
//
// GLC World
//
//-----------------------------------------------------------------------------
GLC_World RepoTranscoderGraphScene::toGLCWorld(
    const core::RepoGraphScene* graph,
	const std::map<std::string, QImage> &textures,
	const std::string &namePrefix)
{
    std::cout << "Converting RepoGraphScene to GLC World" << std::endl;
    //-------------------------------------------------------------------------
    // Temporary textures
    // TODO: a texture manager of some kind is needed to keep track of the allocations
    QHash<QString, GLC_Texture> glcTextures;
    for (std::map<std::string, QImage>::const_iterator it = textures.begin();
        it != textures.end(); ++it)
    {
        const QString name(it->first.c_str());
        if (!it->second.isNull())
            glcTextures.insert(name, GLC_Texture(it->second, name));
    }

    //-------------------------------------------------------------------------
    // Allocate materials
    std::vector<core::RepoNodeAbstract *> materials = graph->getMaterials();
    QVector<GLC_Material *> glcMaterials(materials.size());
    for (unsigned int i = 0; i < materials.size(); ++i){
        glcMaterials[i] = toGLCMaterial(
                materials[i],
                glcTextures);
    }

    //-------------------------------------------------------------------------
    // Allocate meshes
    std::vector<core::RepoNodeAbstract*> meshes = graph->getMeshesVector();
    std::map<boost::uuids::uuid, GLC_3DRep*> glcMeshes;
//    QVector<GLC_3DRep*> glcMeshes(meshes.size());
    for (unsigned int i = 0; i < meshes.size(); ++i){
        core::RepoNodeAbstract* node = meshes[i];
        glcMeshes[node->getUniqueID()] = toGLCMesh(meshes[i], glcMaterials, namePrefix);
    }

    //-------------------------------------------------------------------------
    // Allocate cameras
    std::vector<core::RepoNodeAbstract *> cameras = graph->getCameras();
    QHash<const QString, GLC_3DRep*> glcCameras;
    for (unsigned int i = 0; i < cameras.size(); ++i)
        glcCameras.insert(
            QString(cameras[i]->getName().c_str()),
            toGLCCamera(cameras[i]));

    //-------------------------------------------------------------------------
    // Recursively build the scene graph
    GLC_World world;
    if (graph->getRoot())
    {
        GLC_World * tempWorld = new GLC_World(
            createOccurrenceFromNode(
                graph,
                graph->getRoot(),
                glcMeshes,
                glcCameras));
        tempWorld->setRootName(QString(graph->getRoot()->getName().c_str()));

        //---------------------------------------------------------------------
        // Clean and update positions
        tempWorld->rootOccurence()->removeEmptyChildren();
        tempWorld->rootOccurence()->updateChildrenAbsoluteMatrix();

        world = GLC_World(*tempWorld);
        tempWorld->clear();
        delete tempWorld;
    }

    //-------------------------------------------------------------------------
    // Clean up temporary meshes
    for(std::map<boost::uuids::uuid, GLC_3DRep*>::iterator it = glcMeshes.begin(); it != glcMeshes.end(); ++it){
        delete it->second;
    }
    glcMeshes.clear();

    //-------------------------------------------------------------------------
    // Clean up temporary wireframe cameras
    QHash<const QString, GLC_3DRep*>::iterator it = glcCameras.begin();
    while (glcCameras.constEnd() != it)
    {
        delete it.value();
        ++it;
    }
    glcCameras.clear();

	return world;
}

GLC_StructOccurence* RepoTranscoderGraphScene::createOccurrenceFromNode(
    const core::RepoGraphScene* scene,
    const core::RepoNodeAbstract* node,
    const std::map<boost::uuids::uuid, GLC_3DRep*>& glcMeshes,
    const QHash<const QString, GLC_3DRep*>& glcCameras)
{
    Q_ASSERT (NULL != node);
    QString name(node->getName().c_str());
	GLC_StructInstance* instance = NULL;
	GLC_StructOccurence* occurrence = NULL;	

    // If the node is a transform
    if(!node->getType().compare(REPO_NODE_TYPE_TRANSFORMATION)){
        const core::RepoNodeTransformation* nodeTransform = dynamic_cast<const core::RepoNodeTransformation*>(node);
        if(nodeTransform){
            aiMatrix4x4 transform = nodeTransform->getMatrix();
            instance = new GLC_StructInstance(new GLC_StructReference(name));
            instance->move(toGLCMatrix(transform));
        }
    }
    // If the node is a camera
    else if(!node->getType().compare(REPO_NODE_TYPE_CAMERA)){
        // Check if the node points to a camera and if so, take its wireframe representation
        QHash<const QString, GLC_3DRep*>::const_iterator it = glcCameras.find(name);
        if (it != glcCameras.end()){
            instance = new GLC_StructInstance(new GLC_StructReference(new GLC_3DRep(*(it.value()))));
        }
        instance->setName(name);
    }
    // If the node is a mesh
    else if(!node->getType().compare(REPO_NODE_TYPE_MESH)){
        GLC_3DRep* pRep = NULL;

        const std::map<boost::uuids::uuid, GLC_3DRep*>::const_iterator it = glcMeshes.find(node->getUniqueID());
        if(it != glcMeshes.end()){
            GLC_3DRep * glcMesh = it->second;
            if (glcMesh) {
                pRep = new GLC_3DRep(*glcMesh);
            }
        }


        if (NULL != pRep)
        {
            pRep->clean();
            if (pRep->isEmpty())
            {
                std::cerr << "Empty geometry in node " << name.toStdString() << std::endl;
                delete pRep;
                pRep = 0;
                instance = new GLC_StructInstance(new GLC_StructReference(name));
            }
            else
                instance = new GLC_StructInstance(new GLC_StructReference(pRep));

            // Save instance in instance hash Table
            //m_StructInstanceHash.insert(pNode->m_Id, pInstance);
        }
        else
        {
            std::cerr << "NULL geometry in node " << name.toStdString() << std::endl;
            instance = new GLC_StructInstance(new GLC_StructReference(name));
        }
    }
    // Anything else
    else{
        instance = new GLC_StructInstance(new GLC_StructReference(name));
        instance->setName(name);
    }

    occurrence = new GLC_StructOccurence(instance);
    occurrence->setName(name);

    //-------------------------------------------------------------------------
    // Children
    std::set<const core::RepoNodeAbstract*> children = node->getChildren();
    if (!children.empty())
    {
        for(std::set<const core::RepoNodeAbstract*>::const_iterator it = children.begin(); it != children.end(); ++it){
            occurrence->addChild(
                        createOccurrenceFromNode(
                            scene,
                            *it,
                            glcMeshes,
                            glcCameras));
        }
    }

    return occurrence;
}

GLC_Material* RepoTranscoderGraphScene::toGLCMaterial(core::RepoNodeAbstract * node,
                                                                  const QHash<QString, GLC_Texture>& glcTextures)
{
    core::RepoNodeMaterial* material = dynamic_cast<core::RepoNodeMaterial*>(node);

    if(!node)
        return NULL;

    GLC_Material * glcMaterial = new GLC_Material;
    aiColor3D c;

    //-------------------------------------------------------------------------
    // Name
    glcMaterial->setName(QString(material->getName().c_str()));

    //-------------------------------------------------------------------------
    // Diffuse
    if (material->hasDiffuse());
        glcMaterial->setDiffuseColor(toQColor(material->getDiffuse()));

    //-------------------------------------------------------------------------
    // Specular
    if(material->hasSpecular()){
        float strength = material->hasShininessStrength() ? material->getShininessStrength() : 1.0f;
        glcMaterial->setSpecularColor(toQColor(material->getSpecular(), strength));
    }

    if(material->hasShininess()){
        glcMaterial->setShininess(std::min(material->getShininess()/128.0, 1.0));
    }

    //-------------------------------------------------------------------------
    // Ambient
    if (material->hasAmbient())
        glcMaterial->setAmbientColor(toQColor(material->getAmbient()));

    //-------------------------------------------------------------------------
    // Emissive
    if (material->hasEmissive())
        glcMaterial->setEmissiveColor(toQColor(material->getEmissive()));

    //-------------------------------------------------------------------------
    // Opacity
    if(material->hasOpacity()){
        glcMaterial->setOpacity(material->getOpacity());
    }

    //--------------------------------------------------------------------------
    // Diffuse texture
    // TODO: remove the memory leak, use texture manager
    std::set<const core::RepoNodeAbstract *> children = material->getChildren();

    for (std::set<const core::RepoNodeAbstract *>::iterator childrenIt = children.begin(); childrenIt!=children.end(); ++childrenIt)
    {
        const core::RepoNodeAbstract* node = *childrenIt;

        if(node){
            const QString name(node->getName().c_str());

            QHash<QString, GLC_Texture>::const_iterator it = glcTextures.find(name);
            if (glcTextures.end() != it)
            {
                glcMaterial->setTexture(new GLC_Texture(it.value()));
                break;
            }
        }
    }

    return glcMaterial;
}

GLC_3DRep* RepoTranscoderGraphScene::toGLCMesh(core::RepoNodeAbstract* node, const QVector<GLC_Material*>& materials,
    const std::string &namePrefix)
{
    core::RepoNodeMesh* mesh = dynamic_cast<core::RepoNodeMesh*>(node);

    if(!mesh)
        return NULL;

    GLC_Mesh * glcMesh = new GLC_Mesh;
    std::string name = namePrefix + mesh->getName();
    glcMesh->setName(QString::fromStdString(name));

    //-----------------------------------------------------------------
    // Vertices
    const std::vector<aiVector3t<float> >* vertices = mesh->getVertices();

    QVector<GLfloat> glcVertices;

    if(vertices){
        glcVertices = toGLCVector(
            &(*vertices)[0],
            vertices->size(),
            XYZ);
        glcMesh->addVertice(glcVertices);
    }

    //-----------------------------------------------------------------
    // Normals
    const std::vector<aiVector3t<float> >* normals = mesh->getNormals();

    if(normals){
        glcMesh->addNormals(
            toGLCVector(
                &(*normals)[0],
                normals->size(),
                XYZ));
    }

    //-----------------------------------------------------------------
    // Vertex colours
    const std::vector<aiColor4t<float> >* colors = mesh->getColors();

    if(colors){
        glcMesh->setColorPearVertex(true);
        glcMesh->addColors(toGLCVector(
            &(*colors)[0], colors->size()));
    }

    //-------------------------------------------------------------------------
    // Faces (triangles)
    const std::vector<aiFace>* faces = mesh->getFaces();

    if(faces){
        std::set<const core::RepoNodeAbstract *> children = mesh->getChildren();

        // We are looking for the material that is attributed to the mesh
        GLC_Material* meshMaterial = NULL;

        // Assumption: there will be a child node with the same name as on of the GLC_Material

        for (std::set<const core::RepoNodeAbstract *>::iterator childrenIt = children.begin(); childrenIt != children.end(); ++childrenIt)
        {
            const core::RepoNodeAbstract* node = *childrenIt;

            if(node){
                // Search for the correct name
                const QString name(node->getName().c_str());
                for(QVector<GLC_Material*>::const_iterator it = materials.begin(); it != materials.end(); ++it){
                    GLC_Material* mat =*it;
                    if(mat && !mat->name().compare(name)){
                        meshMaterial = mat;
                        break;
                    }
                }
            }
            if(meshMaterial)
                break;
        }

        // What if meshMaterial is null?

        glcMesh->addTriangles(
            meshMaterial,
            toGLCList(
                glcVertices.toList(),
                &(*faces)[0],
                faces->size()));

        if(vertices){
            //---------------------------------------------------------------------
            // Wireframe
            // Since GLC_Lib renders only triangles, the wireframe for polygon
            // faces has to be created separately.
            GLfloatVector faceVertices;
            aiVector3D vertex;
            for (unsigned int i = 0; i < faces->size(); ++i)
            {
                for (unsigned int j = 0; j < faces->at(i).mNumIndices; ++j)
                {
                    vertex = vertices->at( faces->at(i).mIndices[j]);
                    faceVertices << vertex.x << vertex.y << vertex.z;
                }
                glcMesh->addVerticeGroup(faceVertices);
                faceVertices.clear();
            }
        }
    }

    //-------------------------------------------------------------------------
    // Texture coordinates
    // Assimp provides multiple textures per vertex eg U, UV or UVW
    const std::vector<aiVector3t<float> >* texcoord = mesh->getUVChannel();

    if (texcoord){
        glcMesh->addTexels(toGLCVector(
            &(*texcoord)[0],
            texcoord->size(),
            XY));
    }

    //-----------------------------------------------------------------
    // Copy index list in a vector for Vertex Array Use
    glcMesh->finish();
    GLC_3DRep* pRep = new GLC_3DRep(glcMesh);
    glcMesh = NULL;
    pRep->clean();

    return pRep;
}

GLC_3DRep * RepoTranscoderGraphScene::toGLCCamera(core::RepoNodeAbstract* node)
{
    core::RepoNodeCamera* camera = dynamic_cast<core::RepoNodeCamera*>(node);

    if(!camera)
        return NULL;

    GLC_Point3d position = toGLCPoint(camera->getPosition());

    aiMatrix4x4 matrix = camera->getCameraMatrix();
    GLC_Matrix4x4 glcMatrix = toGLCMatrix(matrix);

    float halfWidth = 0.5; //std::tan(assimpCamera->mHorizontalFOV) / depth;
    float halfHeight = halfWidth / (camera->getAspectRatio() == 0 ?  1 : camera->getAspectRatio() );
    float depth = halfWidth*2 / std::tan(camera->getFieldOfView());

    // Camera wireframe representation
    GLC_3DRep * glccamera = new GLC_3DRep(new RepoGLCCamera(position, depth, halfWidth, halfHeight, glcMatrix));
    std::string name = node->getName();
    glccamera->setName(QString::fromStdString(name));
    return glccamera;
}

} // end namespace gui
} // end namespace repo
