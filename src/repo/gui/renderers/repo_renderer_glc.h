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
                 * Retrieve a 2D Image at the current camera view
                 * with no lighting effects.
                 * @param disableTexture if true, textures are removed from the image
                 * @return returns a QImage of the current view with no lighting effects
                 */
                virtual QImage getCurrentImageWithNoShading(
                        const bool disableTexture, int w = 0, int h = 0);

                /**
                 * Retrieve a 2D Image at the current camera view
                 * in false coloring with no lighting effects.
                 * @param idMap (return value) this function will fill in the ID mapping
                 *              of decoded ints to unique ID of original meshes
                 * @return returns a QImage of the current view with no lighting effects
                 */
                virtual QImage getCurrentImageWithFalseColoring(
                        std::vector<QString> &idMap, int w = 0, int h = 0);

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
                virtual void loadModel(repo::core::model::RepoScene *scene,
                                       const std::vector<double> &offsetVector);

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


                virtual void setMeshColor(
                        const QString &uuidString,
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

                virtual void resetView();

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
                virtual void selectComponent(QOpenGLContext *context, int x, int y, bool multiSelection);


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
                virtual void setAndInitSelectionShaders(QFile &vertexFile,
                    QFile &fragmentFile, QOpenGLContext *context);

                /**
                 * Appends and compiles given vertex and fragments shaders which can then
                 * be selected based on their assigned number in the shaders list.
                 * Returns the assigned selection number
                 *
                 * @brief appendAndInitRenderingShaders
                 * @param vertexFile
                 * @param fragmentFile
                 * @param context
                 * @return
                 */
                virtual int appendAndInitRenderingShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context);

                /**
                 * Selects the given rendering shaders from the list of available compiled
                 * shaders. Returns true if the requested id exists, false otherwise.
                 *
                 * @brief selectRenderingShaders
                 * @param id
                 * @return
                 */
                virtual bool setRenderingShaders(int id);


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
                void setGLCWorld(GLC_World &world,
                                 std::map<QString, GLC_Mesh*>     &_meshMap,
                                 std::map<QString, GLC_Material*> &_matMap,
                                 std::vector<QString> &_idmap);

                virtual void tiltUp(const bool up);
public slots :

                /**
                * Toggle between show/hide genericSpatialPartitioning
                */
                virtual void toggleGenericPartitioning(
                        const std::vector<repo_vector_t> &sceneBbox,
                        const std::shared_ptr<repo_partitioning_tree_t> &tree);


                /**
                * Toggle between show/hide mesh bounding boxes
                */
                virtual void toggleMeshBoundingBoxes(
                                     const repo::core::model::RepoScene *scene);
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

                void updateClippingPlane(Axis axis, double value, bool reverse = false);

                void setClippingPlaneVisibility(bool on);

				/**
				* Zoom/unzoom camera
				* @param zoom zoom factor
				*/

                virtual void zoom(const float &zoom);

                GLfloat getPointSize();

			protected:				


                GLC_CuttingPlane * createCuttingPlane(const GLC_Point3d &centroid, const GLC_Point3d &normal, double l1, double l2);

                void createMeshBBoxes(
                        const repo::core::model::RepoScene            *scene,
                        const repo::core::model::RepoScene::GraphType &gType,
                        const repo::core::model::RepoNode             *node,
                        const std::vector<float>                      &matrix,
                         GLC_Material                            *mat);

                void createSPBoxes(
                        const std::shared_ptr<repo_partitioning_tree_t> &tree,
                        const std::vector<std::vector<float>>   &currentBbox,
                         GLC_Material                      *mat
                        );

                /**
                 * Change the colour of the material
                 * @param uuidString unique id of the mesh (or submesh) to change
                 * @param newMat the new material to change to
                 */
                void changeMeshMaterial(
                        const QString &uuidString,
                        const GLC_Material &newMat);

				/**
				* Given a pointer to GLC_Camera, convert it into a CameraSettings.
				* @param cam GLC_Camera
				* @return returns a CameraSettings
				*/
				CameraSettings convertToCameraSettings(GLC_Camera *cam);

                /**
                 * Disable selection mode and revert material
                 * settings to its original state
                 */
                void disableSelectionMode();

                /**
                 * Enable selection mode and use custom false
                 * coloring unless otherwise stated
                 * @param useCurrentMaterials use currently assigned materials
                 *          instead ofcustom false coloring
                 * @return returns a vector of ID related to custom false coloring,
                 *                  empty vector if failed or useCurrentMaterials is set to true
                 */
                std::vector<QString> enableSelectionMode(
                        const bool useCurrentMaterials);

                /**
                 * Retrieve a 2D Image at the current camera view
                 * with no lighting effects.
                 * Note: false coloring takes precedence over disableTexture
                 * @param disableTexture remove texture on image
                 * @param useFalseColoring use custom false coloring on image
                 * @param idMap if useFalseColoring is set to true, fill up idMap
                 *                with decoded int to meshes' unique id mapping
                 * @return returns a QImage of the current view with no lighting effects
                 */
                QImage getCurrentImageWithNoShading(
                        const bool disableTexture,
                        const bool useFalseColoring,
                        std::vector<QString> &idMap, int w = 0, int h = 0);

                /**
                 * Highlight the mesh (or the submesh) that has the
                 * given ID
                 * @param meshId
                 */
                void highlightMesh(
                        const QString &meshId);

				/**
				* paint info
				* @param painter for painting Info (pass in nullptr if disabled)
				* @param screenHeight (ignored if painter is null)
				* @param screenWidth (ignored if painter is null)
				*/
				virtual void paintInfo(QPainter *painter,
					const int &screenHeight = 100,
					const int &screenWidth = 100);

                /**
                 * Revert mesh material back to its original properties
                 * @param uuidString mesh unique id in QString format
                 */
                void revertMeshMaterial(
                        const QString &uuidString);


				virtual void toggleHighLight(
                    const QString &meshId,
                        const bool &highLight);

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
                std::vector<QString> idmap;
                repo::core::model::RepoScene *scene;
                glc::RenderFlag renderingFlag; //! Rendering flag.

				bool isWireframe;

                //! Globally applied clipping plane IDs
                std::vector<GLC_CuttingPlane *> clippingPlaneWidgets;
                std::set<QString> currentlyHighLighted;
				QString lastHighLighted;

                //! Clipping plane
                GLC_Plane* clippingPlane;

                bool clippingPlaneReverse;

				//! Globally applied shader ID.
				GLuint shaderID;

                std::vector<double> offset;		

			}; // end class
		} //end namespace renderer
	} // end namespace gui
} // end namespace repo
