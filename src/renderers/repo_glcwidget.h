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


#ifndef REPO_GLCWIDGET_H
#define REPO_GLCWIDGET_H

//------------------------------------------------------------------------------
// Core
#include "graph/repo_graph_scene.h"

//------------------------------------------------------------------------------
// GUI
#include "repo_fpscounter.h"

//------------------------------------------------------------------------------
#include <QGLWidget>
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
#include "geometry/glc_mesh.h"
//------------------------------------------------------------------------------

namespace repo {
namespace gui {

class RepoGLCWidget : public QGLWidget
{
	Q_OBJECT

public :

    //--------------------------------------------------------------------------
	//
	// Static variables
	//
    //--------------------------------------------------------------------------

	//! Factor by which to zoom, e.g 1.2 for 120%.
	static const double ZOOM_FACTOR;

	//! Standard camera positions type.
    typedef enum { BACK, BOTTOM, FRONT, ISO, LEFT, RIGHT, TOP } CameraView;

    //--------------------------------------------------------------------------
	//
	// Constructor & destructor
	//
    //--------------------------------------------------------------------------

	//! Default constructor
    RepoGLCWidget(QWidget *p_parent, const QString &windowTitle = "");

	//! Destructor
	~RepoGLCWidget();

protected :

    //--------------------------------------------------------------------------
	//
	// OpenGL functions
	//
    //--------------------------------------------------------------------------

	//! Initializes the OpenGL context.
	void initializeGL();

	//! Initializes the OpenGL shaders (and compiles them).
	void initializeShaders();

	//! Renders the current scene.
	void paintGL();

	//! Displays the colored XYZ axes in the bottom right corner.
	void paintInfo();

	//! Resizes the OpenGL window.
	void resizeGL(int width, int height);

	void reframe(const GLC_BoundingBox& boundingBox);

public :

	void reframeOnSelection();

public slots :

    //--------------------------------------------------------------------------
	//
	// Slots
	//
    //--------------------------------------------------------------------------

    void broadcastCameraChange();

	//! Sets the camera of the view.
	void setCamera(const GLC_Camera&);

	//! Sets a camera view from a pre-defined set of possibilities.
	void setCamera(const CameraView&);

	//! Sets the per-vertex colours on a mesh identified by its string name.
	/*!
	 * Sets the per-vertex colours each as [r,g,b,a] on a single mesh. Hence,
	 * the size of the vector is 4 times the number of vertices in a mesh. If
	 * duplicate vertices are present, setting the same colour on every
	 * vertex belonging to a face colours the faces.
	 */
	void setGLCMeshColors(
		const QString&,
		const QVector<GLfloat>&,
		const bool repaint = true);

	void setGLCMeshColors(
		const float r,
		const float g,
		const float b,
		const float a = 1.0f);

	//! Sets the opacity of a glc mesh identified by its unique name.
	void setGLCMeshOpacity(const QString &, qreal opacity);

	/*! Sets the render properties on a glc occurrence identified by its name.
	 * This is can be used to force transparency by setting
	 * properties.setRenderingMode(glc::OverwriteTransparency);
	 * properties.setOverwriteTransparency(alpha);
	 */
	void setGLCOccurrenceRenderProperties(
		const QString &occurrenceName,
		const GLC_RenderProperties &);

	//! Sets the opacity of a glc occurrence identified by its name.
	void setGLCOccurrenceOpacity(
		const QString &,
		qreal opacity = 1.0,
		const QColor &color = Qt::gray);

	//! Sets the visibility of given occurence name
	void setGLCOccurenceVisibility(const QString &, bool visible);

	//! Sets the visibility of the XYZ axes
	void setInfoVisibility(const bool visible);

	//! Sets the background color of the 3D view and repaints.
	void setBackgroundColor(const QColor &color, const bool repaint = true);

signals :

	void cameraChangedSignal(const GLC_Camera&);

	void selectionChanged(const RepoGLCWidget *, std::vector<std::string>);

public :

	//! Sets the widget to hook with
	void linkCameras(const RepoGLCWidget *widget, const bool& link = true) const;

    //--------------------------------------------------------------------------
	//
	// Setters
	//
    //--------------------------------------------------------------------------

    //! Sets the 3D Repo scene for this widget.
    void setRepoScene(core::RepoGraphScene *repoScene);

    //! Sets the optimized 3D Repo scene for this widget.
    void setRepoSceneOptim(core::RepoGraphScene *repoSceneOptim);

    //! Sets the GLC World for this widget which is used for rendering purposes.
	void setGLCWorld(GLC_World);

	//! Sets the globally applied shader from the shaders list.
    void setShader(GLuint id) { shaderID = id; }

	//! Sets the rendering flag.
	inline void setRenderingFlag(glc::RenderFlag renderingFlag)
    { this->renderingFlag = renderingFlag; }

	//! Sets the rendering mode (GL_POINT, GL_LINE, GL_FILL)
	inline void setMode(GLenum mode)
	{ glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, mode);	}

	//! Adds a bounding box to the scene.
	/*!
	 * Takes the length of the bounding box in x, y and z and the transformation
	 * matrix to reposition the box from [0,0,0] to the desired location in space.
	 */
	void addBoundingBox(const double lx, const double ly, const double lz,
		const std::vector<double>& transformationMatrix);

	//! Removes and deletes all bounding boxes.
	void clearBoundingBoxes();

    void toggleOctree();

    void select(GLC_uint selectionID,
                bool multiSelection = false,
                bool unselectSelected = true,
                bool repaint = true);

    void select(const QString &meshName,
                bool multiSelection = false,
                bool unselectSelected = true,
                bool repaint = true);

    //--------------------------------------------------------------------------
	//
	// Getters
	//
    //--------------------------------------------------------------------------

	//! Returns the 3D Repo scene of this widget.
    core::RepoGraphScene *getRepoScene() const { return repoScene; }

	//! Returns the optimized 3D Repo scene of this widget.
    core::RepoGraphScene *getRepoSceneOptim() const { return repoSceneOptim; }

	//! Returns the GLC World of this widget.
    GLC_World getGLCWorld() const { return glcWorld; }

    int heightForWidth(int w) const { return w; }

	//! Returns the list of names of selected meshes if any.
	std::vector<std::string> getSelectionList() const;

    //! Returns currently selected Repo scene node if any, NULL otherwise.
    core::RepoNodeAbstract* getSelectedNode() const;

	//! Returns the glc meshes mapped by their unique string name.
    QHash<QString, GLC_Mesh*> getGLCMeshes() const { return glcMeshes; }

	//! Returns a list of unique mesh names.
    QList<QString> getGLCMeshNames() const { return glcMeshes.uniqueKeys(); }

	//! Returns a GLC mesh with given name, 0 if not found.
	GLC_Mesh * getGLCMesh(const QString &name) const;

	//! See https://bugreports.qt-project.org/browse/QTBUG-33186
	QImage renderQImage(int w, int h);

protected :

    //--------------------------------------------------------------------------
	//
	// User interaction
	//
    //--------------------------------------------------------------------------
	void keyPressEvent(QKeyEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);

	//! Mouse wheel event processing
	void wheelEvent(QWheelEvent*);

	//! Object selection processing
	void select(int x, int y, bool multiSelection, QMouseEvent* pMouseEvent);

    //--------------------------------------------------------------------------
	//
	// Scene management
	//
    //--------------------------------------------------------------------------

	//! Recursively extracts meshes from a given occurrence. Call with a root node.
	void extractMeshes(GLC_StructOccurence*);

    //--------------------------------------------------------------------------
	//
	// Private variables
	//
    //--------------------------------------------------------------------------

	//! 3D scene, the scene graph representation to store in the DB.
    core::RepoGraphScene *repoScene;

	//! 3D scene, the optimized scene graph representation to store in the DB.
    core::RepoGraphScene *repoSceneOptim;

	//! 3D world, the main scene to render.
	GLC_World glcWorld;

	//! Main light of the scene.
	GLC_Light glcLight;

	//! The viewport, in GLC lib attributed as glView.
	GLC_Viewport glcViewport;


	//! The main collection of auxiliary objects (such as bboxes).
	GLC_3DViewCollection glcViewCollection;

	//! The main collection of UI components (such as axes).
	GLC_3DViewCollection glcUICollection;

	//! The navigation controller of the scene (arc ball, fly etc).
	GLC_MoverController glcMoverController;

	//! Dictionary of meshes pointed to by their associated unique name.
	QHash<QString, GLC_Mesh*> glcMeshes;

	//! Dictionary of mesh ids pointed to by their associated unique name.
	QHash<QString, GLC_uint> glcMeshesIds;

	QHash<QString, GLC_StructOccurence *> glcMeshOccurences;

	//! Dictionary of occurrences pointed to by their associated unique name.
	QHash<QString, GLC_StructOccurence *> glcOccurrences;

	//! True if wireframe is to be rendered, false otherwise.
	bool isWireframe;

	//! True is info is to be rendered, false otherwise.
	bool isInfoVisible;

	//! List of available shaders.
	QList<GLC_Shader*> shaders;

	//! Globally applied shader ID.
	GLuint shaderID;

	//! Rendering flag.
	glc::RenderFlag renderingFlag;

    RepoFPSCounter fpsCounter;


}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_GLCWIDGET_H
