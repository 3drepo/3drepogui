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

#include "repo_renderer_abstract.h"

//------------------------------------------------------------------------------
#include <QOpenGLFunctions>
#include <GLC_Factory>
#include <GLC_Light>
#include <GLC_Material>
#include <GLC_Viewport>
#include <GLC_World>
#include <GLC_MoverController>
#include <GLC_Exception>
#include <GLC_Shader>
#include <GLC_RenderProperties>
#include <GLC_Material>
#include <GLC_3DWidgetManager>
#include <GLC_FlyMover>
#include "geometry/glc_mesh.h"
//------------------------------------------------------------------------------

namespace repo {
	namespace gui {
		namespace renderer {

			class GLCRenderer : public AbstractRenderer
			{
            Q_OBJECT
			public:
				/**
				* Instantiate a new GLCRenderer
				*/
				GLCRenderer();

				~GLCRenderer();
				
				/**
				* Delete shaders
				*/
				virtual void deleteShaders(QOpenGLContext *context);

				/**
				* Get information about the current camera settings
				* @return returns camera settings in the form of CameraNode
				*/
				virtual CameraSettings getCurrentCamera();

				/**
				* Initialise renderer
				*/
				virtual void initialize();

				/**
				* Load model into renderer
				* @param scene Scene to load
				*/
				virtual void loadModel(repo::core::model::RepoScene *scene);

				/**
				* Render scene
				* @param painter for painting Info (pass in nullptr if disabled)
				* @param screenHeight (ignored if painter is null)
				* @param screenWidth (ignored if painter is null)
				*/
				virtual void render(QPainter *painter,
					const int &screenHeight = 100,
					const int &screenWidth = 100);

				/**
				* Resize the viewing window
				* @param width new width
				* @param height new height
				*/
				virtual void resizeWindow(const int &width, const int &height);

				/**
				* Set activiation flag
				* @param flag true/false
				*/
				virtual void setActivationFlag(const bool &flag);

				/**
				* Set shader to the given file and initialise it
				* @param vertexFile vertex shader file
				* @param fragmentFile fragment shader file
				*/
				virtual void setAndInitShaders(QFile &vertexFile, 
					QFile &fragmentFile, QOpenGLContext *context);

				/**
				* Set camera settings of the current camera
				* @param camera camera settings to convert to
				*/
				virtual void setCamera(const CameraSettings &camera);

				/**
				*Sets a camera view from a pre-defined set of possibilities
				* @param view predfined view to set to
				*/
				virtual void setCamera(const CameraView &view);

				/**
				* Set the window background colour to the specified colour
				* @param color colour to change to
				*/
				virtual void setBackgroundColor(const QColor &color);

				void setGLCWorld(repo::core::model::RepoScene *scene, GLC_World &world);

			protected:				

				/**
				* Given a pointer to GLC_Camera, convert it into a CameraSettings.
				* @param cam GLC_Camera
				* @return returns a CameraSettings
				*/
				CameraSettings convertToCameraSettings(GLC_Camera *cam);


				/**
				* paint info
				* @param painter for painting Info (pass in nullptr if disabled)
				* @param screenHeight (ignored if painter is null)
				* @param screenWidth (ignored if painter is null)
				*/
				virtual void paintInfo(QPainter *painter,
					const int &screenHeight = 100,
					const int &screenWidth = 100);

				//! List of available shaders.
				QList<GLC_Shader*> shaders;

				GLC_3DWidgetManager glc3DWidgetManager;
				GLC_World glcWorld; //! 3D world, the main scene to render.				
				GLC_Light glcLight; //! Main light of the scene.				
				GLC_Viewport glcViewport; //! The viewport, in GLC lib attributed as glView.
				GLC_3DViewCollection glcViewCollection; //! The main collection of auxiliary objects (such as bboxes).				
				GLC_3DViewCollection glcUICollection; //! The main collection of UI components (such as axes).
				GLC_MoverController glcMoverController; //! The navigation controller of the scene (arc ball, fly etc).
				glc::RenderFlag renderingFlag; //! Rendering flag.

				//! Globally applied shader ID.
				GLuint shaderID;
				

			}; // end class
		} //end namespace renderer
	} // end namespace gui
} // end namespace repo

