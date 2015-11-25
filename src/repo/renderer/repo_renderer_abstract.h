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

#include <QObject>
#include <QOpenGLFunctions>
#include <QFile>
#include <QPainter>
#include <repo/core/model/collection/repo_scene.h>
#include <repo/core/model/repo_node_utils.h>

#include "../../renderers/repo_fpscounter.h"

namespace repo {
	namespace gui {
		namespace renderer {

			//! Standard camera positions type.
			enum class CameraView { BACK, BOTTOM, FRONT, ISO, LEFT, RIGHT, TOP };
			
			struct CameraSettings
			{
				repo_vector_t eye;
				repo_vector_t target;
				repo_vector_t up;
			};

			class AbstractRenderer : public QObject, protected QOpenGLFunctions
			{
                            Q_OBJECT
			public:
				AbstractRenderer();

				~AbstractRenderer();

				/**
				* Delete shaders
				*/
				virtual void deleteShaders(QOpenGLContext *context)  = 0;

				/**
				* Get information about the current camera settings
				* @return returns camera settings in the form of CameraSettings
				*/
				virtual CameraSettings getCurrentCamera() = 0;

				/**
				* Initialise renderer
				*/
				virtual void initialize() = 0;

				/**
				* Load model into renderer
				* @param scene Scene to load
				*/
				virtual void loadModel(repo::core::model::RepoScene *scene) = 0;

				/**
				* Render scene
				* @param painter for painting Info (pass in nullptr if disabled)
				* @param screenHeight (ignored if painter is null)
				* @param screenWidth (ignored if painter is null)
				*/
				virtual void render(QPainter *painter, 
					const int &screenHeight = 100,
					const int &screenWidth = 100) = 0;


				/**
				* Resize the viewing window
				* @param width new width
				* @param height new height
				*/
				virtual void resizeWindow(const int &width, const int &height) = 0;

				/**
				* Set activiation flag
				* @param flag true/false
				*/
				virtual void setActivationFlag(const bool &flag) = 0;

				/**
				* Set shader to the given file and initialise it
				* @param vertexFile vertex shader file
				* @param fragmentFile fragment shader file
				*/
				virtual void setAndInitShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context) = 0;

				/**
				* Set camera settings of the current camera
				* @param camera camera settings to convert to
				*/
				virtual void setCamera(const CameraSettings &camera) = 0;

				/**
				*Sets a camera view from a pre-defined set of possibilities
				* @param view predfined view to set to
				*/
				virtual void setCamera(const renderer::CameraView& view) = 0;

				/**
				* Set the window background colour to the specified colour
				* @param color colour to change to
				*/
				virtual void setBackgroundColor(const QColor &color) = 0;
						
			signals :
				/**
				* Signal that is emitted when a repaint is needed
				*/
				void repaintNeeded();

				/**
				* Signal that is emitted when a camera setting has changed
				*/
				void cameraChangedSignal(const CameraSettings &camera);

				void modelLoadProgress(int value, int maximum);

			public slots :
				void updateRenderer() { emit repaintNeeded(); };
				void broadcastCameraChange() { emit cameraChangedSignal(getCurrentCamera()); };
                                void workerProgress(int value, int maximum) { emit modelLoadProgress(value, maximum);};

			protected:
				RepoFPSCounter fpsCounter;





			}; // end class
		} //end namespace renderer
	} // end namespace gui
} // end namespace repo

