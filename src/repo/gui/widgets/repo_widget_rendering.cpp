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


#include "repo_widget_rendering.h"
#include "../primitives/repo_fontawesome.h"
#include "../primitives/repo_color.h"
#include "../../logger/repo_logger.h"
#include "../renderers/repo_renderer_glc.h"
//------------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------
#include <QFile>
#include <QFont>
#include <QTime>
#include <QTimer>
#include <QScreen>
//------------------------------------------------------------------------------


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//------------------------------------------------------------------------------
// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

#if defined(Q_OS_LINUX)
#include <unistd.h>
#include <time.h>

uint32_t GetTickCount()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned tick = ts.tv_nsec / 1000000;
    tick += ts.tv_sec * 1000;
    return tick;
}
#endif


using namespace repo::gui::widget;

//------------------------------------------------------------------------------
//
// Static variables
//
//------------------------------------------------------------------------------
const double Rendering3DWidget::ZOOM_FACTOR = 1.2;

//QList<GLC_Shader*> Rendering3DWidget::shaders;

Rendering3DWidget::Rendering3DWidget(
        QWidget* parent,
        Renderer rType,
        repo::gui::renderer::NavMode navMode,
        repo::RepoController *controller,
        const QString& windowTitle)
    : QOpenGLWidget(parent)
    , RenderingAbstractWidget(navMode)
    , isWireframe(false)
    , isInfoVisible(true)
    , repoScene(0)
     , controller(controller)
    , partition(std::shared_ptr<repo_partitioning_tree_t>(nullptr))

{
    // TODO: add to GUI settings
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4); // antialiasing/multisampling
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1); // V sync
    setFormat(format); // must be called before the widget or its parent window gets shown

    //--------------------------------------------------------------------------
    // Default settings
    this->setWindowTitle(windowTitle);
    this->setToolTip(windowTitle);
    this->setWindowIcon(repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                            repo::gui::primitive::RepoFontAwesome::fa_cube));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFocusPolicy(Qt::StrongFocus);

    // To register mouse events on move (by default only on press).
    this->setMouseTracking(true);

    instantiateRenderer(rType);
}

Rendering3DWidget::~Rendering3DWidget()
{
    makeCurrent();
    renderer->deleteShaders(context());

    if (repoScene)
        delete repoScene;

    doneCurrent();

    if (renderer)
        delete renderer;
}

//------------------------------------------------------------------------------
//
// OpenGL
//
//------------------------------------------------------------------------------
void Rendering3DWidget::initializeGL()
{
    //--------------------------------------------------------------------------
    // Renderer initialisation
    renderer->initialize();
    initializeShaders();
    renderer->setActivationFlag(true);


}

void Rendering3DWidget::instantiateRenderer(Renderer rendType)
{
    if (rendType == Renderer::GLC)
    {
        renderer = new renderer::GLCRenderer();
        //----------------------------------------------------------------------
        QObject::connect(renderer, SIGNAL(repaintNeeded()),
                         this, SLOT(update()));
    }
    else
    {
        repoLogError("Unsupported Rendering type: " + (int) rendType);
    }
}

void Rendering3DWidget::initializeShaders()
{
    makeCurrent();

    QFile selectVertexShaderFile(":/shaders/select.vert");
    QFile selectFragmentShaderFile(":/shaders/select.frag");
    //--------------------------------------------------------------------------
    QFile defaultVertexShaderFile(":/shaders/default.vert");
    QFile defaultFragmentShaderFile(":/shaders/default.frag");

    QFile goochVertexShaderFile(":/shaders/goochShading.vert");
    QFile goochFragmentShaderFile(":/shaders/goochShading.frag");

    QFile minnaertVertexShaderFile(":/shaders/minnaert.vert");
    QFile minnaertFragmentShaderFile(":/shaders/minnaert.frag");

    QFile toonVertexShaderFile(":/shaders/toon.vert");
    QFile toonFragmentShaderFile(":/shaders/toon.frag");

    QFile phongVertexShaderFile(":/shaders/phong.vert");
    QFile phongFragmentShaderFile(":/shaders/phong.frag");


    if (selectVertexShaderFile.exists() && selectFragmentShaderFile.exists())
    {
        try
        {
            renderer->setAndInitSelectionShaders(selectVertexShaderFile, selectFragmentShaderFile, context());
            //------------------------------------------------------------------
            renderer->appendAndInitRenderingShaders(defaultVertexShaderFile, defaultFragmentShaderFile, context());
            renderer->appendAndInitRenderingShaders(goochVertexShaderFile, goochFragmentShaderFile, context());
            renderer->appendAndInitRenderingShaders(minnaertVertexShaderFile, minnaertFragmentShaderFile, context());
            renderer->appendAndInitRenderingShaders(toonVertexShaderFile, toonFragmentShaderFile, context());
            renderer->appendAndInitRenderingShaders(phongVertexShaderFile, phongFragmentShaderFile, context());
        }
        catch (GLC_Exception e)
        {
            repoLogError("Init shader failed " + std::string(e.what()));
        }
    }
}

void Rendering3DWidget::paintGL()
{
    if (isInfoVisible)
    {
        QPainter painter(this);
        QSize screenSize(size());

        renderer->render(&painter, screenSize.height(), screenSize.width());
    }
    else
        renderer->render(nullptr);

    // Continuous rendering
    //    QTimer::singleShot(0, this, SLOT(update()));
}


void Rendering3DWidget::resizeGL(int width, int height)
{
    makeCurrent();
    renderer->resizeWindow(width, height);
}

//------------------------------------------------------------------------------
//
// Public slots
//
//------------------------------------------------------------------------------

void Rendering3DWidget::setCamera(const repo::gui::renderer::CameraSettings &camera)
{
    renderer->setCamera(camera);
    update();
}
void Rendering3DWidget::setPredefinedCamera(const repo::gui::renderer::CameraView& view)
{
    renderer->setCamera(view);
    update();
}

void Rendering3DWidget::setMeshColor(
        const repoUUID &uniqueID,
        const qreal &opacity,
        const QColor &color)
{
    renderer->setMeshColor(uniqueID, opacity, color);
    update();
}

void Rendering3DWidget::setInfoVisibility(const bool visible)
{
    isInfoVisible = visible;
}

void Rendering3DWidget::setBackgroundColor(
        const QColor &color,
        const bool isUpdate)
{
    renderer->setBackgroundColor(color);
    if (isUpdate)
        update();
}

void Rendering3DWidget::setClippingPlaneVisibility(bool on)
{
    renderer->setClippingPlaneVisibility(on);
    update();
}

void Rendering3DWidget::updateClippingPlane(repo::gui::renderer::Axis axis, double value, bool reverse)
{
    renderer->updateClippingPlane(axis, value, reverse);
    update();
}

void Rendering3DWidget::linkCameras(
        const Rendering3DWidget *widget,
        const bool & on) const
{
    if (on)
    {
        QObject::connect(renderer, &renderer::AbstractRenderer::cameraChanged,
                         widget, &Rendering3DWidget::setCamera);

        QObject::connect(this,
                         &repo::gui::widget::Rendering3DWidget::keyPressed,
                         widget,
                         &repo::gui::widget::Rendering3DWidget::keyPressEvent);

        // TODO: align all views
        //        renderer->notifyCameraChange();
    }
    else
    {
        QObject::disconnect(renderer, &renderer::AbstractRenderer::cameraChanged,
                            widget, &Rendering3DWidget::setCamera);

        QObject::disconnect(this,
                         &repo::gui::widget::Rendering3DWidget::keyPressed,
                         widget,
                         &repo::gui::widget::Rendering3DWidget::keyPressEvent);
    }
}

//------------------------------------------------------------------------------
//
// Setters
//
//------------------------------------------------------------------------------

void Rendering3DWidget::setRepoScene(
        repo::core::model::RepoScene *repoScene,
        const std::vector<double>    &offsetVector)
{

    connect(renderer, &renderer::AbstractRenderer::modelLoadProgress,
            this, &Rendering3DWidget::rendererProgress);

    connect(this, &Rendering3DWidget::cancelRenderingOps,
            renderer, &renderer::AbstractRenderer::cancelOperations);
    if (repoScene)
    {
        if (this->repoScene)
            delete this->repoScene;

        this->repoScene = repoScene;

        if (renderer)
        {
            renderer->loadModel(repoScene, offsetVector);
        }
        else
        {
            repoLogError("Unable to load Scene for render as renderer is not initialised!");
        }
    }
    else
    {
        repoLogError("Failed to load repoScene!");
    }
}


//------------------------------------------------------------------------------
//
// Getters
//
//------------------------------------------------------------------------------


QImage Rendering3DWidget::renderQImage(int w, int h)
{
    // This does not work with antialiasing (format.setSamples) on
    //------------------------------------------------------------------------------
    //    makeCurrent();
    //    isInfoVisible = false;
    //    int oldW = width();
    //    int oldH = height();
    //    resize(w, h); // resize scene
    //    update(); // draw to the buffer
    //    QImage image = grabFramebuffer();
    //    isInfoVisible = true;
    //    resize(oldW, oldH);
    //    update();
    //    return image;

    return renderFrameBufferQImage(w, h, nullptr);
}


QImage Rendering3DWidget::renderFrameBufferQImage(int w, int h, GLvoid *data)
{
    // THIS IS IMPORTANT
    // http://stackoverflow.com/questions/31323749/easiest-way-for-offscreen-rendering-with-qopenglwidget

    // See https://bugreports.qt-project.org/browse/QTBUG-33186
    // See http://doc.qt.io/qt-5/qtquick-scenegraph-textureinthread-threadrenderer-cpp.html
    //    QOffscreenSurface surface;
    //    surface.create();
    //    context()->makeCurrent(&surface);

    makeCurrent();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    // Note that it is not possible to set more samples, aka antialiasing, here
    // as that will not produce any rendering!
    QOpenGLFramebufferObject m_fbo(w, h, format);
    resizeGL(w, h);

    m_fbo.bind();
    isInfoVisible = false;
    paintGL();
    isInfoVisible = true;

    // https://www.opengl.org/sdk/docs/man2/xhtml/glReadPixels.xml
    if (data)
        context()->functions()->glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, data);

    m_fbo.release();
    QImage image = m_fbo.toImage();
    m_fbo.bindDefault();
    doneCurrent();

    resizeGL(width(), height());
    //    surface.destroy();
    return image;
}

int Rendering3DWidget::getSelectedID(int x, int y)
{
    makeCurrent();
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    QOpenGLFramebufferObject fbo(size().width(), size().height(), format);

    GLsizei width= 4;
    GLsizei height= width;
    GLint newX= x - width / 2;
    GLint newY= (size().height() - y) - height / 2;
    if (newX < 0) newX= 0;
    if (newY < 0) newY= 0;


    fbo.bind();
    isInfoVisible = false;
    GLC_State::setSelectionMode(true);
    paintGL();
    GLC_State::setSelectionMode(false);
    isInfoVisible = true;

    const int squareSize= width * height;
    const GLsizei arraySize= squareSize * 4; // 4 -> R G B A
    QVector<GLubyte> colorId(arraySize);
    context()->functions()->glReadPixels(newX, newY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorId.data());

    fbo.release();
    fbo.bindDefault();
    doneCurrent();


    QHash<GLC_uint, int> idHash;
    QList<int> idWeight;

    // Find the most meaningful color
    GLC_uint returnId= 0;
    // There is nothing at the center
    int maxWeight= 0;
    int currentIndex= 0;
    for (int i= 0; i < squareSize; ++i)
    {
        GLC_uint id= glc::decodeRgbId(&colorId[i * 4]);
        if (idHash.contains(id))
        {
            const int currentWeight= ++(idWeight[idHash.value(id)]);
            if (maxWeight < currentWeight)
            {
                returnId= id;
                maxWeight= currentWeight;
            }
        }
        else if (id != 0)
        {
            idHash.insert(id, currentIndex++);
            idWeight.append(1);
            if (maxWeight < 1)
            {
                returnId= id;
                maxWeight= 1;
            }
        }
    }
    return returnId;
}

//------------------------------------------------------------------------------
//
// User interaction
//
//------------------------------------------------------------------------------
void Rendering3DWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Shift :
        this->setCursor(Qt::SizeAllCursor);
        break;
    case Qt::Key_Control :
        this->setCursor(Qt::PointingHandCursor);
        break;
    case Qt::Key_Minus:
    case Qt::Key_Underscore:
        renderer->zoom(1 / ZOOM_FACTOR);
        update();
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        renderer->zoom(ZOOM_FACTOR);
        update();
        break;
    case Qt::Key_A:
        if ((e->modifiers() == Qt::ControlModifier))
        {
            renderer->toggleSelectAll();
            update();
            break;
        }
        //case Qt::Key_C:
        //TODO:
        //	if (glIsEnabled(GL_CULL_FACE))
        //	{
        //		glDisable(GL_CULL_FACE);
        //		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        //	}
        //	else
        //	{
        //		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        //		glEnable(GL_CULL_FACE);
        //		//glFrontFace(false ? GL_CCW : GL_CW);
        //	}
//    case Qt::Key_D:
//    {
//        auto res = renderer->getCurrentImageWithNoShading(e->modifiers() == Qt::ControlModifier);
//        res.save("C:\\Users\\Carmen\\Desktop\\test.png");
//        break;
//    }
//    case Qt::Key_E:
//    {
//        std::vector<QString> idMap;
//        auto res = renderer->getCurrentImageWithFalseColoring(idMap);
//        res.save("C:\\Users\\Carmen\\Desktop\\test.png");
//        break;
//    }
    case Qt::Key_R:
    {
        renderer->resetView();
        update();
        break;
    }
    case Qt::Key_W:
    {
        renderer->toggleWireframe();
        update();
        break;
    }
    case Qt::Key_P:
        renderer->toggleProjection();
        update();
        break;
    case Qt::Key_Q:
    {
        QColor color = QColorDialog::getColor(Qt::white, this, "Color picker");
        if (color.isValid())
        {
            setBackgroundColor(color);
        }
        break;
    }
    case  Qt::Key_O:
    {
        renderer->toggleOctree();
        update();
        break;
    }

    case  Qt::Key_K:
    {
        if ((e->modifiers() == Qt::ControlModifier))
        {

            renderer->toggleMeshBoundingBoxes(repoScene);
            update();
        }
        else
        {
            if(!sceneBbox.size())
                sceneBbox = repoScene->getSceneBoundingBox();
            if(!partition)
                partition = controller->getScenePartitioning(repoScene);
            renderer->toggleGenericPartitioning(sceneBbox, partition);
            update();
        }

        break;
    }
    case Qt::Key_F1: // Points
    {
        renderer->setRenderingMode(renderer::RenderMode::POINT);
        update();
        break;
    }
    case Qt::Key_F2: // Triangle wireframe
    {
        renderer->setRenderingMode(renderer::RenderMode::WIREFRAME);
        update();
        break;
    }
    case Qt::Key_F3: // Shading with polygon wireframe
    {
        renderer->setRenderingMode(renderer::RenderMode::WIREFRAME_SHADING);
        update();
        break;
    }
    case Qt::Key_F4: // Shading
    {
        renderer->setRenderingMode(renderer::RenderMode::SHADING);
        update();
        break;
    }
    case Qt::Key_5 :
    {
        renderer->setRenderingShaders(0);
        update();
        break;
    }
    case Qt::Key_6:
    {
        renderer->setRenderingShaders(1);
        update();
        break;
    }
    case Qt::Key_7:
    {
        renderer->setRenderingShaders(2);
        update();
        break;
    }
    case Qt::Key_8:
    {
        renderer->setRenderingShaders(3);
        update();
        break;
    }
    case Qt::Key_9:
    {
        renderer->setRenderingShaders(4);
        update();
        break;
    }
    case Qt::Key_0:
        renderer->setRenderingShaders(-1);
        update();
        break;
    }

    // Pass on the event to parent.
    QOpenGLWidget::keyPressEvent(e);

    // The original widget that caused the key press is null sender.
    // However, whichever subsequently linked widget that receives this
    // information will have a non-null sender() so in that case it will stop
    // emitting any further to avoid infinite looping across all linked widgets.
    if (!sender())
        emit keyPressed(e);
}

void Rendering3DWidget::mousePressEvent(QMouseEvent *e)
{
    switch (e->button())
    {
    case (Qt::LeftButton) :
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
			//do nothing
        }
        else if (e->modifiers() == Qt::ShiftModifier)
        {
            this->setCursor(Qt::SizeAllCursor);
            renderer->startNavigation(renderer::NavMode::PAN, e->x(), e->y());
        }
        else
        {
            if (navMode == renderer::NavMode::FLY)
                this->setCursor(Qt::CrossCursor);
            else
                this->setCursor(Qt::ClosedHandCursor);
            renderer->startNavigation(navMode, e->x(), e->y());
        }
        break;
    }
    case (Qt::RightButton) :
        this->setCursor(Qt::SizeAllCursor);
        renderer->startNavigation(renderer::NavMode::PAN, e->x(), e->y());
        break;
    case (Qt::MidButton) :
        this->setCursor(Qt::SizeVerCursor);
        renderer->startNavigation(renderer::NavMode::ZOOM, e->x(), e->y());
        break;
    }
    update();

    mousePressed = true;

    // Pass on the event to parent.
    QOpenGLWidget::mousePressEvent(e);
}

void Rendering3DWidget::keyReleaseEvent(QKeyEvent *e)
{
    this->setCursor(Qt::ArrowCursor);
    QOpenGLWidget::keyReleaseEvent(e);
}

void Rendering3DWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (Qt::LeftButton == e->button())
    {
        bool multiSelection = (
                    (e->modifiers() == Qt::ControlModifier) ||
                    (e->modifiers() == Qt::ShiftModifier));
        select(e->x(), e->y(), multiSelection, e);
    }

    // Pass on the event to parent.
    QOpenGLWidget::mouseDoubleClickEvent(e);
}
void Rendering3DWidget::mouseMoveEvent(QMouseEvent * e)
{
    if (mousePressed)
    {
        //in Navigation mode
        renderer->move(e->x(), e->y());
        update();
    }

    // Pass on the event to parent.
    QOpenGLWidget::mouseMoveEvent(e);
}
void Rendering3DWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (mousePressed)
    {
        renderer->stopNavigation();
        if ( (e->modifiers() != Qt::ControlModifier) &&
             (e->modifiers() != Qt::ShiftModifier))
            this->setCursor(Qt::ArrowCursor);
        mousePressed = false;
        update();
    }

    // Pass on the event to parent.
    QOpenGLWidget::mouseReleaseEvent(e);
}
void Rendering3DWidget::wheelEvent(QWheelEvent * e)
{
    if (!renderer->increaseFlyVelocity(e->delta() < 0 ? 1.0 / 1.3 : 1.3))
    {
        renderer->zoom(e->delta() > 0 ? ZOOM_FACTOR : 1 / ZOOM_FACTOR);
        update();
    }

    // Pass on the event to parent.
    QOpenGLWidget::wheelEvent(e);
}

void Rendering3DWidget::select(
        int x, int y, bool multiSelection, QMouseEvent *)
{
    makeCurrent();
    renderer->selectComponent(context(), x, y, multiSelection);
    update();
}


/*
void Rendering3DWidget::select(
    const QString &name,
    bool multiSelection,
    bool unselectSelected,
    bool update)
{
    QHash<QString, GLC_uint>::const_iterator i = glcMeshesIds.find(name);
    if (i != glcMeshesIds.end())
        select(i.value(), multiSelection, unselectSelected, update);
}
*/
