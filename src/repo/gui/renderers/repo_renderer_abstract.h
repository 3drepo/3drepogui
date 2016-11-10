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
#include <repo/manipulator/modelutility/spatialpartitioning/repo_spatial_partitioner_abstract.h>

#include "repo_fpscounter.h"

namespace repo {
namespace gui {
namespace renderer {

//! Standard camera positions type.
enum class CameraView { BACK, BOTTOM, FRONT, ISO, LEFT, RIGHT, TOP };
enum class NavMode { TURNTABLE, PAN, FLY, ZOOM, ORBIT, TSR, TARGET, HELICOPTER, HELICOPTERF, HELICOPTERV };
enum class RenderMode {POINT, WIREFRAME, WIREFRAME_SHADING, SHADING};
enum class Axis {X = 0, Y = 1, Z = 2};

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

    virtual ~AbstractRenderer();

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
     * Retrieve a 2D Image at the current camera view
     * with no lighting effects.
     * @param disableTexture if true, textures are removed from the image
     * @return returns a QImage of the current view with no lighting effects
     */
    virtual QImage getCurrentImageWithNoShading(
            const bool disableTexture, int w = 0, int h = 0) = 0;

    /**
     * Retrieve a 2D Image at the current camera view
     * in false coloring with no lighting effects.
     * @param idMap (return value) this function will fill in the ID mapping
     *              of decoded ints to unique ID of original meshes
     * @return returns a QImage of the current view with no lighting effects
     */
    virtual QImage getCurrentImageWithFalseColoring(
            std::vector<QString> &idMap, int w = 0, int h = 0) = 0;

    /**
    * Increase velocity
    * @param vel velocity delta
    * @return returns true upon success
    */
    virtual bool increaseFlyVelocity(const float &vel) = 0;

    /**
    * Initialise renderer
    */
    virtual void initialize() = 0;

    /**
    * Load model into renderer
    * @param scene Scene to load
    */
    virtual void loadModel(repo::core::model::RepoScene *scene,
                           const std::vector<double>    &offsetVector) = 0;


    /**
    * Navigate around the model
    * @param x position in x
    * @param y position in y
    * @return returns true upon success
    */
    virtual bool move(const int &x, const int &y) = 0;

    /**
    * Change rendering mode
    * @param mode to render in.
    */
    virtual void setRenderingMode(const RenderMode &mode) = 0;


    /**
    * Reset mesh colours to its original
    */
    virtual void resetColors() = 0;

    /**
     * Resets the view so that the entire bounding box is visible within frustrum.
     * @brief resetView
     */
    virtual void resetView() = 0;


    /**
    * Select a component given the position
    * @param x position in x
    * @param position in y
    * @param multiSelection true if allow multiple selection
    */
    virtual void selectComponent(QOpenGLContext *context, int x, int y, bool multiSelection) = 0;

    /**
    * Set the colour of the mesh given its name
    * @param color color of change to
    */
    virtual void setMeshColor(
            const repoUUID &uniqueID,
            const qreal &opacity,
            const QColor &color) = 0;

    /**
    * Turn on navigation mode
    * @param mode which navigation mode
    * @param x position in x
    * @param y position in y
    */
    virtual void startNavigation(const NavMode &mode, const int &x, const int &y) = 0;

    /**
    * Stop navigation
    */
    virtual void stopNavigation() = 0;

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
    virtual void setAndInitSelectionShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context) = 0;

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
    virtual int appendAndInitRenderingShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context) = 0;

    /**
     * Selects the given rendering shaders from the list of available compiled
     * shaders. Returns true if the requested id exists, false otherwise.
     *
     * @brief selectRenderingShaders
     * @param id
     * @return
     */
    virtual bool setRenderingShaders(int id) = 0;

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

    /**
     * @brief tilt the camera up/down
     * @param up if true, up, otherwise down
     */
    virtual void tiltUp(const bool up) = 0;

    virtual void toggleGenericPartitioning(
            const std::vector<repo_vector_t> &sceneBbox,
            const std::shared_ptr<repo_partitioning_tree_t> &tree) = 0;

    /**
    * Toggle between show/hide mesh bounding boxes
    */
    virtual void toggleMeshBoundingBoxes(
                         const repo::core::model::RepoScene *scene) = 0;

    /**
    * Toggle between show/hide octree
    */
    virtual void toggleOctree() = 0;

    /**
    * Toggle between different projection modes
    */
    virtual void toggleProjection() = 0;

    /**
    * Toggle Select/Unselect all objects that are currently visible
    */
    virtual void toggleSelectAll() = 0;

    /**
    * Toggle between show/hide the wireframe
    */
    virtual void toggleWireframe() = 0;

    /**
     * Updates the position of the clipping plane based on the axis and
     * percentage value from 0 to 1.
     *
     * @brief updateClippingPlane
     * @param axis
     * @param value
     */
    virtual void updateClippingPlane(Axis axis, double value, bool reverse = false) = 0;

    /**
     * Sets clipping plane visibility to true or false.
     *
     * @brief setClippingPlaneVisible
     * @param on
     */
    virtual void setClippingPlaneVisibility(bool on) = 0;

    /**
    * Zoom/unzoom camera
    * @param zoom zoom factor
    */

    virtual void zoom(const float &zoom) = 0;

signals :
    /**
    * Signal that is emitted when a repaint is needed
    */
    void repaintNeeded();

    void killWorker();

    void modelLoadProgress(int value, int maximum);

    void cameraChanged(const CameraSettings &camera);

public slots :

    void cancelOperations()
    { emit killWorker(); }

    void workerProgress(int value, int maximum)
    { emit modelLoadProgress(value, maximum); }

    void notifyCameraChange()
    { emit cameraChanged(getCurrentCamera()); }

protected:

    RepoFPSCounter fpsCounter;

}; // end class
} //end namespace renderer
} // end namespace gui
} // end namespace repo
