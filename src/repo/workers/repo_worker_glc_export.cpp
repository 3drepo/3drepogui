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

using namespace repo::worker;
namespace repoModel = repo::core::model::bson;
//------------------------------------------------------------------------------
// Core

//------------------------------------------------------------------------------


class RepoGLCCamera : public GLC_Geometry
{
public:

	RepoGLCCamera(
		const GLC_Point3d &position,
		const float depth,
		const float halfWidth,
		const float halfHeight,
		const GLC_Matrix4x4 &transformation)
		: position(position)
		, depth(depth)
		, halfWidth(halfWidth)
		, halfHeight(halfHeight)
		, transformation(transformation)
		, GLC_Geometry("Camera", true){};

	~RepoGLCCamera(){ };

	//! Returns a bounding box of this wireframe camera representation
	const GLC_BoundingBox& boundingBox()
	{
		if (NULL == m_pBoundingBox)
		{
			m_pBoundingBox = new GLC_BoundingBox();
			if (m_WireData.isEmpty())
				createWire();
			m_pBoundingBox->combine(m_WireData.boundingBox());
			m_pBoundingBox->combine(GLC_Vector3d(0.0, 0.0, -2 * glc::EPSILON));
		}
		return *m_pBoundingBox;
	}

	//! Returns a clone
	GLC_Geometry* clone() const
	{
		return new RepoGLCCamera(*this);
	}

private:

	//! Renders the wireframe presentation as a line strip.
	void glDraw(const GLC_RenderProperties& renderProperties)
	{
		if (m_WireData.isEmpty())
			createWire();
		m_WireData.glDraw(renderProperties, GL_LINE_STRIP);
	}


	//! Creates the wireframe representation from the given GLC Camera.
	void createWire()
	{
		{
			GLfloatVector vertices;

			GLC_Matrix4x4 rotation = transformation.rotationMatrix().transpose(); //camera.modelViewMatrix().rotationMatrix().transpose(); //.invert();
			GLC_Point3d eye = position; // == rotation * GLC_Point3d(0,0,0) + camera.eye()

			GLC_Point3d frustrumTopLeft = rotation * GLC_Point3d(-halfWidth, halfHeight, depth) + position;
			GLC_Point3d frustrumTopRight = rotation * GLC_Point3d(halfWidth, halfHeight, depth) + position;
			GLC_Point3d frustrumBottomRight = rotation * GLC_Point3d(halfWidth, -halfHeight, depth) + position;
			GLC_Point3d frustrumBottomLeft = rotation * GLC_Point3d(-halfWidth, -halfHeight, depth) + position;

			float triangleHalfBase = halfWidth * 0.7f; // Half of the side of the triangle
			float triangleHeight = triangleHalfBase; // sqrt(triangleHalfBase * 2.0f * triangleHalfBase * 2.0f - triangleHalfBase * triangleHalfBase);
			float verticalOffset = halfHeight * 1.1f; // Y-axis offset to position triangle above viewport
			GLC_Point3d triangleLeft = rotation * GLC_Point3d(-triangleHalfBase, verticalOffset, depth) + position;
			GLC_Point3d triangleTop = rotation * GLC_Point3d(0.0f, verticalOffset + triangleHeight, depth) + position;
			GLC_Point3d triangleRight = rotation * GLC_Point3d(triangleHalfBase, verticalOffset, depth) + position;

			//-------------------------------------------------------------------------
			// Frustrum arms
			vertices << eye.x() << eye.y() << eye.z(); // eye
			vertices << frustrumTopLeft.x() << frustrumTopLeft.y() << frustrumTopLeft.z(); // top left
			GLC_Geometry::addVerticeGroup(vertices);
			vertices.clear();

			vertices << eye.x() << eye.y() << eye.z(); // eye
			vertices << frustrumTopRight.x() << frustrumTopRight.y() << frustrumTopRight.z(); // top right
			GLC_Geometry::addVerticeGroup(vertices);
			vertices.clear();

			vertices << eye.x() << eye.y() << eye.z(); // eye
			vertices << frustrumBottomRight.x() << frustrumBottomRight.y() << frustrumBottomRight.z(); // bottom right
			GLC_Geometry::addVerticeGroup(vertices);
			vertices.clear();

			vertices << eye.x() << eye.y() << eye.z(); // eye
			vertices << frustrumBottomLeft.x() << frustrumBottomLeft.y() << frustrumBottomLeft.z(); // bottom left
			GLC_Geometry::addVerticeGroup(vertices);
			vertices.clear();

			//-------------------------------------------------------------------------
			// Viewport 
			vertices << frustrumTopLeft.x() << frustrumTopLeft.y() << frustrumTopLeft.z(); // top left
			vertices << frustrumTopRight.x() << frustrumTopRight.y() << frustrumTopRight.z(); // top right
			vertices << frustrumBottomRight.x() << frustrumBottomRight.y() << frustrumBottomRight.z(); // bottom right
			vertices << frustrumBottomLeft.x() << frustrumBottomLeft.y() << frustrumBottomLeft.z(); // bottom left
			vertices << frustrumTopLeft.x() << frustrumTopLeft.y() << frustrumTopLeft.z(); // top left
			GLC_Geometry::addVerticeGroup(vertices);
			vertices.clear();

			//-------------------------------------------------------------------------
			// Up Triangle	
			vertices << triangleLeft.x() << triangleLeft.y() << triangleLeft.z(); // -1.0f << -0.4f << 0.3f; // left
			vertices << triangleTop.x() << triangleTop.y() << triangleTop.z(); // -1.0f << 0.4f << 0.3f; // top 
			vertices << triangleRight.x() << triangleRight.y() << triangleRight.z(); // -1.0f << 0.0f << 0.6f; // right
			vertices << triangleLeft.x() << triangleLeft.y() << triangleLeft.z(); // -1.0f << -0.4f << 0.3f; // left
			GLC_Geometry::addVerticeGroup(vertices);
			vertices.clear();
		}
	}

	float depth;

	float halfWidth;

	float halfHeight;

	GLC_Point3d position;

	GLC_Matrix4x4 transformation;

};

GLCExportWorker::GLCExportWorker(
	repo::manipulator::graph::RepoScene* scene):scene(scene){}

GLCExportWorker::~GLCExportWorker() {}

void GLCExportWorker::run()
{

	if (scene)
	{
		//-------------------------------------------------------------------------
		// Start
		jobsCount = scene->getItemsInCurrentGraph();
		done = 0;
		emit progress(0, 0); // undetermined (moving) progress bar
		GLC_World *glcWorld = createGLCWorld(scene);

		//--------------------------------------------------------------------------
		emit progress(jobsCount, jobsCount);
		repoLog("DeuBG: size of pre world is " + std::to_string(glcWorld->size()));
		GLC_World wholeGraph = glcWorld ? GLC_World(*glcWorld) : GLC_World();
		//--------------------------------------------------------------------------

		emit finished(scene, wholeGraph);
		repoLog("Emitted, size of world is " + std::to_string(wholeGraph.size()) + ", returning");
	}
	else{
		repoLog("Trying to produce a GLC representation with a nullptr to scene!");
	}
	emit RepoAbstractWorker::finished();
}

GLC_World* GLCExportWorker::createGLCWorld(
	repo::manipulator::graph::RepoScene *scene)
{

	//FIXME: to remove
	int nmeshes = 0, nmaterials = 0, ncameras = 0, ntexture = 0;
	//-----
	GLC_World* glcWorld = nullptr;
	//------------------------------------------------------------------
	// Allocate Textures
	std::map<repoUUID, std::vector<GLC_Texture*>> parentToGLCTexture;

	repoModel::RepoNodeSet textures = scene->getAllTextures();

	for (auto &texture : textures)
	{
		if (texture)
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
				++ntexture;
			}

		}
	}

	//------------------------------------------------------------------
	// Allocate Materials
	std::map<repoUUID, std::vector<GLC_Material*>> parentToGLCMaterial;

	repoModel::RepoNodeSet materials = scene->getAllMaterials();

	for (auto &material : materials)
	{
		if (material)
		{
			GLC_Material* glcMat = convertGLCMaterial(
				(repoModel::MaterialNode*)material, parentToGLCTexture);
			if (glcMat)
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
				++nmaterials;
			}

		}
	}



	//-------------------------------------------------------------------------
	// Allocate Meshes

	repoModel::RepoNodeSet meshes = scene->getAllMeshes();
	std::map<repoUUID, std::vector<GLC_3DRep*>> parentToGLCMeshes;
	for (auto &mesh : meshes)
	{
		if (mesh)
		{
			GLC_3DRep* glcMesh = convertGLCMesh(
				(repoModel::MeshNode*)mesh, parentToGLCMaterial);
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
				++nmeshes;
			}
		}
	}

	//-------------------------------------------------------------------------
	// Allocate cameras
	repoModel::RepoNodeSet cameras = scene->getAllCameras();
	std::map<repoUUID, std::vector<GLC_3DRep*>> parentToGLCCameras;
	for (auto &camera : cameras)
	{
		if (camera)
		{
			GLC_3DRep* glcCamera = convertGLCCamera(
				(repoModel::CameraNode*)camera);
			if (glcCamera)
			{
				std::vector<repoUUID> parents = camera->getParentIDs();
				for (auto &parent : parents)
				{
					//Map the material to all parent UUIDs
					if (parentToGLCCameras.find(parent) == parentToGLCCameras.end())
					{
						parentToGLCCameras[parent] = std::vector<GLC_3DRep*>();
					}
					//Map the mesh to all parent UUIDs
					parentToGLCCameras[parent].push_back(glcCamera);
				}
				++ncameras;
			}

		}
	}

	repoLog("Debug: " + std::to_string(nmeshes) + " meshes");
	repoLog("Debug: " + std::to_string(nmaterials) + " mats");
	repoLog("Debug: " + std::to_string(ntexture) + " textures");
	repoLog("Debug: " + std::to_string(ncameras) + " ncameras");

	//------------------------------------------------------------------
	// GLC World conversion - recursion to obtain the whole tree
	if (!cancelled && scene && scene->hasRoot()){
		glcWorld = new GLC_World(createOccurrenceFromNode(scene, scene->getRoot(), parentToGLCMeshes, parentToGLCCameras));
	}

	return glcWorld;
}


GLC_StructOccurence* GLCExportWorker::createOccurrenceFromNode(
	repo::manipulator::graph::RepoScene                     *scene,
	const repo::core::model::bson::RepoNode                 *node,
	std::map<repoUUID, std::vector<GLC_3DRep*>>             &glcMeshesMap,
	std::map<repoUUID, std::vector<GLC_3DRep*>>             &glcCamerasMap,
	const bool                                              &countJob)
{
	/*
	* There are a number of assumptions within this function
	* - Materials/Textures of a mesh lives in the same scene (not in a referenced scene)
	* - Materials are immediate children to the mesh that references it
	* - Textures are immediate children of the material that references it
	* - Meshes are immediate children of transformations
	*/
	GLC_StructInstance* instance = nullptr;
	GLC_StructOccurence* occurrence = nullptr;
	if (node)
	{
		repoLog("Debug: node type: " + node->getType());
		repoModel::NodeType type = node->getTypeAsEnum();
		repoUUID sharedID = node->getSharedID();

		QString name(node->getName().c_str());
		switch (type)
		{
			/**
			* Base on the assumption, there are only 2 types of nodes that we need to deal with
			* TRANSFORMATION - kind all its children meshes/cameras and add them into the instance
			* REFERENCE - Create another GLC world out of that referenced scene
			* Meshes/Materials/Textures should be already set up to be dealt with at it's
			* parent transformation.
			* Anything else is probably not rendering related(?) and should be ignored
			*/
		case repoModel::NodeType::TRANSFORMATION:
		{
			GLC_3DRep* pRep = nullptr;
			std::map<repoUUID, std::vector<GLC_3DRep*>>::iterator it = glcMeshesMap.find(sharedID);
			if (it != glcMeshesMap.end())
			{
				//has meshes
				for (auto glcMesh : it->second)
				{
					if (glcMesh)
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
						//delete pRep;
						pRep = 0;
						instance = new GLC_StructInstance(new GLC_StructReference(name));
					}
					else{
						instance = new GLC_StructInstance(new GLC_StructReference(pRep));
					}
				}
				else
				{
					std::cerr << "NULL geometry in node " << name.toStdString() << std::endl;
					instance = new GLC_StructInstance(new GLC_StructReference(name));
				}
			}

			//FIXME: instance is overridden by camera if there is a mesh + camera under the same transformation!
			it = glcCamerasMap.find(sharedID);
			if (it != glcCamerasMap.end())
			{
				//has cameras
				for (auto &glcCamera : it->second)
				{
					if (glcCamera)
					{

						instance = new GLC_StructInstance(new GLC_StructReference(new GLC_3DRep(*glcCamera)));
						//if (pRep)
						//	// instead of merging pRep
						//	pRep->merge(glcCamera);
						//else
						//	pRep = new GLC_3DRep(*glcCamera);
					}

				}
			}
			
			if (!instance)
			{
				instance = new GLC_StructInstance(new GLC_StructReference(name));
			}

			//-------------------------------------------------------------------------
			// Transformation
			instance->move(GLC_Matrix4x4(&((repoModel::TransformationNode*)node)->getTransMatrix().at(0)));
			occurrence = new GLC_StructOccurence(instance);
			occurrence->setName(name);

			break;
		}
		case repoModel::NodeType::REFERENCE:
			repo::manipulator::graph::RepoScene *refScene = scene->getSceneFromReference(node->getSharedID());
			if (refScene)
			{
				GLC_StructOccurence *childOccurance = createOccurrenceFromNode(
					refScene,
					refScene->getRoot(),
					glcMeshesMap,
					glcCamerasMap,
					false);
				if (childOccurance)
					occurrence = childOccurance;
			}

			break;
		}//switch

		//-------------------------------------------------------------------------
		// Children

		for (auto child : scene->getChildrenAsNodes(node->getSharedID()))
		{

			GLC_StructOccurence *childOccurance = createOccurrenceFromNode(
				scene,
				child,
				glcMeshesMap,
				glcCamerasMap,
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
	const repo::core::model::bson::CameraNode *camera)
{
	GLC_3DRep * glcCamera = nullptr;
	if (camera)
	{
		repo_vector_t position = camera->getPosition();
		GLC_Point3d glcPosition(position.x, position.y, position.z);

		GLC_Matrix4x4 glcMatrix(&camera->getCameraMatrix(false).at(0));

		float halfWidth = 0.5; //std::tan(assimpCamera->mHorizontalFOV) / depth;
		float aspectRatio = camera->getAspectRatio();

		float halfHeight = halfWidth / (aspectRatio ? aspectRatio : 1);
		float depth = halfWidth * 2 / std::tan(camera->getHorizontalFOV());

		// Camera wireframe representation
		RepoGLCCamera *repoGLCCam = new RepoGLCCamera(glcPosition, depth, halfWidth, halfHeight, glcMatrix);


		glcCamera = new GLC_3DRep(repoGLCCam);

		glcCamera->setName(QString::fromStdString(camera->getName().c_str()));
	}

	return glcCamera;

}

GLC_Material* GLCExportWorker::convertGLCMaterial(
	const repo::core::model::bson::MaterialNode   *material,
	std::map<repoUUID, std::vector<GLC_Texture*>> &mapTexture)
{
	GLC_Material* glcMat = nullptr;

	if (material)
	{
		glcMat = new GLC_Material();
		glcMat->setName(QString(material->getName().c_str()));
		repo_material_t repoMat = material->getMaterialStruct();

		repoLog("------Material " + material->getName() + " --------------");
		//-------------------------------------------------------------------------
		// Ambient
		if (repoMat.ambient.size() >= 3)
		{
			QColor ambientCol = toQColor(repoMat.ambient);
			glcMat->setAmbientColor(ambientCol);

			repoLog("Ambient: " + ambientCol.name().toStdString());
		}

		//-------------------------------------------------------------------------
		// Diffuse
		if (repoMat.diffuse.size() >= 3)
		{
			QColor diffuse = toQColor(repoMat.diffuse);
			glcMat->setDiffuseColor(diffuse);


			repoLog("diffuse: " + diffuse.name().toStdString());
		}

		//-------------------------------------------------------------------------
		// Emissive
		if (repoMat.emissive.size() >= 3)
		{
			QColor emissive = toQColor(repoMat.emissive);
			glcMat->setEmissiveColor(emissive);

			repoLog("emissive: " + emissive.name().toStdString());

		}

		//-------------------------------------------------------------------------
		// Shininess
		if (repoMat.shininess == repoMat.shininess)
		{

			float shininess = std::min(repoMat.shininess / 128.0, 1.0);
			glcMat->setShininess(shininess);

			repoLog("shininess : " + std::to_string(shininess));
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
			repoLog("specular: " + specular.name().toStdString());
		}

		//-------------------------------------------------------------------------
		// Opacity
		if (repoMat.opacity == repoMat.opacity)
		{
			glcMat->setOpacity(repoMat.opacity);
			repoLog("opacity : " + std::to_string(repoMat.opacity));
		}

	
		//-------------------------------------------------------------------------
		// Diffuse texture
		std::map<repoUUID, std::vector<GLC_Texture*>>::iterator mapIt = mapTexture.find(material->getSharedID());
		if (mapIt != mapTexture.end())
		{
			repoLog("has texture");
			//FIXME: only takes in 1 texture.
			glcMat->setTexture((GLC_Texture*)mapIt->second.at(0));
		}
		else
		{
			repoLog("no texture");
		}

		repoLog("-------------------------------------");
	}

	return glcMat;
}

GLC_3DRep* GLCExportWorker::convertGLCMesh(
	const repo::core::model::bson::MeshNode        *mesh,
	std::map<repoUUID, std::vector<GLC_Material*>> &mapMaterials)
{
	GLC_Mesh * glcMesh = new GLC_Mesh;
	if (mesh)
	{

		std::string name = mesh->getName();
		glcMesh->setName(QString::fromStdString(name));

		std::vector<repo_vector_t> *vector3d;
		//Vertices
		vector3d = mesh->getVertices();
		QVector<GLfloat> glcVec = createGLCVector(vector3d);
		if (glcVec.size() > 0)
			glcMesh->addVertice(glcVec);

		std::ofstream myfile;

		repoLog("size of vertices = " + std::to_string(glcVec.size()));


		//Normals
		std::vector<repo_vector_t> * normal3d = mesh->getNormals();
		QVector<GLfloat> glcNorm = createGLCVector(normal3d);
		if (glcNorm.size() > 0)
			glcMesh->addNormals(glcNorm);

		repoLog("size of normals = " + std::to_string(glcNorm.size()));

		if (normal3d)
			delete normal3d;

		//Colors
		std::vector<repo_color4d_t> *colors;
		colors = mesh->getColors();
		QVector<GLfloat> glcCol = createGLCVector(colors);
		if (glcCol.size() > 0)
		{
			glcMesh->setColorPearVertex(true);
			glcMesh->addColors(glcCol);
		}

		repoLog("size of colors = " + std::to_string(glcCol.size()));

		if (colors)
			delete colors;

		//faces
		std::vector<repo_face_t> *faces;
		faces = mesh->getFaces();
		QList<GLuint> glcFaces = createGLCFaceList(faces, glcVec);


		if (glcFaces.size() > 0)
		{

			myfile.open("faces.txt");


			for (int i = 0; i < glcFaces.size(); ++i)
			{
				myfile << glcFaces.at(i)<< "\n";
			}
			myfile.close();
			GLC_Material* material = nullptr;
			std::map<repoUUID, std::vector<GLC_Material*>>::iterator mapIt =
				mapMaterials.find(mesh->getSharedID());
			if (mapIt != mapMaterials.end())
			{
				//FIXME: assume 1 material only
				material = (GLC_Material*)mapIt->second.at(0);
			}
			if (material)
			{
				repoLog("mesh : " + mesh->getName() + " material: " + material->name().toStdString());
			}
			glcMesh->addTriangles(material, glcFaces);

			//---------------------------------------------------------------------
			// Wireframe
			// Since GLC_Lib renders only triangles, the wireframe for polygon
			// faces has to be created separately.
			GLfloatVector faceVertices;
			myfile.open("polygon_wireframe.txt");
			for (auto &face : *faces)
			{
				for (uint32_t j = 0; j < face.numIndices; ++j)
				{
					//FIXME: this is assuming order in assimp's mVertice = vector3d's order
					repo_vector_t vertex = vector3d->at(face.indices[j]);
					faceVertices << vertex.x << vertex.y << vertex.z;
					myfile << vertex.x  << ", " << vertex.y << ", " << vertex.z << "\n";
				}

				glcMesh->addVerticeGroup(faceVertices);
				faceVertices.clear();
			}
			myfile.close();
		}

		std::vector<repo_vector2d_t>* uvVectors = mesh->getUVChannels();
		QVector<GLfloat> glcUVVec = createGLCVector(uvVectors);

		if (glcUVVec.size() > 0)
		{
			glcMesh->addTexels(glcUVVec);
		}

		if (uvVectors)
		delete uvVectors;

		if (vector3d)
			delete vector3d;
		if (faces)
			delete faces;
	}
	glcMesh->finish();


	GLC_3DRep* pRep = new GLC_3DRep(glcMesh);
	glcMesh = NULL;
	pRep->clean();
	return pRep;
}


QList<GLuint> GLCExportWorker::createGLCFaceList(
	const std::vector<repo_face_t> *faces,
	const QVector<GLfloat>         &vertices)
{
	QList<GLuint> glcList;

	if (faces)
	{
		for (auto &face : *faces)
		{
			QList<GLuint> glcFaceIndices;
			glcFaceIndices.reserve(face.numIndices);
			//---------------------------------------------------------------------
			// Copy all assimp indices of a single face to a QList
			std::copy(
				face.indices,
				face.indices + face.numIndices,
				std::back_inserter(glcFaceIndices));

			//---------------------------------------------------------------------
			// GLC 2.5.0 can render only up to triangles,
			// hence triangulate all non-tri faces
			if (glcFaceIndices.size() > 3)
				glc::triangulatePolygon(&glcFaceIndices, vertices.toList());

			glcList.append(glcFaceIndices);

		}
	}

	return glcList;
}

GLC_Texture* GLCExportWorker::convertGLCTexture(
	const repo::core::model::bson::TextureNode *texture)
{
	GLC_Texture* glcTexture = nullptr;

	if (texture)
	{
		std::vector<char> *data = texture->getRawData();
		if (data)
		{
			QImage image = QImage::fromData((uchar*)&data->at(0), data->size()*sizeof(char));
			glcTexture = new GLC_Texture(image, QString(texture->getName().c_str()));
		}
	}

	return glcTexture;
}

QVector<GLfloat> GLCExportWorker::createGLCVector(
	const std::vector<repo_color4d_t> *col
	)
{
	QVector<GLfloat> glcVector;

	if (col)
	{
		glcVector.resize(col->size() * 4); //repo_color_t always have 4 values
		int ind = 0;
		for (auto &mem : *col)
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
	const std::vector<repo_vector_t> *vec
	)
{
	QVector<GLfloat> glcVector;

	if (vec)
	{
		glcVector.resize(vec->size() * 3); //repo_vector_t always have 3 values
		int ind = 0;
		for (auto &mem : *vec)
		{
			glcVector[ind++] = (GLfloat)mem.x;
			glcVector[ind++] = (GLfloat)mem.y;
			glcVector[ind++] = (GLfloat)mem.z;
		}
	}

	return glcVector;
}


QVector<GLfloat> GLCExportWorker::createGLCVector(
	const std::vector<repo_vector2d_t> *vec
	)
{
	QVector<GLfloat> glcVector;

	if (vec)
	{
		glcVector.resize(vec->size() * 2); //repo_vector_t always have 3 values
		int ind = 0;
		for (auto &mem : *vec)
		{
			glcVector[ind++] = (GLfloat)mem.x;
			glcVector[ind++] = (GLfloat)mem.y;
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