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


#include "repo_renderer_glc.h"
#include "../../workers/repo_worker_glc_export.h"
#include <repo/core/model/bson/repo_bson_factory.h>

//------------------------------------------------------------------------------
#include <GLC_UserInput>
#include <GLC_Context>
#include <GLC_Octree>
#include <GLC_State>
#include <glc_renderstatistics.h>
//------------------------------------------------------------------------------

using namespace repo::gui::renderer;

GLCRenderer::GLCRenderer()
    : AbstractRenderer()
    , glcLight()
    , glcViewport()
    , glcMoverController()
    , glc3DWidgetManager(&glcViewport)
    , renderingFlag(glc::ShadingFlag)
    , clippingPlaneWidgets({nullptr, nullptr, nullptr})
    , clippingPlaneReverse(false)
    , shaderID(0)
    , isWireframe(false)
    , currentlyHighLighted("")
{
    //--------------------------------------------------------------------------
    // GLC settings
    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0); // Red colour
    glcMoverController = GLC_Factory::instance()->createDefaultMoverController(
                repColor, &glcViewport);

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

    //--------------------------------------------------------------------------


    QObject::connect(
                &glcMoverController, &GLC_MoverController::repaintNeeded,
                this, &AbstractRenderer::repaintNeeded);

    QObject::connect(
                &glcMoverController, &GLC_MoverController::repaintNeeded,
                this, &AbstractRenderer::notifyCameraChange);

    QObject::connect(&glcViewport, &GLC_Viewport::updateOpenGL,
                     this, &AbstractRenderer::repaintNeeded);
}

GLCRenderer::~GLCRenderer()
{
    glcWorld.clear();
}


std::vector<QString> GLCRenderer::applyFalseColoringMaterials()
{
    std::vector<QString> ids;
    if(matMap.size() > (pow(2, 24) -1))
    {
        //We represent indices using rgb values, which is 3*8 bit.
        //0 can't be used as it will be the same as the background,
        // so we can represent 2^24-1 components
        repoError << "This model has too many components to support selection!";
        return ids;
    }

    ids.push_back(""); //white is never used as the background will be white.
    for(auto &matPair : matMap)
    {
        QVector<GLubyte> colorId(4);
        glc::encodeRgbId(ids.size(),&colorId[0]);
        ids.push_back(matPair.first);
        QColor color((int)colorId[0], (int)colorId[1], (int)colorId[2], (int)colorId[3]);
        setMeshColor(matPair.first, 1.0, color);
    }

    return ids;


}

CameraSettings GLCRenderer::convertToCameraSettings(GLC_Camera *cam)
{
    CameraSettings res;
    auto eye = cam->eye();
    auto target = cam->target();
    auto up = cam->upVector();

    res.eye.x = eye.x();
    res.eye.y = eye.y();
    res.eye.z = eye.z();

    res.target.x = target.x();
    res.target.y = target.y();
    res.target.z = target.z();

    res.up.x = up.x();
    res.up.y = up.y();
    res.up.z = up.z();

    return res;
}

void GLCRenderer::deleteShaders(QOpenGLContext *context)
{
    GLC_SelectionMaterial::deleteShader(context);
    for (int i = 0; i < shaders.size(); ++i)
        delete shaders[i];
    shaders.clear();
}



void GLCRenderer::disableSelectionMode()
{
    if(GLC_State::isInSelectionMode())
    {
        GLC_State::setSelectionMode(false);
        GLC_State::setUseCustomFalseColor(false);
        resetColors();
    }
    else
    {
        repoError << "Trying to disable selectionMode when it is not in selection mode!";
    }

}


std::vector<QString> GLCRenderer::enableSelectionMode(const bool useCurrentMaterials)
{
    std::vector<QString> idMapping;
    if(GLC_State::isInSelectionMode())
    {
        repoError << "Trying to enable selectionMode when it is in selection mode!";
    }
    else if(matMap.size() > (pow(2, 24) -1))
    {
        //We represent indices using rgb values, which is 3*8 bit.
        //0 can't be used as it will be the same as the background,
        // so we can represent 2^24-1 components
        repoError << "This model has too many components to support selection!";

    }
    else
    {

        if (glcWorld.collection()->spacePartitioningIsUsed())
        {
                // TODO: this causes GLC_Light exception on Release but not Debug.
    //        GLC_Frustum selectionFrustum(glcViewport.selectionFrustum(x, y));
    //        glcWorld.collection()->updateInstanceViewableState(selectionFrustum);
            glcWorld.collection()->setSpacePartitionningUsage(false);
            glcWorld.collection()->updateInstanceViewableState(glcViewport.frustum());
            glcWorld.collection()->setSpacePartitionningUsage(true);
        }

        GLC_State::setSelectionMode(true);
        GLC_State::setUseCustomFalseColor(true);
        if(!useCurrentMaterials)
        {
            idMapping = applyFalseColoringMaterials();
        }


        glEnable(GL_DEPTH_TEST);

    }

    return idMapping;
}


void GLCRenderer::extractMeshes(GLC_StructOccurrence * occurrence)
{
    if (occurrence)
    {
        // Store the occurrence in a hash map.
        QString occurrenceName = occurrence->name();

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
                            meshMap[glcMesh->name()] = glcMesh;
                            QList<GLuint> materialIds = glcMesh->materialIds();
                            for (const GLuint id : materialIds)
                            {
                                GLC_Material *mat = glcMesh->material(id);
                                if (mat)
                                {
                                    matMap[mat->name()] = mat;
                                }
                            }
                        }
                    }
                }
            }
        }

        //----------------------------------------------------------------------
        // Children
        QList<GLC_StructOccurrence*> children = occurrence->children();
        QList<GLC_StructOccurrence*>::iterator it;
        for (it = children.begin(); it != children.end(); ++it)
        {
            GLC_StructOccurrence *child = *it;
            extractMeshes(child);
        }
    }
}

CameraSettings GLCRenderer::getCurrentCamera()
{
    return convertToCameraSettings(glcViewport.cameraHandle());
}

QImage GLCRenderer::getCurrentImageWithNoShading(
        const bool disableTexture,
        const bool useFalseColoring)
{
    resetColors();

    if(!useFalseColoring && disableTexture)
    {
        //loop through all the materials and remove it's texture

        for(auto &mat : matMap)
        {
            if(mat.second->hasTexture())
            {
                changedMats[mat.second] = GLC_Material(*mat.second);
                mat.second->removeTexture();
            }
        }

    }


    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    QOpenGLFramebufferObject fbo(glcViewport.size().width(), glcViewport.size().height(), format);

    fbo.bind();

    enableSelectionMode(!useFalseColoring);

    render(nullptr);

    auto image = fbo.toImage();


    disableSelectionMode();
    fbo.release();
    fbo.bindDefault();

    return image;
}

QImage GLCRenderer::getCurrentImageWithNoShading(
        const bool disableTexture)
{


    return getCurrentImageWithNoShading(disableTexture, false);

}

QImage GLCRenderer::getCurrentImageWithFalseColoring()
{

    return getCurrentImageWithNoShading(false, true);

}

void GLCRenderer::highlightMesh(
        const QString &meshId)
{
    if(currentlyHighLighted == meshId)
    {
        //currently highlighted, should unhighlight it
        revertMeshMaterial(meshId);
        currentlyHighLighted = "";
    }
    else
    {
        GLC_Material highlightMat;


        highlightMat.setAmbientColor(QColor::fromRgbF(1.0f, 0.5f, 0.0f, 0.2f));
        highlightMat.setDiffuseColor(QColor::fromRgbF(1.0f, 0.5f, 0.0f, 0.2f));
        highlightMat.setSpecularColor(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
        highlightMat.setEmissiveColor(QColor::fromRgbF(1.0f, 0.5f, 0.0f, 0.2f));
        highlightMat.setOpacity(1.0);

        changeMeshMaterial(meshId, highlightMat);
        currentlyHighLighted = meshId;
    }



}

void GLCRenderer::changeMeshMaterial(
        const QString &uuidString,
        const GLC_Material &newMat)
{
    auto meshIt = meshMap.find(uuidString);
    auto matIt = matMap.find(uuidString);
    GLC_Material *mat = nullptr;
    if (matIt != matMap.end())
    {
        mat = matIt->second;

    }
    else if(meshIt != meshMap.end())
    {
        GLC_Mesh *mesh = meshIt->second;
        if (mesh->materialCount())
        {
            //has material, alter the emissive color
            auto matIds = mesh->materialIds();
            mat = mesh->material(matIds[0]);
        }
        else
        {
            //The mesh should have at least the default material due to how GLC_Mesh is constructed
            repoLogError("mesh " + uuidString.toStdString() + " has no material. This is unexpected!");
        }

    }
    else
    {
        repoLogError("Failed to set color of mesh " + uuidString.toStdString() + " : mesh not found!");
    }

    if (mat)
    {
        if (changedMats.find(mat) == changedMats.end())
        {
            //preserve original material
            changedMats[mat] = GLC_Material(*mat);
        }

        *mat=newMat;

    }
}


bool GLCRenderer::increaseFlyVelocity(const float &vel)
{
    bool success;

    GLC_FlyMover* flyMover = dynamic_cast<GLC_FlyMover*>(glcMoverController.activeMover());
    if (success = flyMover)
    {
        flyMover->increaseVelocity(vel);
    }
    return success;
}

void GLCRenderer::initialize()
{
    initializeOpenGLFunctions();
    GLC_State::init();

    glcViewport.cameraHandle()->setDefaultUpVector(glc::Y_AXIS);
    glcViewport.initGl();

    bool isAdvancedGPU = QString("NVIDIA") == GLC_State::vendor() ||
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

    glEnable(GL_NORMALIZE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);

    GLint bufs;
    GLint samples;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
    glGetIntegerv(GL_SAMPLES, &samples);

    //--------------------------------------------------------------------------
    // Initialize fps counter
    fpsCounter.initialize();

}

void GLCRenderer::loadModel(
        repo::core::model::RepoScene *scene,
        const std::vector<double> &offsetVector)
{
    //We have a scene, fire up the GLC worker to get a GLC World representation
    //--------------------------------------------------------------------------
    // Establish and connect the new worker.
    repo::worker::GLCExportWorker* worker =
            new repo::worker::GLCExportWorker(scene, offsetVector);
    connect(worker, &repo::worker::GLCExportWorker::finished,
            this, &GLCRenderer::setGLCWorld);
    connect(worker, &repo::worker::GLCExportWorker::progress, this, &GLCRenderer::workerProgress);

    QObject::connect(
                this, &AbstractRenderer::killWorker,
                worker, &repo::worker::GLCExportWorker::cancel, Qt::DirectConnection);

    //--------------------------------------------------------------------------
    // Fire up the asynchronous calculation.
    QThreadPool::globalInstance()->start(worker);

	if (offsetVector.size())
	{
		auto sceneOffset = scene->getWorldOffset();
		std::vector<double> dOffset = { sceneOffset[0] - offsetVector[0],
			sceneOffset[1] - offsetVector[1], sceneOffset[2] - offsetVector[2] };

		offset = dOffset;
	}
    
}

bool GLCRenderer::move(const int &x, const int &y)
{
    bool done = glcMoverController.hasActiveMover() &&
            glcMoverController.move(GLC_UserInput(x, y));
    emit cameraChanged(getCurrentCamera());
    return done;
}

void GLCRenderer::setRenderingMode(const RenderMode &mode)
{
    switch (mode)
    {
    case RenderMode::POINT:
        glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, GL_POINT);
        renderingFlag = glc::ShadingFlag;
        break;
    case RenderMode::WIREFRAME:
        glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, GL_LINE);
        renderingFlag = glc::ShadingFlag;
        isWireframe = true;
        break;
    case RenderMode::WIREFRAME_SHADING:
        glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, GL_FILL);
        renderingFlag = glc::WireRenderFlag;
        isWireframe = true;
        break;
    case RenderMode::SHADING:
        glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, GL_FILL);
        renderingFlag = glc::ShadingFlag;
        break;
    default:
        repoLogError("Unsupported rendering mode: " + std::to_string((int)mode));

    }
}

void GLCRenderer::setMeshColor(
        const repoUUID &uniqueID,
        const qreal &opacity,
        const QColor &color)
{

    QString uuidString = QString::fromStdString(UUIDtoString(uniqueID));

    setMeshColor(uuidString, opacity, color);
}

void GLCRenderer::setMeshColor(
        const QString &uuidString,
        const qreal &opacity,
        const QColor &color)
{
    GLC_Material coloredMat;
    coloredMat.setAmbientColor(color);
    coloredMat.setDiffuseColor(color);
    coloredMat.setSpecularColor(QColor(1.0, 1.0, 1.0, 1.0));
    coloredMat.setEmissiveColor(QColor(0.0, 0.0, 0.0, 1.0));
    coloredMat.setShininess(50);
    coloredMat.setOpacity(opacity);

    changeMeshMaterial(uuidString, coloredMat);
}

void GLCRenderer::startNavigation(const NavMode &mode, const int &x, const int &y)
{
    switch (mode)
    {
    case NavMode::TURNTABLE:
        glcMoverController.setActiveMover(
                    GLC_MoverController::TurnTable,
                    GLC_UserInput(x, y));
        break;

    case NavMode::ORBIT:
        glcMoverController.setActiveMover(
                    GLC_MoverController::TrackBall,
                    GLC_UserInput(x, y));
        break;

    case NavMode::TSR:
        glcMoverController.setActiveMover(
                    GLC_MoverController::TSR,
                    GLC_UserInput(x, y));
        break;

    case NavMode::TARGET:
        glcMoverController.setActiveMover(
                    GLC_MoverController::Target,
                    GLC_UserInput(x, y));
        break;

    case NavMode::PAN:
        glcMoverController.setActiveMover(
                    GLC_MoverController::Pan,
                    GLC_UserInput(x, y));
        break;

    case NavMode::FLY:
        glcMoverController.setActiveMover(
                    GLC_MoverController::Fly,
                    GLC_UserInput(x, y));
        break;

    case NavMode::ZOOM :
        glcMoverController.setActiveMover(
                    GLC_MoverController::Zoom,
                    GLC_UserInput(x, y));
        break;

    default:
        repoLogError("Unrecognised mode: " + std::to_string((int)mode));
    }
}

void GLCRenderer::stopNavigation()
{
    if (glcMoverController.hasActiveMover())
    {
        glcMoverController.setNoMover();
    }
}

void GLCRenderer::setGLCWorld(GLC_World                        &world,
                              std::map<QString, GLC_Mesh*>     &_meshMap,
                              std::map<QString, GLC_Material*> &_matMap)
{
    repoLog("Setting GLC World...");
    repoLog("\tGLC World empty: " + std::to_string(world.isEmpty()));
    repoLog("\tGLC World size: " + std::to_string(world.size()));
    repoLog("\tGLC World #vertex: " + std::to_string(world.numberOfVertex()));

    meshMap    = _meshMap;
    matMap     = _matMap;

    this->glcWorld = world;
    this->glcWorld.collection()->setLodUsage(true, &glcViewport);
    this->glcWorld.collection()->setVboUsage(true);

    this->glcWorld.collection()->setSpacePartitionningUsage(true);
    GLC_Octree* octree = new GLC_Octree(this->glcWorld.collection());
    octree->updateSpacePartitioning();
    this->glcWorld.collection()->bindSpacePartitioning(octree);
    this->glcWorld.collection()->updateSpacePartitionning();
    this->glcWorld.collection()->updateInstanceViewableState(glcViewport.frustum());

    GLC_BoundingBox bbox = this->glcWorld.boundingBox();
    glcViewport.setDistMinAndMax(bbox);
    setCamera(CameraView::ISO);
    //extractMeshes(this->glcWorld.rootOccurrence());


    //glcLight.setPosition(bbox.upperCorner().x(), bbox.upperCorner().y(), bbox.upperCorner().z());
}

void GLCRenderer::paintInfo(QPainter *painter,
                            const int &screenHeight,
                            const int &screenWidth)
{
    fpsCounter.increment();
    if (painter)
    {
        static QLocale locale;

        float panelRatio = (float)(screenHeight - 4) / screenHeight;
        double displayRatio = (double)(screenHeight) /
                (double)(screenWidth);

        // Change matrix to follow camera orientation
        double scaleFactor = 0.08;

        // To prevent further scaling when below a certain size.
        if (screenHeight < 400)
            scaleFactor *= (double)400 / (double)screenHeight;

        //--------------------------------------------------------------------------
        // Display info area
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);

        //--------------------------------------------------------------------------

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBlendFunc(GL_ONE, GL_SRC_ALPHA);


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

        GLC_Matrix4x4 uiMatrix(glcViewport.cameraHandle()->viewMatrix());


        glTranslated(1.0 - (scaleFactor * displayRatio * 1.4),
                     -panelRatio + (scaleFactor * 1.3), 0.0);

        glScaled(scaleFactor * displayRatio, scaleFactor, scaleFactor);
        glMultMatrixd(uiMatrix.getData());

        //            painter->setPen(Qt::red);
        //            painter->drawText(QPointF(1.0f, 0.0f, 0.0f), "x");
        //            painter->setPen(Qt::darkGreen);
        //            painter->drawText(QPointF(0.0f, 1.0f, 0.0f), "y");
        //            painter->setPen(Qt::blue);
        //            painter->drawText(QPointF(0.0f, 0.0f, 1.0f), "z");

        glcUICollection.render(0, glc::ShadingFlag);

        //----------------------------------------------------------------------

        // Restore 3D state
        glPopAttrib();
        glPopMatrix(); // restore model-view matrix

        //--------------------------------------------------------------------------

        // Display stats
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

        painter->setPen(Qt::gray);
        painter->drawText(9, 14, QString() +
                          tr("Tris") + ": " + locale.toString((qulonglong)GLC_RenderStatistics::triangleCount()));
        painter->drawText(9, 30, QString() +
                          tr("Objs") + ": " + locale.toString((uint)GLC_RenderStatistics::bodyCount()));

        painter->drawText(screenWidth - 60, 14, fpsCounter.getFPSString());

        //----------------------------------------------------------------------
        // Display selection
        if (!currentlyHighLighted.isEmpty())
            painter->drawText(9, screenHeight - 9, tr("Selected") + ": " + currentlyHighLighted);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
}

void GLCRenderer::render(QPainter *painter,
                         const int &screenHeight,
                         const int &screenWidth)
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
        QColor bc = glcViewport.backgroundColor();
        glClearColor(bc.redF(), bc.greenF(), bc.blueF(), 1.0f);
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

        if (!GLC_State::isInSelectionMode())
            glc3DWidgetManager.render();

        //----------------------------------------------------------------------
        // Display UI Info (orbit circle)
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        glcMoverController.drawActiveMoverRep();

        glDisable(GL_DEPTH_TEST);
        GLC_ContextManager::instance()->currentContext()->glcMatrixMode(GL_MODELVIEW);
        paintInfo(painter, screenHeight, screenWidth);

        // So that models look nice
        glDisable(GL_CULL_FACE);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    }
    catch (GLC_Exception &e)
    {
        repoLogError(e.what());
    }
}

void GLCRenderer::resetColors()
{
    for (const auto &pair : changedMats)
    {
        *pair.first = pair.second;
    }
    changedMats.clear();
}

void GLCRenderer::resetView()
{
    const GLC_BoundingBox bbox = glcWorld.boundingBox();
    if (!bbox.isEmpty())
    {
        glcViewport.reframe(bbox);
        emit cameraChanged(getCurrentCamera());
    }
}

void GLCRenderer::resizeWindow(const int &width, const int &height)
{
    glcViewport.setWinGLSize(width, height); // Compute window aspect ratio
}

void GLCRenderer::revertMeshMaterial(
        const QString &uuidString)
{
    auto meshIt = meshMap.find(uuidString);
    auto matIt = matMap.find(uuidString);
    GLC_Material *mat = nullptr;
    if (matIt != matMap.end())
    {
        mat = matIt->second;

    }
    else if(meshIt != meshMap.end())
    {
        GLC_Mesh *mesh = meshIt->second;
        if (mesh->materialCount())
        {
            auto matIds = mesh->materialIds();
            mat = mesh->material(matIds[0]);
        }
        else
        {
            //The mesh should have at least the default material due to how GLC_Mesh is constructed
            repoLogError("mesh " + uuidString.toStdString() + " has no material. This is unexpected!");
        }

    }
    else
    {
        repoLogError("Failed to revert color of mesh " + uuidString.toStdString() + " : mesh not found!");
    }

    if (mat)
    {
        auto changedIt = changedMats.find(mat);
        if ( changedIt != changedMats.end())
        {
            *mat = changedIt->second;
            changedMats.erase(changedIt);
        }

    }

}


void GLCRenderer::selectComponent(QOpenGLContext *context, int x, int y, bool multiSelection)
{
    if(multiSelection)
        repoLogError("Multi-selection currently does not work");
    //FIXME: multi-selection doesn't work at the moment
    if(matMap.size() > (pow(2, 24) -1))
    {
        //We represent indices using rgb values, which is 3*8 bit.
        //0 can't be used as it will be the same as the background,
        // so we can represent 2^24-1 components
        repoError << "This model has too many components to support selection!";
        return;
    }

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    QOpenGLFramebufferObject fbo(glcViewport.size().width(), glcViewport.size().height(), format);

    fbo.bind();
    QColor backgroundColor = glcViewport.backgroundColor();
    glcViewport.setBackgroundColor(QColor(Qt::white));

    auto ids = enableSelectionMode(false);

    render(nullptr);


    QVector<GLubyte> colorId(4); // 4 -> R G B A
    context->functions()->glReadPixels(x, glcViewport.size().height() - y, 1, 1,
                                       GL_RGBA, GL_UNSIGNED_BYTE, colorId.data());

    disableSelectionMode();
    GLC_uint returnId = glc::decodeRgbId(&colorId[0]);

    if(returnId < ids.size())
    {
        highlightMesh(ids[(int)returnId]);
    }
    fbo.release();
    fbo.bindDefault();


    glcViewport.setBackgroundColor(backgroundColor);

    context->doneCurrent();
}

void GLCRenderer::setActivationFlag(const bool &flag)
{
    GLC_RenderStatistics::setActivationFlag(true);
}

void GLCRenderer::setAndInitSelectionShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context)
{
    if (GLC_State::glslUsed()) // && !GLC_State::selectionShaderUsed())
    {
        GLC_State::setSelectionShaderUsage(true);
        GLC_SelectionMaterial::setShaders(vertexFile,fragmentFile,context);
        GLC_SelectionMaterial::initShader(context);
    }
}

int GLCRenderer::appendAndInitRenderingShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context)
{
    GLC_Shader* shader= new GLC_Shader();
    shader->setVertexAndFragmentShader(vertexFile, fragmentFile);
    shader->createAndCompileProgrammShader();
    shaders.append(shader);
    return shaders.size() - 1;
}

bool GLCRenderer::setRenderingShaders(int index)
{
    bool shaderSet = false;
    if (index < 0)
    {
        shaderID = 0;
    }
    else if (index >= 0 && index < shaders.size())
    {
        shaderID = shaders[index]->id();
        shaderSet = true;
    }

    return shaderSet;
}


void GLCRenderer::setBackgroundColor(const QColor &color)
{
    glcViewport.setBackgroundColor(color);
}

void GLCRenderer::setCamera(const CameraSettings &camera)
{
    const GLC_Camera cam(GLC_Point3d(camera.eye.x, camera.eye.y, camera.eye.z),
                         GLC_Point3d(camera.target.x, camera.target.y, camera.target.z),
                         GLC_Point3d(camera.up.x, camera.up.y, camera.up.z));
    glcViewport.cameraHandle()->setCam(cam);
}

void GLCRenderer::setCamera(const CameraView& view)
{
    switch (view)
    {
    case CameraView::FRONT:
        glcViewport.cameraHandle()->setFrontView();
        break;
    case CameraView::BACK:
        glcViewport.cameraHandle()->setRearView();
        break;
    case CameraView::LEFT:
        glcViewport.cameraHandle()->setLeftView();
        break;
    case CameraView::RIGHT:
        glcViewport.cameraHandle()->setRightView();
        break;
    case CameraView::TOP:
        glcViewport.cameraHandle()->setTopView();
        break;
    case CameraView::BOTTOM:
        glcViewport.cameraHandle()->setBottomView();
        break;
    case CameraView::ISO:
    default:
        glcViewport.cameraHandle()->setIsoView();
    }

    if (!glcWorld.isEmpty() && !glcWorld.boundingBox().isEmpty())
        glcViewport.reframe(glcWorld.boundingBox());
    else
    {
        repoLogError("GLC world is empty or bounding box is empty!");
    }
}

void GLCRenderer::createSPBoxes(
        const std::shared_ptr<repo_partitioning_tree_t> &tree,
        const std::vector<std::vector<float>>   &currentBox,
         GLC_Material                      *mat
        )
{
    if(tree)
    {
        //visualise this box
        GLC_Point3d lower (currentBox[0][0], currentBox[0][1], currentBox[0][2]);
        GLC_Point3d higher(currentBox[1][0], currentBox[1][1], currentBox[1][2]);

        GLC_BoundingBox glcBbox(lower, higher);
        auto box = GLC_Factory::instance()->createBox(glcBbox);
        box.geomAt(0)->replaceMasterMaterial(mat);
        glcViewCollection.add(box);

        //recursively call this function for children

        if(tree->type != repo::PartitioningTreeType::LEAF_NODE)
        {
            auto median = tree->pValue;
            auto rightBox = currentBox;
            auto leftBox = currentBox;
            int axis = tree->type == repo::PartitioningTreeType::PARTITION_X? 0 :
                                        (tree->type == repo::PartitioningTreeType::PARTITION_Y? 1 : 2);
			double offsetAxis = offset.size() ? offset[axis] : 0;
			rightBox[0][axis] = median + offsetAxis;
			leftBox[1][axis] = median + offsetAxis;
            createSPBoxes(tree->left, leftBox, mat);
            createSPBoxes(tree->right, rightBox, mat);
        }

    }

}

void GLCRenderer::toggleGenericPartitioning(
                       const std::vector<repo_vector_t> &sceneBbox,
                       const std::shared_ptr<repo_partitioning_tree_t> &tree)
{

    if (glcViewCollection.isEmpty())
    {
        if(tree)
        {
            static GLC_Material* mat = new GLC_Material(Qt::yellow);
            mat->setOpacity(0.1);

            std::vector<std::vector<float>> bbox = {
                {sceneBbox[0].x, sceneBbox[0].y, sceneBbox[0].z},
                {sceneBbox[1].x, sceneBbox[1].y, sceneBbox[1].z}
            };

            if(offset.size())
            {
                for(int i = 0; i < offset.size(); ++i)
                {
                    bbox[0][i] += offset[i];
                    bbox[1][i] += offset[i];
                }
            }

            repoLog("Bounding box: ["+std::to_string(sceneBbox[0].x)+","+std::to_string(sceneBbox[0].y)+","+std::to_string(sceneBbox[0].z)
                    +"]["+std::to_string(sceneBbox[1].x)+","+std::to_string(sceneBbox[1].y)+","+std::to_string(sceneBbox[1].z)
                    +"]");
            createSPBoxes(tree, bbox, mat);
        }

    }
    else
        glcViewCollection.clear();

}

void GLCRenderer::createMeshBBoxes(
        const repo::core::model::RepoScene            *scene,
        const repo::core::model::RepoScene::GraphType &gType,
        const repo::core::model::RepoNode             *node,
        const std::vector<float>                      &matrix,
              GLC_Material                            *mat)
{
    switch(node->getTypeAsEnum())
    {
        case repo::core::model::NodeType::MESH:
        {
            auto meshPtr = dynamic_cast<const repo::core::model::MeshNode*>(node);
            if(meshPtr)
            {
                auto mappings = meshPtr->getMeshMapping();
                if(mappings.size()>1)
                {
                    for(const auto &map : mappings)
                    {
                        auto min = multiplyMatVec(matrix,  map.min);
                        auto max = multiplyMatVec(matrix,  map.max);

                        GLC_Point3d lower (min.x, min.y, min.z);
                        GLC_Point3d higher(max.x, max.y, max.z);

                        GLC_BoundingBox glcBbox(lower, higher);
                        auto box = GLC_Factory::instance()->createBox(glcBbox);
                        box.geomAt(0)->replaceMasterMaterial(mat);
                        glcViewCollection.add(box);
                    }
                }
                else
                {
                    //single mesh, visualise this mesh's bounding box
                    auto currentBox = meshPtr->getBoundingBox();
                    for(auto &entry : currentBox)
                    {
                        entry = multiplyMatVec(matrix, entry);
                    }

                    GLC_Point3d lower (currentBox[0].x, currentBox[0].y, currentBox[0].z);
                    GLC_Point3d higher(currentBox[1].x, currentBox[1].y, currentBox[1].z);

                    GLC_BoundingBox glcBbox(lower, higher);
                    auto box = GLC_Factory::instance()->createBox(glcBbox);
                    box.geomAt(0)->replaceMasterMaterial(mat);
                    glcViewCollection.add(box);
                }

            }

        }
        break;
        case repo::core::model::NodeType::TRANSFORMATION:
        {
            auto transPtr = dynamic_cast<const repo::core::model::TransformationNode*>(node);
            auto newTrans = matMult(matrix, transPtr->getTransMatrix(false));
            auto children = scene->getChildrenAsNodes(gType, transPtr->getSharedID());
            for(const auto &child : children)
            {
                createMeshBBoxes(scene, gType, child, newTrans, mat);
            }

        }
        break;
    }



}

void GLCRenderer::toggleMeshBoundingBoxes(
                     const repo::core::model::RepoScene *scene)
{
    if (glcViewCollection.isEmpty())
    {
        if(scene)
        {
            GLC_Material* meshBboxMat = new GLC_Material(Qt::cyan);
            meshBboxMat->setOpacity(0.1);

            std::vector<float> identity = {
                                           1,0,0,0,
                                           0,1,0,0,
                                           0,0,1,0,
                                           0,0,0,1
                                          };

            if(offset.size())
            {
                identity[3] += offset[0];
                identity[7] += offset[1];
                identity[11] += offset[2];
            }

            auto gType = scene->getViewGraph();
            createMeshBBoxes(scene, gType, scene->getRoot(gType), identity, meshBboxMat);
        }

    }
    else
    {
        glcViewCollection.clear();
    }

}


void GLCRenderer::toggleOctree()
{
    if (glcViewCollection.isEmpty())
    {
        //FIXME: this is a memory leak
        GLC_Material* mat = new GLC_Material(Qt::red);
        mat->setOpacity(0.1);
        GLC_SpacePartitioning* spacePartitioning = glcWorld.collection()->spacePartitioningHandle();
        if (spacePartitioning)
        {
            GLC_Octree* octree = dynamic_cast<GLC_Octree*>(spacePartitioning);
            if (octree)
            {
                octree->createBox(mat, &glcViewCollection);

            }
        }
    }
    else
        glcViewCollection.clear();
}

void GLCRenderer::toggleProjection()
{
    glcViewport.setToOrtho(!glcViewport.useOrtho());
}

void GLCRenderer::toggleSelectAll()
{
    if (glcWorld.collection()->selectionSize() ==
            glcWorld.collection()->drawableObjectsSize())
        glcWorld.unselectAll();
    else
        glcWorld.selectAllWith3DViewInstanceInCurrentShowState();
}

void GLCRenderer::toggleWireframe()
{
    isWireframe = !isWireframe;
    glcWorld.collection()->setPolygonModeForAll(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
}

void GLCRenderer::updateClippingPlane()
{
    GLC_CuttingPlane* cuttingPlane= dynamic_cast<GLC_CuttingPlane*>(sender());
    if (cuttingPlane)
    {
        double reverse = clippingPlaneReverse ? -1.0 : 1.0;
        clippingPlane->setPlane(reverse * cuttingPlane->normal(), cuttingPlane->center());
    }
}

void GLCRenderer::setClippingPlaneVisibility(bool on)
{
    // Clipping plane has two components:
    // 1) openGL clippingPlane
    // 2) three clippingPlaneWidgets (visual rectangles), one per axis

    if (on)
    {
        clippingPlane = new GLC_Plane();
        glcViewport.addClipPlane(GL_CLIP_PLANE0, clippingPlane);

        if (!clippingPlaneWidgets[0] && !clippingPlaneWidgets[1] && !clippingPlaneWidgets[2])
        {
            GLC_BoundingBox bbox = glcWorld.boundingBox();
            double margin = 0.2 * std::max<double>(std::max<double>(bbox.xLength(), bbox.yLength()), bbox.zLength());
            double x = bbox.xLength() + margin;
            double y = bbox.yLength() + margin;
            double z = bbox.zLength() + margin;
            clippingPlaneWidgets[(int)Axis::X] = createCuttingPlane(bbox.center(), GLC_Vector3d(-glc::X_AXIS), z, y);
            clippingPlaneWidgets[(int)Axis::Y] = createCuttingPlane(bbox.center(), GLC_Vector3d(-glc::Y_AXIS), x, z);
            clippingPlaneWidgets[(int)Axis::Z] = createCuttingPlane(bbox.center(), GLC_Vector3d(-glc::Z_AXIS), x, y);
        }
    }
    else
    {
          glcViewport.removeClipPlane(GL_CLIP_PLANE0);
          for (auto w : clippingPlaneWidgets)
          {
              if (w)
                  w->setVisible(false);
          }
    }
}

GLC_CuttingPlane* GLCRenderer::createCuttingPlane(const GLC_Point3d &centroid, const GLC_Point3d &normal, double l1, double l2)
{
    GLC_CuttingPlane* cuttingPlane = new GLC_CuttingPlane(centroid, normal, l1, l2);
    cuttingPlane->setOpacity(0);
    connect(cuttingPlane, SIGNAL(asChanged()), this, SLOT(updateClippingPlane()));
    glc3DWidgetManager.add3DWidget(cuttingPlane);
    cuttingPlane->setVisible(false);
    return cuttingPlane;
}

void GLCRenderer::updateClippingPlane(Axis axis, double value, bool reverse)
{    
    clippingPlaneReverse = reverse;

    // Hide any previously visible clipping plane widgets
    for (auto w : clippingPlaneWidgets)
    {
        if (w)
            w->setVisible(false);
    }
    // Enable clipping plane if necessary
    setClippingPlaneVisibility(true);

    //--------------------------------------------------------------------------
    // Calculate clipping plane position
    GLC_BoundingBox bbox = glcWorld.boundingBox();
    GLC_Point3d centroid = bbox.center();

    double pos;
    switch (axis)
    {
    case Axis::X:
        pos = centroid.x() - bbox.xLength()/2 + (bbox.xLength() * (value));
        centroid.setX(pos);
        break;
    case Axis::Y:
        pos = centroid.y() - bbox.yLength()/2 + (bbox.yLength() * (value));
        centroid.setY(pos);
        break;
    case Axis::Z:
        pos = centroid.z() - bbox.zLength()/2 + (bbox.zLength() * (value));
        centroid.setZ(pos);
        break;
    }

    //--------------------------------------------------------------------------
    // Update position of the clipping plane widget and make sure it is set visible
    if (clippingPlaneWidgets.size() > (int) axis)
    {
        GLC_CuttingPlane* clippingPlaneWidget = clippingPlaneWidgets[(int) axis];
        // The move action does not get to the desired position on the first shot
        // therefore iterate multiple times until reached. Stop at some fixed
        // large number just in case to prevent accidental infinite looping.
        int i = 0;
        glc::WidgetEventFlag result;
        clippingPlaneWidget->select(clippingPlaneWidget->center(), clippingPlaneWidget->id());
        do {

            result = clippingPlaneWidget->move(centroid, clippingPlaneWidget->id());
            ++i;
        }
        while (clippingPlaneWidget->center() != centroid && i < 1000);

        clippingPlaneWidget->setVisible(true);
    }
}

void GLCRenderer::zoom(const float &zoom)
{
    glcViewport.cameraHandle()->zoom(zoom);
    emit cameraChanged(getCurrentCamera());
}

