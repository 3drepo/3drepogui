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

#pragma once
//-----------------------------------------------------------------------------
#include "repo_worker_abstract.h"
//-----------------------------------------------------------------------------
#include <repo/repo_controller.h>
#include <repo/core/model/bson/repo_node_camera.h>
#include <repo/core/model/bson/repo_node_material.h>
#include <repo/core/model/bson/repo_node_mesh.h>
#include <repo/core/model/bson/repo_node_texture.h>
#include <repo/core/model/bson/repo_node_transformation.h>
#include <repo/core/model/bson/repo_node_light.h>

#include <QImage>
#include <GLC_World>
#include <glc_factory.h>

namespace repo {
	namespace worker {


		class GLCExportWorker : public RepoAbstractWorker
		{
			Q_OBJECT

		public:

			/*! 
			Takes a scene interpretation from 3D Repo world and export
			it to GLC world.
			*/
			GLCExportWorker(
                repo::core::model::RepoScene* scene,
                const std::vector<double> &offsetVector);

			//! Default empty destructor.
			~GLCExportWorker();

			public slots :

			/*!
			* Loads scene and emits signals as the loading progresses and once
			* the loading is finished.
			*/
			void run();


			/**
			* \brief Create a GLC occurance from the node
			* This is a recursive function that will call itself with 
			* the child of the node
			* @param scene Repo scene graph
			* @param node current node to process
			* @param glcMeshesMap 
			* @param countJob contribute to the #jobs done (false for processing reference nodes)
			*/
            GLC_StructOccurrence* createOccurrenceFromNode(
				repo::core::model::RepoScene         *scene,
				const repo::core::model::RepoNode           *node,
				std::map<repoUUID, std::vector<GLC_3DRep*>> &glcMeshesMap,
                std::map<repoUUID, std::vector<GLC_3DRep*>> &glcCamerasMap,
                std::map<QString, GLC_Mesh*>     &meshMap,
                std::map<QString, GLC_Material*> &matMap,
				const bool                                        &countJob=true);

			
			/**
			* Convert a repo scene to a GLC occurance structure of nodes.
			*/
            GLC_StructOccurrence* convertSceneToOccurance(
                repo::core::model::RepoScene *scene,
                    std::map<QString, GLC_Mesh*>     &meshMap,
                    std::map<QString, GLC_Material*> &matMap,
                    const std::vector<double> &offsetVector = std::vector<double>());

		signals:

            //! Emitted when loading is finished. Passes GLC world.
            void finished(GLC_World&,
                          std::map<QString, GLC_Mesh*>&,
                          std::map<QString, GLC_Material*>&,
                          std::vector<GLC_Light*>&);

		private:
			repo::core::model::RepoScene* scene;
            const std::vector<double> offsetVector;

            GLC_World* createGLCWorld(
                repo::core::model::RepoScene     *scene,
                std::map<QString, GLC_Mesh*>     &meshMap,
                std::map<QString, GLC_Material*> &matMap,
                 std::vector<GLC_Light*> &lights
                    );

            void createGLCLights(
                repo::core::model::RepoScene     *scene,
                 std::vector<GLC_Light*>          &lights
                    );

			GLC_3DRep* convertGLCCamera(
				const repo::core::model::CameraNode *camera);

            GLC_Light* convertGLCLight(
                     const repo::core::model::LightNode *light,
                    const unsigned int &ind
                    );

			GLC_Material* convertGLCMaterial(
				const repo::core::model::MaterialNode   *material,
				std::map<repoUUID, std::vector<GLC_Texture*>> &mapTexture);

			GLC_3DRep* convertGLCMesh(
				const repo::core::model::MeshNode        *mesh,
                std::map<repoUUID, std::vector<GLC_Material*>> &mapMaterials,
                 std::map<QString, GLC_Material*> &matMap);


			GLC_Texture* convertGLCTexture(
				const repo::core::model::TextureNode *texture);

			QList<GLuint> createGLCFaceList(
                const std::vector<repo_face_t> &faces,
				const QVector<GLfloat>         &vertices,
				const int32_t &start = -1,
				const int32_t &end = -1);

			GLC_3DRep* createGLCMesh(
				const repo::core::model::RepoScene *scene,
				const repo::core::model::MeshNode   *node);

			QVector<GLfloat> createGLCVector(
                const std::vector<repo_vector_t> &vec
				);

			QVector<GLfloat> createGLCVector(
                const std::vector<repo_vector2d_t> &vec
				);

			QVector<GLfloat> createGLCVector(
                const std::vector<repo_color4d_t> &col
				);

			QColor toQColor(const std::vector<float> &c, float scale = 1.f);
			//! Number of jobs to be completed.
			uint32_t jobsCount;

			//! Number of jobs already done.
			uint32_t done;

		}; // end class

	} // end namespace gui
} // end namespace repo


