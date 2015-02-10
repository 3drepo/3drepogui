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


#include "repo_worker_assimp.h"
#include <QFileInfo>
#include <assimp/cimport.h>
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------

repo::gui::RepoWorkerAssimp::RepoWorkerAssimp(const QString & fullPath,
                                              const RepoWidgetAssimpFlags* assimpFlagsWidget)
: fullPath(fullPath)
, assimpFlagsWidget(assimpFlagsWidget)
, RepoWorkerAbstract() {}

repo::gui::RepoWorkerAssimp::~RepoWorkerAssimp() {}

QString repo::gui::RepoWorkerAssimp::getFileName(const QString& fullFilePath)
{
	QFileInfo path(fullFilePath);
	return path.fileName();
}

std::map<std::string, QImage> repo::gui::RepoWorkerAssimp::loadTextures(
        const aiScene *scene,
        const std::string &basePath)
{
	std::map<std::string, QImage> namedTextures;
	for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;
		const aiMaterial *material = scene->mMaterials[m];

		aiString path;	// filename
		while (AI_SUCCESS == texFound)
		{			
			texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, & path);	
			std::string fileName(path.data);

			// Multiple materials can point to the same texture
			if (!fileName.empty() && namedTextures.find(fileName.c_str()) == namedTextures.end())
			{
				QImage image;
				// In assimp, embedded textures name starts with asterisk and a textures array index
				// so the name can be "*0" for example
				if (scene->HasTextures() && '*' == fileName.at(0))
				{ 
					//---------------------------------------------------------
					// Embedded texture
					int textureIndex = atoi(fileName.substr(1, fileName.size()).c_str());				
					aiTexture *texture = scene->mTextures[textureIndex];
				
					if (0 == texture->mHeight) 
					{
						// if height is 0, it is compressed
						const uchar * data = (uchar *) texture->pcData;
						image = QImage::fromData(data, (int) texture->mWidth).
							mirrored(false, true);								
					}
					else 
					{			
						image = QImage(
							texture->mWidth, 
							texture->mWidth, 
							QImage::Format_ARGB32);
						for (unsigned int i = 0; i < texture->mWidth; ++i) {
							for (unsigned int j = 0; j < texture->mHeight; ++j) {
								aiTexel texel = texture->pcData[
									i * texture->mHeight + j];
								image.setPixel(i, j, qRgba(
									texel.r,
									texel.g, 
									texel.b, 
									texel.a));
							}
						}					
					}	
				}
				else
				{ 
					//---------------------------------------------------------
					// External texture
					QString fileloc((basePath + fileName).c_str());
					if (!image.load(fileloc))
					{			
                        std::cerr << "Image " << fileloc.toStdString();
                        std::cerr << " could not be loaded." << std::endl;
						/*
						if (fileName.size() >= 3 &&  TGAHandler::TGA_EXTENSION == fileName.substr(fileName.size() - 3, 3)) 
						{
							QFile file(fileloc);
							file.open(QIODevice::ReadOnly);
							QDataStream in(&file);    // read the data serialized from the file
							in.setByteOrder(QDataStream::LittleEndian);
							TGAHandler *handler = new TGAHandler();
							handler->read(in, &image);
						}		*/

					}		
				}
				namedTextures.insert(std::make_pair(path.data, image));
			}
			texIndex++;
		}
	}	
	return namedTextures;
}

std::map<std::string, repo::core::RepoNodeAbstract *> repo::gui::RepoWorkerAssimp::loadTextures(
		const std::map<string, QImage> &qtextures,
		const std::string &texturesFolderPath)
{
	std::map<std::string, repo::core::RepoNodeAbstract *> repoTextures;
	for (map<string, QImage>::const_iterator it = qtextures.begin(); it != qtextures.end(); it++)
	{
		std::string name = it->first;
		QImage qimage = it->second;
					
		// If QImage checked that the file exists and is loadable...
		if (NULL != qimage.bits())
		{
			// Read the raw file to save space in the DB. QImage will happily
			// claim a file is 32 bit depth even though it is 24 for example.							
            ifstream::pos_type size;
            char *memblock;
			ifstream file (texturesFolderPath + name, ios::in|ios::binary|ios::ate);
			if (!file.is_open())
            {
                std::cerr <<  "Could not open texture: " << texturesFolderPath <<  name << std::endl;
            }
			else
			{
				size = file.tellg();
				memblock = new char [size];
				file.seekg (0, ios::beg);
				file.read (memblock, size);
				file.close();								
								
				repo::core::RepoNodeAbstract * texture = new repo::core::RepoNodeTexture(
					name,
					memblock,
					(unsigned int) size,
					qimage.width(),
					qimage.height());
				repoTextures.insert(std::make_pair(name, texture));	
				delete [] memblock;
			}
		}
	}
	return repoTextures;
}

void repo::gui::RepoWorkerAssimp::run()
{
	//-------------------------------------------------------------------------
	// Start
	int jobsCount = 5;
	emit progress(0, 0);
	std::string fileName = getFileName(fullPath).toStdString();

	//-------------------------------------------------------------------------
	// Import model
    repo::core::AssimpWrapper assimpWrapper;
	assimpWrapper.importModel(
		fileName, 
		fullPath.toStdString(), 
        settings.getAssimpPostProcessingFlags(),
        settings.getSplitLargeMeshesTriangleLimit(),
        settings.getSplitLargeMeshesVertexLimit(),
        settings.getSortAndRemovePoints(),
        settings.getSortAndRemoveLines(),
        settings.getSortAndRemoveTriangles(),
        settings.getSortAndRemovePolygons());
    const aiScene *assimpScene = assimpWrapper.getScene();
	emit progress(1, jobsCount);
	
	repo::core::RepoGraphScene * repoGraphScene = 0;
	GLC_World glcWorld;

	if (!assimpScene)
        std::cerr << std::string(aiGetErrorString()) << std::endl;
	else
	{
		//-------------------------------------------------------------------------
		// Polygon count
		qlonglong polyCount = 0;
		for (unsigned int i = 0; i < assimpScene->mNumMeshes; ++i) 
			polyCount += assimpScene->mMeshes[i]->mNumFaces;

        std::cout << "Loaded ";
        std::cout << fileName << " with " << polyCount << " polygons in ";
        std::cout << assimpScene->mNumMeshes << " ";
        std::cout << ((assimpScene->mNumMeshes == 1) ? "mesh" : "meshes");
        std::cout << std::endl;
		emit progress(2, jobsCount);

		//-------------------------------------------------------------------------
		// Textures
		std::map<std::string, QImage> textures = loadTextures(
			assimpScene,
			assimpWrapper.getFullFolderPath());
		emit progress(3, jobsCount);

		//-------------------------------------------------------------------------
		// Repo scene graph
		const std::map<std::string, core::RepoNodeAbstract *> tex = loadTextures(
			textures, 
			assimpWrapper.getFullFolderPath());
		repoGraphScene = new repo::core::RepoGraphScene(assimpScene, tex);
		emit progress(4, jobsCount);

		//-------------------------------------------------------------------------
		// GLC World conversion
		glcWorld = RepoTranscoderAssimp::toGLCWorld(assimpScene, textures);
	}
	emit progress(jobsCount, jobsCount);

	//-------------------------------------------------------------------------
	// Done
	emit finished(repoGraphScene, glcWorld);	
	emit RepoWorkerAbstract::finished();
}
