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

//------------------------------------------------------------------------------
// Core
#include <repo/core/model/collection/repo_scene.h>

//------------------------------------------------------------------------------
// GUI
#include "../renderers/repo_renderer_abstract.h"
#include "repo_widget_rendering_abstract.h"

//------------------------------------------------------------------------------
#include <QGLWidget>
#include <QOpenGLWidget>
//------------------------------------------------------------------------------


namespace repo {
namespace gui {
namespace widget {

enum class Renderer { GLC };

class Rendering3DWidget : public QOpenGLWidget, public QOpenGLFunctions, public RenderingAbstractWidget
{
    Q_OBJECT

public:

    //--------------------------------------------------------------------------
    //
    // Static variables
    //
    //--------------------------------------------------------------------------

    //! Factor by which to zoom, e.g 1.2 for 120%.
    static const double ZOOM_FACTOR;

    //--------------------------------------------------------------------------
    //
    // Constructor & destructor
    //
    //--------------------------------------------------------------------------

    //! Default constructor
    Rendering3DWidget(
            QWidget *p_parent,
            Renderer rendererType,
            renderer::NavMode navMode,
            const QString &windowTitle = "");

    //! Destructor
    ~Rendering3DWidget();

protected:

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

    //void reframe(const GLC_BoundingBox& boundingBox);

public:

    //void reframeOnSelection();

public slots :

    //--------------------------------------------------------------------------
    //
    // Slots
    //
    //--------------------------------------------------------------------------

    //! Sets the camera of the view.
    void setCamera(const renderer::CameraSettings& camera);

    //! Sets a camera view from a pre-defined set of possibilities.
    void setPredefinedCamera(const repo::gui::renderer::CameraView&);

    void rendererProgress(int value, int maximum) { emit modelLoadProgress(value, maximum); }

    void cancelOperations() { emit cancelRenderingOps(); }


    /**
                * Reset mesh colours to its original
                */
    void resetColors() { renderer->resetColors(); update(); }


    /**
                * Set the colour of the mesh given its name
                * @param name name of mesh
                * @param color color of change to
                */
    void setMeshColor(
            const repoUUID &uniqueID,
            const qreal &opacity,
            const QColor &color);

    //! Sets the visibility of the XYZ axes
    void setInfoVisibility(const bool visible);

    //! Sets the background color of the 3D view and repaints.
    void setBackgroundColor(const QColor &color, const bool repaint = true);

    void updateClippingPlane(repo::gui::renderer::Axis axis, double value);

signals:

    void cancelRenderingOps();

    void modelLoadProgress(int value, int maximum);

    void selectionChanged(const Rendering3DWidget *, std::vector<std::string>);

public:

    //! Sets the widget to hook with
    void linkCameras(const Rendering3DWidget *widget, const bool& link = true) const;

    //--------------------------------------------------------------------------
    //
    // Setters
    //
    //--------------------------------------------------------------------------

    //! Sets the 3D Repo scene for this widget.
    void setRepoScene(repo::core::model::RepoScene *repoScene);

    //! Sets the GLC World for this widget which is used for rendering purposes.
    /*void setGLCWorld(GLC_World);*/

    //! Sets the globally applied shader from the shaders list.
    //void setShader(GLuint id) { shaderID = id; }

    //! Sets the rendering mode (GL_POINT, GL_LINE, GL_FILL)
    //inline void setMode(GLenum mode)
    //{
    //	glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, mode);
    //}

    //! Adds a bounding box to the scene.
    /*!
                * Takes the length of the bounding box in x, y and z and the transformation
                * matrix to reposition the box from [0,0,0] to the desired location in space.
                */
    //void addBoundingBox(const double lx, const double ly, const double lz,
    //	const std::vector<double>& transformationMatrix);

    ////! Removes and deletes all bounding boxes.
    //void clearBoundingBoxes();


    //void select(const QString &meshName,
    //	bool multiSelection = false,
    //	bool unselectSelected = true,
    //	bool repaint = true);

    //--------------------------------------------------------------------------
    //
    // Getters
    //
    //--------------------------------------------------------------------------

    //! Returns the 3D Repo scene of this widget.
    repo::core::model::RepoScene *getRepoScene() const { return repoScene; }

    //! Returns the GLC World of this widget.
    //GLC_World getGLCWorld() const { return glcWorld; }

    int heightForWidth(int w) const { return w; }

    //! Returns the list of names of selected meshes if any.
    std::vector<std::string> getSelectionList() const;

    //! Returns currently selected Repo scene node if any, NULL otherwise.
    repo::core::model::RepoNode* getSelectedNode() const;

    ////! Returns the glc meshes mapped by their unique string name.
    //QHash<QString, GLC_Mesh*> getGLCMeshes() const { return glcMeshes; }

    ////! Returns a list of unique mesh names.
    //QList<QString> getGLCMeshNames() const { return glcMeshes.uniqueKeys(); }

    ////! Returns a GLC mesh with given name, 0 if not found.
    //GLC_Mesh * getGLCMesh(const QString &name) const;

    //! See https://bugreports.qt-project.org/browse/QTBUG-33186
    QImage renderQImage(int w, int h);

    QImage renderFrameBufferQImage(int w, int h, GLvoid *data);

protected:

    //--------------------------------------------------------------------------
    //
    // User interaction
    //
    //--------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
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
    // Renderer management
    //
    //--------------------------------------------------------------------------
    /**
                * Instantiate renderer
                * It will instantiate the renderer object with the right inherited renderer type
                * @param rendType renderer Type
                */
    void instantiateRenderer(Renderer rendType);



    //--------------------------------------------------------------------------
    //
    // Private variables
    //
    //--------------------------------------------------------------------------

    //! 3D scene, the scene graph representation to store in the DB.
    repo::core::model::RepoScene *repoScene;

    //! Renderer for this instance of rendering widget
    renderer::AbstractRenderer* renderer;

    ////! Dictionary of meshes pointed to by their associated unique name.
    //QHash<QString, GLC_Mesh*> glcMeshes;

    ////! Dictionary of mesh ids pointed to by their associated unique name.
    //QHash<QString, GLC_uint> glcMeshesIds;

    //QHash<QString, GLC_StructOccurrence *> glcMeshOccurences;

    ////! Dictionary of occurrences pointed to by their associated unique name.
    //QHash<QString, GLC_StructOccurrence *> glcOccurrences;

    //! True if wireframe is to be rendered, false otherwise.
    bool isWireframe;

    //! True is info is to be rendered, false otherwise.
    bool isInfoVisible;

    bool mousePressed;

}; // end


} //end namepsace widgets

} // end namespace gui
} // end namespace repo

