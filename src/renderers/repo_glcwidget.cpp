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


#include "repo_glcwidget.h"
#include "../primitives/repo_fontawesome.h"
#include "../primitives/repo_color.h"
//------------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------
#include <QFile>
#include <QFont>
#include <QTime>
#include <QTimer>
#include <QScreen>
//------------------------------------------------------------------------------
#include <GLC_UserInput>
#include <GLC_Context>
#include <GLC_Octree>
#include <glc_renderstatistics.h>
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
    clock_gettime( CLOCK_MONOTONIC, &ts);
    unsigned tick = ts.tv_nsec / 1000000;
    tick += ts.tv_sec * 1000;
    return tick;
}
#endif


//------------------------------------------------------------------------------
//
// Static variables
//
//------------------------------------------------------------------------------
const double repo::gui::RepoGLCWidget::ZOOM_FACTOR = 1.2;

//QList<GLC_Shader*> repo::gui::RepoGLCWidget::shaders;

repo::gui::RepoGLCWidget::RepoGLCWidget(QWidget* parent, const QString& windowTitle)
    : QGLWidget(new GLC_Context(QGLFormat::defaultFormat()), parent)
    , glcLight()
    , glcViewport()
    , glcMoverController()
    , isWireframe(false)
    , shaderID(0)
    , isInfoVisible(true)
    , renderingFlag(glc::ShadingFlag)
    , repoScene(0)
{
    //--------------------------------------------------------------------------
    // Default settings
    this->setWindowTitle(windowTitle);
    this->setToolTip(windowTitle);
    this->setWindowIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_cube));
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFocusPolicy(Qt::StrongFocus);

    // To register mouse events on move (by default only on press).
    this->setMouseTracking(true);

    //--------------------------------------------------------------------------
    // Connect slots
    connect(&glcViewport, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));

    //--------------------------------------------------------------------------
    // GLC settings
    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0); // Red colour
    glcMoverController = GLC_Factory::instance()->createDefaultMoverController(
                repColor, &glcViewport);
    connect(&glcMoverController, SIGNAL(repaintNeeded()), this, SLOT(updateGL()));

    glcViewport.setBackgroundColor(Qt::white);
    glcLight.setPosition(1.0, 1.0, 1.0);


    //--------------------------------------------------------------------------
    // Create XYZ axes overlay
    GLC_3DViewInstance line = GLC_Factory::instance()->createLine(GLC_Point3d(), glc::X_AXIS);
    line.geomAt(0)->setWireColor(Qt::red);
    glcUICollection.add(line);
    line = GLC_Factory::instance()->createLine(GLC_Point3d(), glc::Y_AXIS);
    line.geomAt(0)->setWireColor(Qt::darkGreen);
    glcUICollection.add(line);
    line = GLC_Factory::instance()->createLine(GLC_Point3d(), glc::Z_AXIS);
    line.geomAt(0)->setWireColor(Qt::blue);
    glcUICollection.add(line);

    QObject::connect(&glcMoverController, SIGNAL(repaintNeeded()),
                     this, SLOT(broadcastCameraChange()));
}

repo::gui::RepoGLCWidget::~RepoGLCWidget()
{
    glcUICollection.clear();
    glcViewCollection.clear();
    glcWorld.clear();

    GLC_SelectionMaterial::deleteShader(context());

    for (int i = 0; i < shaders.size(); ++i)
        delete shaders[i];
    shaders.clear();

    if (repoScene)
        delete repoScene;
}

//------------------------------------------------------------------------------
//
// OpenGL
//
//------------------------------------------------------------------------------
void repo::gui::RepoGLCWidget::initializeGL()
{
    //--------------------------------------------------------------------------
    // OpenGL initialisation
    glcViewport.cameraHandle()->setDefaultUpVector(glc::Y_AXIS);
    glcViewport.initGl();

    bool isAdvancedGPU =
            GLC_State::vendorIsNvidia() ||
            QString("Intel") == GLC_State::vendor();

    //--------------------------------------------------------------------------
    // For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
    const GLint swapInterval = 1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swapInterval);
    GLC_State::setGlslUsage(true);
#else
    GLC_State::setGlslUsage(isAdvancedGPU);
#endif

    //--------------------------------------------------------------------------
    // Enable VBOs and other settings.
    GLC_State::setVboSupport();
    GLC_State::setVboUsage(isAdvancedGPU);
    GLC_State::setDefaultOctreeDepth(3);
    GLC_State::setPixelCullingUsage(true);
    GLC_State::setFrustumCullingUsage(true);
    GLC_State::setSpacePartionningUsage(true);
    GLC_State::setCacheUsage(true);
    GLC_State::setFrameBufferBlitSupport();
    GLC_State::setPointSpriteSupport();
    GLC_State::setFrameBufferSupport();
    glcViewport.setMinimumPixelCullingSize(3);


    //glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);

    GLint bufs;
    GLint samples;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
    glGetIntegerv(GL_SAMPLES, &samples);

    //setFormat(QGLFormat(QGL::SampleBuffers));
    GLC_RenderStatistics::setActivationFlag(true);

    initializeShaders();

    //--------------------------------------------------------------------------
    // Initialize fps counter
    fpsCounter.initialize();
    GLC_RenderStatistics::setActivationFlag(true);
}

void repo::gui::RepoGLCWidget::initializeShaders()
{

    if (GLC_State::glslUsed()) // && !GLC_State::selectionShaderUsed())
    {
        GLC_State::setSelectionShaderUsage(true);
        QFile vertexShaderFile(":/shaders/select.vert");
        QFile fragmentShaderFile(":/shaders/select.frag");
        GLC_SelectionMaterial::setShaders(
                    vertexShaderFile,
                    fragmentShaderFile,
                    context());
        GLC_SelectionMaterial::initShader(context());
        /*

        if (shaders.isEmpty())
        {
            // Phong Shader
            GLC_Shader* pShader = new GLC_Shader();
            QFile vertexPhongShaderFile(":shaders/phong.vert");
            QFile fragmentPhongShaderFile(":shaders/phong.frag");
            pShader->setVertexAndFragmentShader(vertexPhongShaderFile, fragmentPhongShaderFile);
            pShader->createAndCompileProgrammShader();
            shaders.append(pShader);

            // Toon Shader
            pShader = new GLC_Shader();
            QFile vertexToonShaderFile(":shaders/toon.vert");
            QFile fragmentToonShaderFile(":shaders/toon.frag");
            pShader->setVertexAndFragmentShader(vertexToonShaderFile, fragmentToonShaderFile);
            pShader->createAndCompileProgrammShader();
            shaders.append(pShader);

            // Minnaert Shader
            pShader = new GLC_Shader();
            QFile vertex2ShaderFile(":shaders/minnaert.vert");
            QFile fragment2ShaderFile(":shaders/minnaert.frag");
            pShader->setVertexAndFragmentShader(vertex2ShaderFile, fragment2ShaderFile);
            pShader->createAndCompileProgrammShader();
            shaders.append(pShader);

            // Gooch Shader
            pShader = new GLC_Shader();
            QFile vertex3ShaderFile(":shaders/gooch.vert");
            QFile fragment3ShaderFile(":shaders/gooch.frag");
            pShader->setVertexAndFragmentShader(vertex3ShaderFile, fragment3ShaderFile);
            pShader->createAndCompileProgrammShader();
            shaders.append(pShader);
        }

    */
        //	if (shaders.size() > 2)
        //	{
        //		shaderID = shaders[2]->id();
        //		updateGL();
        //	}
    }

}

void repo::gui::RepoGLCWidget::paintGL()
{
    try
    {
        GLC_RenderStatistics::reset();

        //----------------------------------------------------------------------
        // Calculate camera's depth of view
        glcViewport.setDistMinAndMax(glcWorld.boundingBox());
        glcWorld.collection()->updateInstanceViewableState();

        //----------------------------------------------------------------------
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //----------------------------------------------------------------------
        // Load identity matrix
        GLC_Context::current()->glcLoadIdentity();
        glEnable(GL_MULTISAMPLE);


        //----------------------------------------------------------------------
        // Define the light
        glcLight.glExecute();

        //----------------------------------------------------------------------
        // Define view matrix
        glcViewport.glExecuteCam();

        glcViewport.useClipPlane(true);
        // Apply global shader if set.
        if (shaderID && !GLC_State::isInSelectionMode())
            GLC_Shader::use(shaderID);

        // Display opaque instanced objects
        glcWorld.render(0, renderingFlag);
        if (GLC_State::glslUsed())
            glcWorld.renderShaderGroup(renderingFlag);

        // Display transparent instanced objects
        glcWorld.render(0, glc::TransparentRenderFlag);
        if (GLC_State::glslUsed())
            glcWorld.renderShaderGroup(glc::TransparentRenderFlag);


        // Render the collection which contains bounding boxes
        glcViewCollection.render(0, glc::WireRenderFlag); // To see a box edged
        glcViewCollection.render(0, glc::TransparentRenderFlag); // Render transparent faces

        //----------------------------------------------------------------------
        // Display selected objects
        const int selectedNodesCount = glcWorld.collection()->selectionSize();
        if ((selectedNodesCount > 0) &&
                GLC_State::selectionShaderUsed() &&
                !GLC_State::isInSelectionMode())
        {
            //if (selectedNodesCount != glcWorld.collection()->drawableObjectsSize())
            //{
            //Draw the selection with Zbuffer
            glcWorld.render(1, renderingFlag);
            //}
            glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
            // Draw the selection transparent
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glDepthFunc(GL_ALWAYS);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glcWorld.render(1, renderingFlag);
            glPopAttrib();
        }
        else if (selectedNodesCount > 0)
            glcWorld.render(1, renderingFlag);

        // Remove global shader if set.
        if (shaderID)
            GLC_Shader::unuse();

        glcViewport.useClipPlane(false);

        //----------------------------------------------------------------------
        // Display UI Info (orbit circle)
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glcMoverController.drawActiveMoverRep();

        if (isInfoVisible)
            paintInfo();


        // So that models look nice
        glDisable(GL_CULL_FACE);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    }
    catch (GLC_Exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    //QGLFramebufferObject::bindDefault();
}

void repo::gui::RepoGLCWidget::paintInfo()
{
    static QLocale locale;
    //--------------------------------------------------------------------------
    // FPS calculations
    fpsCounter.increment();

    //--------------------------------------------------------------------------
    // Display info area
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    //--------------------------------------------------------------------------
    QSize screenSize(size());
    int screenHeight = screenSize.height();
    float panelRatio = (float)(screenHeight - 4) / screenHeight;
    double displayRatio = (double)(screenSize.height()) /
            (double)(screenSize.width());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_ONE,GL_SRC_ALPHA);


    //        //--------------------------------------------------------------------------
    //        // Display panel
    //        float panelRat = 1.0f - ((float)42/ screenHeight);
    //        qglColor(QColor(5, 58, 165, 30));
    //        glBegin(GL_QUADS);
    //            glVertex2f(-1.f,1.0f);
    //            glVertex2f( 1.f,1.0f);
    //            glVertex2f( 1.f,panelRat);
    //            glVertex2f(-1.f,panelRat);
    //        glEnd();


    //----------------------------------------------------------------------
    // Display stats
    qglColor(Qt::gray);
    QString selectionName;
    if(glcWorld.selectionSize() > 0)
        selectionName = glcWorld.selectedOccurenceList().first()->name();
    renderText(9, 14, QString() +
               QChar(0x25B2) + " " +
               locale.toString((qulonglong)GLC_RenderStatistics::triangleCount()) +
               tr(" in ") +
               locale.toString((uint)GLC_RenderStatistics::bodyCount()) +
               tr(" objects"));
    renderText(screenSize.width() - 50, 14, fpsCounter.getFPSString());

    //--------------------------------------------------------------------------
    // Display selection
    if(glcWorld.selectionSize() > 0)
        renderText(9, screenHeight - 9, tr("Selected") + ": " + selectionName);


    GLC_Matrix4x4 uiMatrix(glcViewport.cameraHandle()->viewMatrix());
    // Change matrix to follow camera orientation
    double scaleFactor = 0.08;

    // To prevent further scaling when below a certain size.
    if (screenSize.height() < 400)
        scaleFactor *= (double) 400 / (double) screenSize.height();

    glTranslated(1.0 - (scaleFactor * displayRatio * 1.4),
                 - panelRatio + (scaleFactor * 1.3), 0.0);

    glScaled(scaleFactor * displayRatio, scaleFactor, scaleFactor);
    glMultMatrixd(uiMatrix.getData());

    qglColor(Qt::red);
    renderText(1.0, 0.0, 0.0, "x");
    qglColor(Qt::darkGreen);
    renderText(0.0, 1.0, 0.0, "y");
    qglColor(Qt::blue);
    renderText(0.0, 0.0, 1.0, "z");
    glcUICollection.render(0, glc::ShadingFlag);

    //--------------------------------------------------------------------------
    // Restore 3D state
    glPopAttrib();
    glPopMatrix(); // restore model-view matrix

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void repo::gui::RepoGLCWidget::resizeGL(int width, int height)
{
    glcViewport.setWinGLSize(width, height); // Compute window aspect ratio
}

void repo::gui::RepoGLCWidget::reframe(const GLC_BoundingBox& boundingBox)
{
    const GLC_BoundingBox collectionBox= glcWorld.boundingBox();
    if (boundingBox.isEmpty())
    {
        if (!collectionBox.isEmpty())
        {
            GLC_Camera savCam(*(glcViewport.cameraHandle()));
            glcViewport.reframe(collectionBox);
            updateGL();
            emit cameraChangedSignal(*glcViewport.cameraHandle());
        }
    }
    else
    {
        GLC_Camera savCam(*(glcViewport.cameraHandle()));
        glcViewport.reframe(boundingBox);
        if (savCam == *(glcViewport.cameraHandle()))
        {
            glcViewport.reframe(collectionBox);
        }
        updateGL();
        emit cameraChangedSignal(*glcViewport.cameraHandle());
    }
}

void repo::gui::RepoGLCWidget::reframeOnSelection()
{
    GLC_BoundingBox SelectionBox;
    PointerViewInstanceHash* pSelections= glcWorld.collection()->selection();
    PointerViewInstanceHash::iterator iEntry= pSelections->begin();
    while (iEntry != pSelections->constEnd())
    {
        SelectionBox.combine(iEntry.value()->boundingBox());
        ++iEntry;
    }
    reframe(SelectionBox);
}


//------------------------------------------------------------------------------
//
// Public slots
//
//------------------------------------------------------------------------------

void repo::gui::RepoGLCWidget::broadcastCameraChange()
{
    emit cameraChangedSignal(*glcViewport.cameraHandle());
}

void repo::gui::RepoGLCWidget::setCamera(const GLC_Camera& camera)
{
    glcViewport.cameraHandle()->setCam(camera);
    updateGL();
}
void repo::gui::RepoGLCWidget::setCamera(const CameraView& view)
{
    switch (view)
    {
    case FRONT :
        glcViewport.cameraHandle()->setFrontView();
        break;
    case BACK :
        glcViewport.cameraHandle()->setRearView();
        break;
    case LEFT :
        glcViewport.cameraHandle()->setLeftView();
        break;
    case RIGHT :
        glcViewport.cameraHandle()->setRightView();
        break;
    case TOP :
        glcViewport.cameraHandle()->setTopView();
        break;
    case BOTTOM :
        glcViewport.cameraHandle()->setBottomView();
        break;
    case ISO :
    default :
        glcViewport.cameraHandle()->setIsoView();
    }

    if (!glcWorld.isEmpty())
        glcViewport.reframe(glcWorld.boundingBox());

    updateGL();
    emit cameraChangedSignal(*glcViewport.cameraHandle());
}

void repo::gui::RepoGLCWidget::setGLCMeshColors(
        const QString &name,
        const QVector<GLfloat> &colors,
        const bool repaint)
{
    GLC_Mesh* glcMesh = getGLCMesh(name);
    if (glcMesh)
    {
        //	glcMesh->setColorPearVertex(true);
        // TODO: fix me, needs to set color per mesh
        if (repaint)
            updateGL();
        //QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

void repo::gui::RepoGLCWidget::setGLCMeshColors(
        const float r,
        const float g,
        const float b,
        const float a)
{
    QHash<QString, GLC_Mesh*>::iterator it;

    for (it = glcMeshes.begin(); it != glcMeshes.end(); ++it)
    {
        GLC_Mesh *glcMesh = it.value();
        // Always [r,g,b,a] hence times 4
        QVector<GLfloat> vector(glcMesh->VertexCount() * 4);
        for (size_t i = 0; i < vector.size(); i += 4)
        {
            vector[i + 0] = r;
            vector[i + 1] = g;
            vector[i + 2] = b;
            vector[i + 3] = a;
        }
        setGLCMeshColors(glcMesh->name(), vector);
    }
    updateGL();
}

void repo::gui::RepoGLCWidget::setGLCMeshOpacity(const QString &name, qreal opacity)
{
    GLC_Mesh* glcMesh = getGLCMesh(name);
    if (glcMesh)
    {
        QSet<GLC_Material*> materials = glcMesh->materialSet();
        QSet<GLC_Material*>::iterator it;

        for (it = materials.begin(); it != materials.end(); ++it)
        {
            GLC_Material *mat = *it;
            mat->setOpacity(opacity);
        }
    }
}

void repo::gui::RepoGLCWidget::setGLCOccurrenceRenderProperties(
        const QString &occurrenceName,
        const GLC_RenderProperties &properties)
{
    GLC_StructOccurence * oc;

    QHash<QString, GLC_StructOccurence *>::iterator it =
            glcOccurrences.find(occurrenceName);

    QHash<QString, GLC_StructOccurence *>::iterator itt =
            glcMeshOccurences.find(occurrenceName);
    if (glcMeshOccurences.end() != itt)
        oc = itt.value();
    else if (glcOccurrences.end() != it)
        oc = it.value();

    if (oc)
        oc->setRenderProperties(properties);

}

void repo::gui::RepoGLCWidget::setGLCOccurrenceOpacity(
        const QString &occurrenceName, qreal opacity, const QColor &color)
{
    GLC_RenderProperties properties;
    if (opacity < 1.0)
    {
        //	properties.setOverwriteTransparency(opacity);
        // This does not create memory leak as the overwrite material deletes
        // previous one if set and not used.
        properties.setOverwriteMaterial(new GLC_Material(color));
        properties.setRenderingMode(glc::OverwriteMaterial);
    }
    else
        properties.setRenderingMode(glc::NormalRenderMode);
    setGLCOccurrenceRenderProperties(occurrenceName, properties);
}

void repo::gui::RepoGLCWidget::setGLCOccurenceVisibility(const QString &occurrenceName, bool visible)
{
    QHash<QString, GLC_StructOccurence *>::iterator it = glcOccurrences.find(occurrenceName);
    if (glcOccurrences.end() != it)
    {
        GLC_StructOccurence * oc = it.value();
        oc->setVisibility(visible);
    }
}

void repo::gui::RepoGLCWidget::setInfoVisibility(const bool visible)
{
    isInfoVisible = visible;
}

void repo::gui::RepoGLCWidget::setBackgroundColor(
        const QColor &color,
        const bool repaint)
{
    glcViewport.setBackgroundColor(color);
    if (repaint)
        updateGL();
}

void repo::gui::RepoGLCWidget::linkCameras(
        const RepoGLCWidget *widget,
        const bool & on) const
{
    if (on)
    {
        connect(
                    this, SIGNAL(cameraChangedSignal(const GLC_Camera &)),
                    widget, SLOT(setCamera(const GLC_Camera &)));
        // TODO: align all views
    }
    else
    {
        disconnect(
                    this, SIGNAL(cameraChangedSignal(const GLC_Camera &)),
                    widget, SLOT(setCamera(const GLC_Camera &)));
    }
}

//------------------------------------------------------------------------------
//
// Setters
//
//------------------------------------------------------------------------------

void repo::gui::RepoGLCWidget::setRepoScene(core::RepoGraphScene *repoScene)
{
    if (this->repoScene)
        delete this->repoScene;

    this->repoScene = repoScene;
}

void repo::gui::RepoGLCWidget::setGLCWorld(GLC_World glcWorld)
{
    this->glcWorld = glcWorld;
    this->glcWorld.collection()->setLodUsage(true, &glcViewport);
    this->glcWorld.collection()->setVboUsage(true);

    this->glcWorld.collection()->setSpacePartitionningUsage(true);
    GLC_Octree* octree= new GLC_Octree(this->glcWorld.collection());
    octree->updateSpacePartitioning();
    this->glcWorld.collection()->bindSpacePartitioning(octree);
    this->glcWorld.collection()->updateSpacePartitionning();
    this->glcWorld.collection()->updateInstanceViewableState(glcViewport.frustum());

    glcViewport.setDistMinAndMax(this->glcWorld.boundingBox());
    setCamera(ISO);
    extractMeshes(this->glcWorld.rootOccurence());
}

//------------------------------------------------------------------------------
//
// Getters
//
//------------------------------------------------------------------------------

std::vector<std::string> repo::gui::RepoGLCWidget::getSelectionList() const
{
    std::vector<std::string> selectedNames;
    QList<GLC_StructOccurence *> occurrences = glcWorld.selectedOccurenceList();
    QList<GLC_StructOccurence *>::iterator it;
    for (it = occurrences.begin(); it != occurrences.end(); ++it)
    {
        GLC_StructOccurence *oc = *it;
        selectedNames.push_back(oc->name().toStdString());
    }
    return selectedNames;
}

GLC_Mesh* repo::gui::RepoGLCWidget::getGLCMesh(const QString &name) const
{
    GLC_Mesh * glcMesh = 0;
    QHash<QString, GLC_Mesh*>::const_iterator it = glcMeshes.find(name);
    if (glcMeshes.end() != it)
        glcMesh = it.value();
    return glcMesh;
}

QImage repo::gui::RepoGLCWidget::renderQImage(int w, int h)
{
    // See https://bugreports.qt-project.org/browse/QTBUG-33186

    makeCurrent();

    // Set the rendering engine to the size of the image to render
    // Also set the format so that the depth buffer will work
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    QOpenGLFramebufferObject qfb(w,h,format);
    qfb.bind();

    QImage image;
    // If the frame buffer does not work then return an empty image
    if(qfb.isValid())
    {
        // Draw the scene to the buffer
        isInfoVisible = false;
        resizeGL(w,h); // resize scene
        updateGL(); // draw to the buffer
        qfb.release();
        image = qfb.toImage();

        isInfoVisible = true;
        resizeGL(width(),height());
        updateGL();
    }
    return(image);
}


//------------------------------------------------------------------------------
// Create GLC_Object to display
void repo::gui::RepoGLCWidget::addBoundingBox(
        const double lx,
        const double ly,
        const double lz,
        const std::vector<double>& transformationMatrix)
{
    GLC_Box* pBox= new GLC_Box(lx, ly, lz);
    GLC_3DViewInstance box(pBox);

    // Rotate and translate the box by the transformation matrix
    box.multMatrix(GLC_Matrix4x4(&transformationMatrix[0]));

    // Create GLC_Box from a Bounding box
    //GLC_3DViewInstance box(GLC_Factory::instance()->createBox(glcWorld.boundingBox()));

    // Replace GLC_Box default material
    GLC_Material * pRedMaterial= new GLC_Material(Qt::gray);
    pRedMaterial->setOpacity(0.1);
    box.geomAt(0)->replaceMasterMaterial(pRedMaterial);

    // Add the box to the collection
    glcViewCollection.add(box);
}

void repo::gui::RepoGLCWidget::clearBoundingBoxes()
{
    glcViewCollection.clear();
}

void repo::gui::RepoGLCWidget::toggleOctree()
{
    if (glcViewCollection.isEmpty())
    {
        GLC_Material* mat = new GLC_Material(Qt::red);
        mat->setOpacity(0.1);
        GLC_SpacePartitioning* spacePartitioning = glcWorld.collection()->spacePartitioningHandle();
        if (spacePartitioning)
        {
            GLC_Octree* octree= dynamic_cast<GLC_Octree*>(spacePartitioning);
            if (octree)
            {
                octree->createBox(mat, &glcViewCollection);

            }
        }
    }
    else
        glcViewCollection.clear();
    updateGL();
}

//------------------------------------------------------------------------------
//
// User interaction
//
//------------------------------------------------------------------------------
void repo::gui::RepoGLCWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_0 :
        shaderID = 0;
        updateGL();
        break;
    case Qt::Key_1 :
        if (shaders.size() > 0)
        {
            shaderID = shaders[0]->id();
            updateGL();
        }
        break;
    case Qt::Key_2 :
        if (shaders.size() > 1)
        {
            shaderID = shaders[1]->id();
            updateGL();
        }
        break;
    case Qt::Key_3 :
        if (shaders.size() > 2)
        {
            shaderID = shaders[2]->id();
            updateGL();
        }
        break;
    case Qt::Key_Minus:
    case Qt::Key_Underscore:
        glcViewport.cameraHandle()->zoom(1/ZOOM_FACTOR);
        updateGL();
        emit cameraChangedSignal(*glcViewport.cameraHandle());
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        glcViewport.cameraHandle()->zoom(ZOOM_FACTOR);
        emit cameraChangedSignal(*glcViewport.cameraHandle());
        updateGL();
        break;
    case Qt::Key_A :
        if ((e->modifiers() == Qt::ControlModifier))
        {
            if (glcWorld.collection()->selectionSize() ==
                    glcWorld.collection()->drawableObjectsSize())
                glcWorld.unselectAll();
            else
                glcWorld.selectAllWith3DViewInstanceInCurrentShowState();
            updateGL();
        }
    case Qt::Key_T :
        if (glIsEnabled(GL_TEXTURE_2D))
            glDisable(GL_TEXTURE_2D);
        else
            glEnable(GL_TEXTURE_2D);
        updateGL();
        break;
    case Qt::Key_C :
        if (glIsEnabled(GL_CULL_FACE))
        {
            glDisable(GL_CULL_FACE);
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        }
        else
        {
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
            glEnable(GL_CULL_FACE);
            //glFrontFace(false ? GL_CCW : GL_CW);
        }
        updateGL();
        break;
    case Qt::Key_L :
        if (glIsEnabled(GL_LIGHTING))
            glDisable(GL_LIGHTING);
        else
            glEnable(GL_LIGHTING);
        updateGL();
        break;
    case Qt::Key_W :
    {
        isWireframe = !isWireframe;
        setMode(isWireframe ? GL_LINE : GL_FILL);
        updateGL();
        break;
    }
    case Qt::Key_P :
        glcViewport.setToOrtho(!glcViewport.useOrtho());
        updateGL();
        break;
    case Qt::Key_R :
    {
        QHash<QString, GLC_Mesh*>::iterator it;
        for (it = glcMeshes.begin(); it != glcMeshes.end(); ++it)
        {
            GLC_Mesh *glcMesh = it.value();
            float r = (float) rand() / (RAND_MAX);
            float g = (float) rand() / (RAND_MAX);
            float b = (float) rand() / (RAND_MAX);
            // Always [r,g,b,a] hence times 4
            QVector<GLfloat> vector(glcMesh->VertexCount() * 4);
            for (unsigned int i = 0; i < vector.size(); i += 4)
            {
                vector[i + 0] = r;
                vector[i + 1] = g;
                vector[i + 2] = b;
                vector[i + 3] = 0.9f;
            }
            setGLCMeshColors(glcMesh->name(), vector);
        }
        updateGL();
        break;
    }
    case Qt::Key_Q :
    {
        QColor color = QColorDialog::getColor(Qt::white,
                                              this,
                                              "Color picker");
        if (color.isValid())
        {
            glcViewport.setBackgroundColor(color);
            updateGL();
        }
        break;
    }
    case  Qt::Key_O :
    {
        toggleOctree();
    }
        break;
    case Qt::Key_F1 : // Points
    {
        setMode(GL_POINT);
        setRenderingFlag(glc::ShadingFlag);
        updateGL();
        break;
    }
    case Qt::Key_F2 : // Triangle wireframe
    {
        setMode(GL_LINE);
        setRenderingFlag(glc::ShadingFlag);
        updateGL();
        break;
    }
    case Qt::Key_F3 : // Shading with polygon wireframe
    {
        setMode(GL_FILL);
        setRenderingFlag(glc::WireRenderFlag);
        updateGL();
        break;
    }
    case Qt::Key_F4 : // Shading
    {
        setMode(GL_FILL);
        setRenderingFlag(glc::ShadingFlag);
        updateGL();
        break;
    }
    }
    // Pass on the event to parent.
    QGLWidget::keyPressEvent(e);
}
void repo::gui::RepoGLCWidget::mousePressEvent(QMouseEvent *e)
{
    //if (glcMoverController.hasActiveMover()) return;
    switch (e->button())
    {
    case (Qt::RightButton):
        this->setCursor(Qt::ClosedHandCursor);
        glcMoverController.setActiveMover(
                    GLC_MoverController::TrackBall,
                    GLC_UserInput(e->x(), e->y()));
        updateGL();
        break;
    case (Qt::LeftButton):
        this->setCursor(Qt::SizeAllCursor);
        glcMoverController.setActiveMover(
                    GLC_MoverController::Pan,
                    GLC_UserInput(e->x(), e->y()));
        updateGL();
        break;
    case (Qt::MidButton):
        this->setCursor(Qt::CrossCursor);
        glcMoverController.setActiveMover(
                    GLC_MoverController::Fly,
                    GLC_UserInput(e->x(), e->y()));
        updateGL();
        break;
    }
    // Pass on the event to parent.
    QGLWidget::mousePressEvent(e);
}
void repo::gui::RepoGLCWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (Qt::LeftButton == e->button())
    {
        bool multiSelection = (
                    (e->modifiers() == Qt::ControlModifier) ||
                    (e->modifiers() == Qt::ShiftModifier));
        select(e->x(),e->y(), multiSelection, e);
    }

    // Pass on the event to parent.
    QGLWidget::mouseDoubleClickEvent(e);
}
void repo::gui::RepoGLCWidget::mouseMoveEvent(QMouseEvent * e)
{
    if (glcMoverController.hasActiveMover() &&
            glcMoverController.move(GLC_UserInput(e->x(), e->y())))
    {
        updateGL();
        emit cameraChangedSignal(*glcViewport.cameraHandle());
    }
    else
    {
        //	select(e->x(),e->y(), false, e);
        //	updateGL();
    }

    // Pass on the event to parent.
    QGLWidget::mouseMoveEvent(e);
}
void repo::gui::RepoGLCWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (glcMoverController.hasActiveMover())
    {
        this->setCursor(Qt::ArrowCursor);
        glcMoverController.setNoMover();
        updateGL();
    }
    // Pass on the event to parent.
    QGLWidget::mouseReleaseEvent(e);
}
void repo::gui::RepoGLCWidget::wheelEvent(QWheelEvent * e)
{
    GLC_FlyMover* flyMover = dynamic_cast<GLC_FlyMover*>(glcMoverController.activeMover());
    if (flyMover)
    {
        flyMover->increaseVelocity(e->delta() < 0 ? 1.0 / 1.3 : 1.3);
    }
    else
    {
        glcViewport.cameraHandle()->zoom(e->delta() > 0 ? ZOOM_FACTOR : 1/ZOOM_FACTOR);
        updateGL();
        emit cameraChangedSignal(*glcViewport.cameraHandle());
    }

    // Pass on the event to parent.
    QGLWidget::wheelEvent(e);
}

void repo::gui::RepoGLCWidget::select(int x, int y, bool multiSelection,
                                      QMouseEvent *pMouseEvent)
{
    setAutoBufferSwap(false);
    //	glcWorld.collection()->setLodUsage(true, &glcViewport);
    GLC_uint selectionID = glcViewport.renderAndSelect(x, y);
    GLC_uint pickupID = glcViewport.selectOnPreviousRender(x, y);
    //	glcWorld.collection()->setLodUsage(false, &glcViewport);
    select(selectionID, multiSelection);
}

void repo::gui::RepoGLCWidget::select(GLC_uint selectionID,
                                      bool multiSelection,
                                      bool unselectSelected,
                                      bool repaint)
{
    setAutoBufferSwap(true);
    if (glcWorld.containsOccurence(selectionID))
    {
        if (unselectSelected
                && (!glcWorld.isSelected(selectionID))
                && (glcWorld.selectionSize() > 0)
                && (!multiSelection))
        {
            glcWorld.unselectAll();
            emit selectionChanged(this, getSelectionList());
            //emit unselectAll();
        }
        if (!glcWorld.isSelected(selectionID))
        {
            glcWorld.select(selectionID);
            emit selectionChanged(this, getSelectionList());
            //emit updateSelection(m_World.collection()->selection());
        }
        else if (unselectSelected && glcWorld.isSelected(selectionID) && multiSelection)
        {
            glcWorld.unselect(selectionID);
            emit selectionChanged(this, getSelectionList());
            //emit updateSelection(m_World.collection()->selection());
        }
        else
        {
            if (unselectSelected)
                glcWorld.unselectAll();
            glcWorld.select(selectionID);

            emit selectionChanged(this, getSelectionList());
            //emit updateSelection(m_World.collection()->selection());
        }
    }
    else if (unselectSelected && (glcWorld.selectionSize() > 0) && (!multiSelection))
    {
        // if a geometry is selected, unselect it
        glcWorld.unselectAll();
        emit selectionChanged(this, getSelectionList());
        //emit unselectAll();
    }

    if (repaint)
        updateGL();
}

void repo::gui::RepoGLCWidget::select(
        const QString &name,
        bool multiSelection,
        bool unselectSelected,
        bool repaint)
{
    QHash<QString, GLC_uint>::const_iterator i = glcMeshesIds.find(name);
    if (i != glcMeshesIds.end())
        select(i.value(), multiSelection, unselectSelected, repaint);
}

//------------------------------------------------------------------------------
//
// Scene Management
//
//------------------------------------------------------------------------------

void repo::gui::RepoGLCWidget::extractMeshes(GLC_StructOccurence * occurrence)
{
    if (occurrence)
    {
        // Store the occurrence in a hash map.
        QString occurrenceName = occurrence->name();
        glcOccurrences.insert(occurrenceName, occurrence);

        if (occurrence->structInstance() &&
                occurrence->structInstance()->structReference())
        {
            GLC_StructReference * glcReference = occurrence->structInstance()->structReference();
            if (!glcReference->representationIsEmpty())
            {
                GLC_3DRep * pRep = dynamic_cast<GLC_3DRep*>
                        (glcReference->representationHandle());
                if (pRep)
                {
                    for (int i = 0; i < pRep->numberOfBody(); ++i)
                    {
                        GLC_Mesh * glcMesh = dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));

                        if (glcMesh)
                        {
                            glcMesh->setColorPearVertex(true);
                            glcMeshes.insert(glcMesh->name(), glcMesh);
                            glcMeshesIds.insert(glcMesh->name(), occurrence->id());
                            glcMeshOccurences.insert(glcMesh->name(), occurrence);
                        }
                    }
                }
            }
        }

        //----------------------------------------------------------------------
        // Children
        QList<GLC_StructOccurence*> children = occurrence->children();
        QList<GLC_StructOccurence*>::iterator it;
        for (it = children.begin(); it != children.end(); ++it)
        {
            GLC_StructOccurence *child = *it;
            extractMeshes(child);
        }
    }
}

