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

#include "repo_transcoder_assimp.h"
#include <iostream>
#include <glc_factory.h>
#include "../primitives/repo_glccamera.h"

//-----------------------------------------------------------------------------
//
// GLC World
//
//-----------------------------------------------------------------------------
GLC_World repo::gui::RepoTranscoderAssimp::toGLCWorld(
	const aiScene* assimpScene,
	const std::map<std::string, QImage> &textures,
	const std::string &namePrefix)
{
	//-------------------------------------------------------------------------
	// Temporary textures
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
	QVector<GLC_Material *> glcMaterials(assimpScene->mNumMaterials);
	for (unsigned int i = 0; i < assimpScene->mNumMaterials; ++i)
		glcMaterials[i] = toGLCMaterial(
				assimpScene->mMaterials[i],
				glcTextures);

	//-------------------------------------------------------------------------
	// Allocate meshes
	QVector<GLC_3DRep*> glcMeshes(assimpScene->mNumMeshes);
	for (unsigned int i = 0; i < assimpScene->mNumMeshes; ++i)
		glcMeshes[i] = toGLCMesh(assimpScene->mMeshes[i], glcMaterials, namePrefix);

	//-------------------------------------------------------------------------
	// Allocate cameras
	QHash<const QString, GLC_3DRep*> glcCameras;
	for (unsigned int i = 0; i < assimpScene->mNumCameras; ++i)
		glcCameras.insert(
			QString(assimpScene->mCameras[i]->mName.data),
			toGLCCamera(assimpScene->mCameras[i]));

	//-------------------------------------------------------------------------
	// Recursively build the scene graph
	GLC_World world;
	if (assimpScene->mRootNode)
	{
		GLC_World * tempWorld = new GLC_World(
			createOccurrenceFromNode(
				assimpScene, 
				assimpScene->mRootNode,
				glcMeshes,
				glcCameras)); 
		tempWorld->setRootName(QString(assimpScene->mRootNode->mName.data));
	
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
	for (unsigned int i = 0; i < glcMeshes.size(); ++i)
	{
		delete glcMeshes[i];
		glcMeshes[i] = 0;
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

	//-------------------------------------------------------------------------
	// Remove only unused materials.
/*	for (unsigned int i = 0; i < glcMaterials.size(); ++i)
	{
		if (glcMaterials[i] && glcMaterials[i]->isUnused())
			delete glcMaterials[i];
	}
*/
	return world;
}

GLC_StructOccurence* repo::gui::RepoTranscoderAssimp::createOccurrenceFromNode(
	const aiScene* assimpScene, 
	const aiNode* assimpNode,
	const QVector<GLC_3DRep*>& glcMeshes,
	const QHash<const QString, GLC_3DRep*>& glcCameras)
{
	Q_ASSERT (NULL != assimpNode);
	QString name(assimpNode->mName.C_Str());
	GLC_StructInstance* instance = NULL;
	GLC_StructOccurence* occurrence = NULL;	
	
	// TODO: store instance objects in a hash map based on their attached
	// meshes to reduce duplication of geometry. If instance already defined
	// and found in the hash map, skip the meshes section below.

	//-------------------------------------------------------------------------
	// Meshes
	if (assimpNode->mNumMeshes > 0)
	{
		GLC_3DRep* pRep = NULL;
		for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i)
		{
			GLC_3DRep * glcMesh = glcMeshes[assimpNode->mMeshes[i]];
			if (glcMesh)
			{
				if (NULL == pRep)
					pRep = new GLC_3DRep(*glcMesh);
				else
					pRep->merge(glcMesh); // instead of merging pRep
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
	else // No meshes in this node
	{ 	
		// Check if the node points to a camera and if so, take its wireframe representation
		QHash<const QString, GLC_3DRep*>::const_iterator it = glcCameras.find(name);
		if (it != glcCameras.end())
			instance = new GLC_StructInstance(new GLC_StructReference(new GLC_3DRep(*(it.value()))));
		else
			instance = new GLC_StructInstance(new GLC_StructReference(name));	
		instance->setName(name);
	}

	//-------------------------------------------------------------------------
	// Transformation
	instance->move(toGLCMatrix(assimpNode->mTransformation));
	occurrence = new GLC_StructOccurence(instance);
	occurrence->setName(name); // TODO: put back



	//-------------------------------------------------------------------------
	// Children
	if (assimpNode->mNumChildren > 0)
	{
		for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i)
			occurrence->addChild(
				createOccurrenceFromNode(
					assimpScene, 
					assimpNode->mChildren[i],
					glcMeshes,
					glcCameras));
	}
	return occurrence;
}

//-----------------------------------------------------------------------------
//
// Static helpers
//
//-----------------------------------------------------------------------------

GLC_Material * repo::gui::RepoTranscoderAssimp::toGLCMaterial(
	const aiMaterial * assimpMaterial,
	const QHash<QString, GLC_Texture> & glcTextures)
{
	// See http://assimp.sourceforge.net/lib_html/materials.html
	GLC_Material * glcMaterial = new GLC_Material;
	aiColor3D c;

	//-------------------------------------------------------------------------
	// Name	
	aiString name;
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_NAME, name))
		glcMaterial->setName(QString(name.C_Str()));
	
	//-------------------------------------------------------------------------
	// Diffuse
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, c))
		glcMaterial->setDiffuseColor(toQColor(c));
	
	//-------------------------------------------------------------------------
	// Shininess
	float shininess;
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SHININESS, shininess))
		glcMaterial->setShininess(std::min(shininess/128.0, 1.0));	
	
	//-------------------------------------------------------------------------
	// Specular
	float shininessStrength = 1.0;
	assimpMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);		
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, c))
		glcMaterial->setSpecularColor(toQColor(c, shininessStrength));
	
	//-------------------------------------------------------------------------
	// Ambient
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, c))
		glcMaterial->setAmbientColor(toQColor(c));

	//-------------------------------------------------------------------------
	// Emissive
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, c))
		glcMaterial->setEmissiveColor(toQColor(c));

	//-------------------------------------------------------------------------
	// Opacity
	float opacity;
	if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_OPACITY, opacity))
		glcMaterial->setOpacity(opacity);

	//-------------------------------------------------------------------------
	// Diffuse texture
	aiString path;	
	int texIndex = 0;
	aiReturn texFound = AI_SUCCESS;
	while (AI_SUCCESS == texFound && !glcMaterial->hasTexture())
	{			
		texFound = assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 
			texIndex, & path);	
		QString name(path.data);
		if (!name.isEmpty())
		{
			QHash<QString, GLC_Texture>::const_iterator it = 
				glcTextures.find(name);
			if (glcTextures.end() != it)
				glcMaterial->setTexture(new GLC_Texture(it.value()));
		}
		texIndex++;
	}
	return glcMaterial;
}

GLC_3DRep* repo::gui::RepoTranscoderAssimp::toGLCMesh(
	const aiMesh * assimpMesh,
	const QVector<GLC_Material*>& glcMaterials,
	const std::string &namePrefix)
{
	GLC_Mesh * glcMesh = new GLC_Mesh;
	std::string name = namePrefix + assimpMesh->mName.C_Str();
	glcMesh->setName(QString::fromStdString(name));
			
	//-----------------------------------------------------------------
	// Vertices
	const QVector<GLfloat> vertices = toGLCVector(
		assimpMesh->mVertices, 
		assimpMesh->mNumVertices,
		XYZ);
	glcMesh->addVertice(vertices);
					
	//-----------------------------------------------------------------
	// Normals
	if (assimpMesh->HasNormals())
		glcMesh->addNormals(
			toGLCVector(
				assimpMesh->mNormals, 
				assimpMesh->mNumVertices,
				XYZ));

	//-----------------------------------------------------------------
	// Vertex colours
	// Assimp provides potentially multiple colours per vertex.
	if (assimpMesh->HasVertexColors(0))
	{						
		glcMesh->setColorPearVertex(true);
		glcMesh->addColors(toGLCVector(
			assimpMesh->mColors[0], assimpMesh->mNumVertices));	
	}

	/*						
	QVector<GLfloat> vector(glcMesh->VertexCount() * 4); // always [r,g,b,a]
	for (int i = 0; i < assimpMesh->mNumFaces; ++i)
	{
		const aiFace assimpFace = assimpMesh->mFaces[i];		
		float r = (float) rand() / (RAND_MAX);
		float g = (float) rand() / (RAND_MAX);
		float b = (float) rand() / (RAND_MAX);
		for (unsigned int j = 0; j < assimpFace.mNumIndices; ++j)
		{
			unsigned int index = assimpFace.mIndices[j] * 4;
			vector[index + 0] = r;
			vector[index + 1] = g;
			vector[index + 2] = b;
			vector[index + 3] = 0.5f;
		}
	}

	glcMesh->setColorPearVertex(true);
	//glcMesh->setColors(vector);
	*/
			
		
	//-------------------------------------------------------------------------
	// Faces (triangles) with assigned material
	if (assimpMesh->HasFaces())
	{
		glcMesh->addTriangles(
			glcMaterials[assimpMesh->mMaterialIndex], 
			toGLCList(
				vertices.toList(),
				assimpMesh->mFaces, 
				assimpMesh->mNumFaces));

		//---------------------------------------------------------------------
		// Wireframe
		// Since GLC_Lib renders only triangles, the wireframe for polygon
		// faces has to be created separately.
		GLfloatVector faceVertices;
		aiVector3D vertex;
		for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i)
		{
			for (unsigned int j = 0; j < assimpMesh->mFaces[i].mNumIndices; ++j)
			{
				vertex = assimpMesh->mVertices[assimpMesh->mFaces[i].mIndices[j]];
				faceVertices << vertex.x << vertex.y << vertex.z;
			}
			glcMesh->addVerticeGroup(faceVertices);
			faceVertices.clear();
		}
	}

	//-------------------------------------------------------------------------
	// Texture coordinates
	// Assimp provides multiple textures per vertex eg U, UV or UVW
	if (assimpMesh->HasTextureCoords(0))
		glcMesh->addTexels(toGLCVector(
			assimpMesh->mTextureCoords[0],
			assimpMesh->mNumVertices,
			XY));
		
	//-----------------------------------------------------------------
	// Copy index list in a vector for Vertex Array Use
	glcMesh->finish();
	GLC_3DRep* pRep = new GLC_3DRep(glcMesh);
	glcMesh = NULL;
	pRep->clean();

	return pRep;
}


GLC_3DRep * repo::gui::RepoTranscoderAssimp::toGLCCamera(const aiCamera * assimpCamera)
{
	GLC_Point3d position = toGLCPoint(assimpCamera->mPosition);
	aiMatrix4x4 matrix;
	assimpCamera->GetCameraMatrix(matrix);
	GLC_Matrix4x4 glcMatrix = toGLCMatrix(matrix);

	float halfWidth = 0.5; //std::tan(assimpCamera->mHorizontalFOV) / depth;
	float halfHeight = halfWidth / (assimpCamera->mAspect == 0 ?  1 : assimpCamera->mAspect);
	float depth = halfWidth*2 / std::tan(assimpCamera->mHorizontalFOV);

	// Camera wireframe representation
	GLC_3DRep * camera = new GLC_3DRep(new RepoGLCCamera(position, depth, halfWidth, halfHeight, glcMatrix));
	std::string name = assimpCamera->mName.C_Str();
	camera->setName(QString::fromStdString(name));
	return camera;
}

QColor repo::gui::RepoTranscoderAssimp::toQColor(const aiColor3D &c, float scale)
{
	return QColor(
		std::min(int (c.r * scale * 255), 255),
		std::min(int (c.g * scale * 255), 255),
		std::min(int (c.b * scale * 255), 255));
}

QColor repo::gui::RepoTranscoderAssimp::toQColor(const aiColor4D &c)
{
	return QColor(
		std::min(int (c.r * 255), 255),
		std::min(int (c.g * 255), 255),
		std::min(int (c.b * 255), 255), 
		std::min(int (c.a * 255), 255));
}

QVector<GLfloat> repo::gui::RepoTranscoderAssimp::toGLCVector(
	const aiVector3D * vertexArray, 
	unsigned int arraySize,
	RepoOrdinates ordinatesType)
{
	QVector<GLfloat> glcVector(arraySize * ordinatesType); // always [x,y,z]
	for (unsigned int i = 0, j = 0; i < glcVector.size(); i += ordinatesType, ++j)
	{
		// This switch deliberately falls through to capture XYZ, XY or just X
		// depending on the need. I.e. UV textures would be only XY.
		switch(ordinatesType) 
		{
			case XYZ :
				glcVector[i + 2] = ((GLfloat) (vertexArray[j].z));
			case XY :
				glcVector[i + 1] = ((GLfloat) (vertexArray[j].y));
			case X :
				glcVector[i + 0] = ((GLfloat) (vertexArray[j].x));
		}
	}
	return glcVector;
}

QVector<GLfloat> repo::gui::RepoTranscoderAssimp::toGLCVector(
	const aiColor4t<float> * colorArray,
	unsigned int arraySize)
{
	QVector<GLfloat> glcVector(arraySize * 4); // always [r,g,b,a]
	for (unsigned int i = 0, j = 0; i < glcVector.size(); i += 4, ++j)
	{
		glcVector[i + 0] = ((GLfloat) (colorArray[j].r));
		glcVector[i + 1] = ((GLfloat) (colorArray[j].g));
		glcVector[i + 2] = ((GLfloat) (colorArray[j].b));
		glcVector[i + 3] = ((GLfloat) (colorArray[j].a));		
	}
	return glcVector;
}

QList<GLuint> repo::gui::RepoTranscoderAssimp::toGLCList(
	const QList<GLfloat> & verticesList,
	const aiFace * facesAarray, 
	unsigned int size)
{
	QList<GLuint> glcList;
	for (unsigned int i = 0; i < size; ++i)
	{
		const aiFace assimpFace = facesAarray[i];		
		QList<GLuint> glcFaceIndices;
		glcFaceIndices.reserve(assimpFace.mNumIndices);

		//---------------------------------------------------------------------
		// Copy all assimp indices of a single face to a QList
		std::copy(
			assimpFace.mIndices + 0, 
			assimpFace.mIndices + assimpFace.mNumIndices, 
			std::back_inserter(glcFaceIndices));

		//---------------------------------------------------------------------
		// GLC 2.5.0 can render only up to triangles,
		// hence triangulate all non-tri faces
		if (glcFaceIndices.size() > 3) 
			glc::triangulatePolygon(& glcFaceIndices, verticesList);
		
		glcList.append(glcFaceIndices);
	}
	return glcList;
}

GLC_Matrix4x4 repo::gui::RepoTranscoderAssimp::toGLCMatrix
	(const aiMatrix4x4 & assimpMatrix)
{
	// Assimp's is row-major while GLC is column-major
	double * temp = new double[16];
	//-------------------------------------------------------------------------
	temp[0]  = assimpMatrix.a1;
	temp[4]  = assimpMatrix.a2;
	temp[8]  = assimpMatrix.a3;
	temp[12] = assimpMatrix.a4;	 
	//-------------------------------------------------------------------------
	temp[1]  = assimpMatrix.b1;
	temp[5]  = assimpMatrix.b2;
	temp[9]  = assimpMatrix.b3;
	temp[13] = assimpMatrix.b4;
	//-------------------------------------------------------------------------	
	temp[2]  = assimpMatrix.c1;
	temp[6]  = assimpMatrix.c2;
	temp[10] = assimpMatrix.c3;
	temp[14] = assimpMatrix.c4;
	//-------------------------------------------------------------------------	
	temp[3]  = assimpMatrix.d1;
	temp[7]  = assimpMatrix.d2;
	temp[11] = assimpMatrix.d3;
	temp[15] = assimpMatrix.d4;
	//-------------------------------------------------------------------------
	GLC_Matrix4x4 glcMatrix(temp);
	delete [] temp;
	return glcMatrix;		
}

GLC_Point3d repo::gui::RepoTranscoderAssimp::toGLCPoint(const aiVector3D &v)
{
	return GLC_Point3d(v.x, v.y, v.z);
}
