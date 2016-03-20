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
#ifdef __APPLE_CC__
#include <OpenGL/OpenGL.h>
#endif

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
#include <GLC_CuttingPlane>
#include <GLC_Plane>
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
				* Recursively extracts meshes from a given occurrence. 
				* Call with a root node.
				* @param startOccurance root occurance of the (sub)tree you wish to start from
				*/
				void extractMeshes(GLC_StructOccurrence* startOccurance);

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
				* Increase velocity
				* @param vel velocity delta
				* @return returns true upon success
				*/
				virtual bool increaseFlyVelocity(const float &vel);

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
				* Navigate around the model
				* @param x position in x
				* @param y position in y
				*/
				virtual bool move(const int &x, const int &y);

				/**
				* Change rendering mode
				* @param mode to render in.
				*/
                virtual void setRenderingMode(const RenderMode &mode);

				/**
				* Set the colour of the mesh given its name
				* @param name name of mesh
				* @param color color of change to
				*/
				virtual void setMeshColor(
					const repoUUID &uniqueID,
					const qreal &opacity,
					const QColor &color);

				/**
				* Start navigate around the model
				* @param mode which navigation mode
				* @param x position in x
				* @param y position in y
				*/
				virtual void startNavigation(const NavMode &mode, const int &x, const int &y);

				/**
				* Stop navigation
				*/
				virtual void stopNavigation();

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
				* Reset mesh colours to its original
				*/
				virtual void resetColors();

				/**
				* Resize the viewing window
				* @param width new width
				* @param height new height
				*/
				virtual void resizeWindow(const int &width, const int &height);
				
				/**
				* Select a component given a position. This will highlight the component
				* @param x position in x
				* @param y position in y
				* @param multiSelection if multiple objects should be highlighted
				*/
				virtual void selectComponent(const int &x, const int &y, bool multiSelection);

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

				/**
				* Set the world to render to be the world provided
				* @param world the world to render
				*/
				void setGLCWorld(GLC_World &world);

public slots :

				/**
				* Toggle between show/hide octree
				*/
				virtual void toggleOctree();

				/**
				* Toggle between different projection modes
				*/
				virtual void toggleProjection() ;

				/**
				* Toggle Select/Unselect all objects that are currently visible
				*/
				virtual void toggleSelectAll();

				/**
				* Toggle between show/hide the wireframe
				*/
				virtual void toggleWireframe();

                void updateClippingPlane();

                void updateClippingPlane(Axis axis, double value);

                void setClippingPlaneVisibility(bool on);

				/**
				* Zoom/unzoom camera
				* @param zoom zoom factor
				*/

                virtual void zoom(const float &zoom);

			protected:				


                GLC_CuttingPlane * createCuttingPlane(const GLC_Point3d &centroid, const GLC_Point3d &normal, double l1, double l2);

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
				std::map<QString, GLC_Mesh*> meshMap;
				std::map<QString, GLC_Material*> matMap;
				std::map<GLC_Material*, GLC_Material> changedMats; //Map the pointer of the GLC material that has been changed to the original
				glc::RenderFlag renderingFlag; //! Rendering flag.
				bool isWireframe;

                //! Globally applied clipping plane IDs
                std::vector<GLC_CuttingPlane *> clippingPlaneWidgets;

                //! Clipping plane
                GLC_Plane* clippingPlane;

				//! Globally applied shader ID.
				GLuint shaderID;
				

			}; // end class
		} //end namespace renderer
	} // end namespace gui
} // end namespace repo

