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
#include "../../primitives/repo_fontawesome.h"
#include "../../primitives/repo_color.h"
#include "../logger/repo_logger.h"
#include "../renderer/repo_renderer_glc.h"
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


using namespace repo::gui::widgets;

//------------------------------------------------------------------------------
//
// Static variables
//
//------------------------------------------------------------------------------
const double RepoRenderingWidget::ZOOM_FACTOR = 1.2;

//QList<GLC_Shader*> RepoRenderingWidget::shaders;

RepoRenderingWidget::RepoRenderingWidget(QWidget* parent, Renderer rType, const QString& windowTitle)
    : QOpenGLWidget(parent)
	, isWireframe(false)
	, isInfoVisible(true)
    , repoScene(0)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format); // must be called before the widget or its parent window gets shown

	//--------------------------------------------------------------------------
	// Default settings
    this->setWindowTitle(windowTitle);
	this->setToolTip(windowTitle);
	this->setWindowIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_cube));
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setFocusPolicy(Qt::StrongFocus);

	// To register mouse events on move (by default only on press).
	this->setMouseTracking(true);

	instantiateRenderer(rType);

	//--------------------------------------------------------------------------
    QObject::connect(
		renderer, &renderer::AbstractRenderer::repaintNeeded,
        this, &RepoRenderingWidget::repaintCurrent);

	QObject::connect(
		renderer, &renderer::AbstractRenderer::cameraChangedSignal,
		this, &RepoRenderingWidget::broadcastCameraChange);


}

void RepoRenderingWidget::repaintCurrent()
{
    makeCurrent();
    paintGL();
    doneCurrent();
}

RepoRenderingWidget::~RepoRenderingWidget()
{
    makeCurrent();


    // FIXME: delete shader
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
void RepoRenderingWidget::initializeGL()
{
	//--------------------------------------------------------------------------
	// Renderer initialisation
	renderer->initialize();

	initializeShaders();


	renderer->setActivationFlag(true);
}

void RepoRenderingWidget::instantiateRenderer(Renderer rendType)
{
	if (rendType == Renderer::GLC)
	{
		renderer = new renderer::GLCRenderer();
	}
	else
	{
		repoLogError("Unsupported Rendering type: " + (int) rendType);
	}
}

void RepoRenderingWidget::initializeShaders()
{
    makeCurrent();

	QFile vertexShaderFile(":/shaders/select.vert");
	QFile fragmentShaderFile(":/shaders/select.frag");

	if (vertexShaderFile.exists() && fragmentShaderFile.exists())
	{
		try
		{
			renderer->setAndInitShaders(vertexShaderFile, fragmentShaderFile, context());

		}
		catch (GLC_Exception e){
			repoLogError("Init shader failed " + std::string(e.what()));
		}
	}
}

void RepoRenderingWidget::paintGL()
{
	if (isInfoVisible)
	{
		QPainter painter(this);
		QSize screenSize(size());
		
        renderer->render(&painter, screenSize.height(), screenSize.width());
	}
	else
		renderer->render(nullptr);	
}


void RepoRenderingWidget::resizeGL(int width, int height)
{
    makeCurrent();
	renderer->resizeWindow(width, height);
	
}

//  ========== Not used at all. commenting them out for now. =========
//void RepoRenderingWidget::reframe(const GLC_BoundingBox& boundingBox)
//{
//	const GLC_BoundingBox collectionBox = glcWorld.boundingBox();
//	if (boundingBox.isEmpty())
//	{
//		if (!collectionBox.isEmpty())
//		{
//			GLC_Camera savCam(*(glcViewport.cameraHandle()));
//			glcViewport.reframe(collectionBox);
//            update();
//			emit cameraChangedSignal(*glcViewport.cameraHandle());
//		}
//	}
//	else
//	{
//		GLC_Camera savCam(*(glcViewport.cameraHandle()));
//		glcViewport.reframe(boundingBox);
//		if (savCam == *(glcViewport.cameraHandle()))
//		{
//			glcViewport.reframe(collectionBox);
//		}
//        update();
//		emit cameraChangedSignal(*glcViewport.cameraHandle());
//	}
//}
//
//void RepoRenderingWidget::reframeOnSelection()
//{
//	GLC_BoundingBox SelectionBox;
//	PointerViewInstanceHash* pSelections = glcWorld.collection()->selection();
//	PointerViewInstanceHash::iterator iEntry = pSelections->begin();
//	while (iEntry != pSelections->constEnd())
//	{
//		SelectionBox.combine(iEntry.value()->boundingBox());
//		++iEntry;
//	}
//	reframe(SelectionBox);
//}


//------------------------------------------------------------------------------
//
// Public slots
//
//------------------------------------------------------------------------------

void RepoRenderingWidget::broadcastCameraChange(const repo::gui::renderer::CameraSettings &camera)
{
	emit cameraChangedSignal(camera);
}

void RepoRenderingWidget::setCamera(const repo::gui::renderer::CameraSettings &camera)
{

	renderer->setCamera(camera);
    update();
}
void RepoRenderingWidget::setPredefinedCamera(const repo::gui::renderer::CameraView& view)
{
	renderer->setCamera(view);
    update();
}
//
//void RepoRenderingWidget::setGLCMeshColors(
//	const QString &name,
//	const QVector<GLfloat> &colors,
//    const bool isupdate)
//{
//	GLC_Mesh* glcMesh = getGLCMesh(name);
//	if (glcMesh)
//	{
//		//	glcMesh->setColorPearVertex(true);
//		// TODO: fix me, needs to set color per mesh
//        if (isupdate)
//            update();
//		//QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
//	}
//}
//
//void RepoRenderingWidget::setGLCMeshColors(
//	const float r,
//	const float g,
//	const float b,
//	const float a)
//{
//	QHash<QString, GLC_Mesh*>::iterator it;
//
//	for (it = glcMeshes.begin(); it != glcMeshes.end(); ++it)
//	{
//		GLC_Mesh *glcMesh = it.value();
//		// Always [r,g,b,a] hence times 4
//		QVector<GLfloat> vector(glcMesh->VertexCount() * 4);
//		for (size_t i = 0; i < vector.size(); i += 4)
//		{
//			vector[i + 0] = r;
//			vector[i + 1] = g;
//			vector[i + 2] = b;
//			vector[i + 3] = a;
//		}
//		setGLCMeshColors(glcMesh->name(), vector);
//	}
//    update();
//}
//
//void RepoRenderingWidget::setGLCMeshOpacity(const QString &name, qreal opacity)
//{
//	GLC_Mesh* glcMesh = getGLCMesh(name);
//	if (glcMesh)
//	{
//		QSet<GLC_Material*> materials = glcMesh->materialSet();
//		QSet<GLC_Material*>::iterator it;
//
//		for (it = materials.begin(); it != materials.end(); ++it)
//		{
//			GLC_Material *mat = *it;
//			mat->setOpacity(opacity);
//		}
//	}
//}
//
//void RepoRenderingWidget::setGLCOccurrenceRenderProperties(
//	const QString &occurrenceName,
//	const GLC_RenderProperties &properties)
//{
//    GLC_StructOccurrence *oc;
//    QHash<QString, GLC_StructOccurrence *>::iterator it =
//		glcOccurrences.find(occurrenceName);
//
//    QHash<QString, GLC_StructOccurrence *>::iterator itt =
//		glcMeshOccurences.find(occurrenceName);
//	if (glcMeshOccurences.end() != itt)
//		oc = itt.value();
//	else if (glcOccurrences.end() != it)
//		oc = it.value();
//
//	if (oc)
//		oc->setRenderProperties(properties);
//
//}
//
//void RepoRenderingWidget::setGLCOccurrenceOpacity(
//	const QString &occurrenceName, qreal opacity, const QColor &color)
//{
//	repoLog("changing occurence properties for " + occurrenceName.toStdString());
//	GLC_RenderProperties properties;
//	if (opacity < 1.0)
//	{
//		//	properties.setOverwriteTransparency(opacity);
//		// This does not create memory leak as the overwrite material deletes
//		// previous one if set and not used.
//		properties.setOverwriteMaterial(new GLC_Material(color));
//		properties.setRenderingMode(glc::OverwriteMaterial);
//	}
//	else
//		properties.setRenderingMode(glc::NormalRenderMode);
//	setGLCOccurrenceRenderProperties(occurrenceName, properties);
//}
//
//void RepoRenderingWidget::setGLCOccurenceVisibility(const QString &occurrenceName, bool visible)
//{
//    QHash<QString, GLC_StructOccurrence *>::iterator it = glcOccurrences.find(occurrenceName);
//	if (glcOccurrences.end() != it)
//	{
//        GLC_StructOccurrence * oc = it.value();
//		oc->setVisibility(visible);
//	}
//}

void RepoRenderingWidget::setInfoVisibility(const bool visible)
{
	isInfoVisible = visible;
}

void RepoRenderingWidget::setBackgroundColor(
	const QColor &color,
    const bool isupdate)
{
	renderer->setBackgroundColor(color);
    if (isupdate)
        update();
}

void RepoRenderingWidget::linkCameras(
	const RepoRenderingWidget *widget,
	const bool & on) const
{
	if (on)
	{
		connect(
			this, &RepoRenderingWidget::cameraChangedSignal,
			widget, &RepoRenderingWidget::setCamera);
		// TODO: align all views
	}
	else
	{
		disconnect(
			this, &RepoRenderingWidget::cameraChangedSignal,
			widget, &RepoRenderingWidget::setCamera);
	}
}

//------------------------------------------------------------------------------
//
// Setters
//
//------------------------------------------------------------------------------

void RepoRenderingWidget::setRepoScene(repo::core::model::RepoScene *repoScene)
{
	
	connect(
		renderer, &renderer::AbstractRenderer::modelLoadProgress,
		this, &RepoRenderingWidget::rendererProgress);

	connect(
		this, &RepoRenderingWidget::cancelRenderingOps,
		renderer, &renderer::AbstractRenderer::cancelOperations);
	if (repoScene)
	{
		if (this->repoScene)
			delete this->repoScene;

		this->repoScene = repoScene;

		if (renderer)
		{
			renderer->loadModel(repoScene);
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

//std::vector<std::string> RepoRenderingWidget::getSelectionList() const
//{
//	std::vector<std::string> selectedNames;
//    QList<GLC_StructOccurrence *> occurrences = glcWorld.selectedOccurrenceList();
//    QList<GLC_StructOccurrence *>::iterator it;
//	for (it = occurrences.begin(); it != occurrences.end(); ++it)
//	{
//        GLC_StructOccurrence *oc = *it;
//		selectedNames.push_back(oc->name().toStdString());
//	}
//	return selectedNames;
//}
//
//repo::core::model::RepoNode* RepoRenderingWidget::getSelectedNode() const
//{
//	repo::core::model::RepoNode *node = 0;
//	if (glcWorld.selectionSize() > 0)
//	{
//        GLC_StructOccurrence* o = glcWorld.selectedOccurrenceList().first();
//	}
//	return node;
//}
//
//GLC_Mesh* RepoRenderingWidget::getGLCMesh(const QString &name) const
//{
//	GLC_Mesh * glcMesh = 0;
//	QHash<QString, GLC_Mesh*>::const_iterator it = glcMeshes.find(name);
//	if (glcMeshes.end() != it)
//		glcMesh = it.value();
//	return glcMesh;
//}

QImage RepoRenderingWidget::renderQImage(int w, int h)
{
	// See https://bugreports.qt-project.org/browse/QTBUG-33186

	makeCurrent();

	// Set the rendering engine to the size of the image to render
	// Also set the format so that the depth buffer will work
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	QOpenGLFramebufferObject qfb(w, h, format);
	qfb.bind();

	QImage image;
	// If the frame buffer does not work then return an empty image
	if (qfb.isValid())
	{
		// Draw the scene to the buffer
		isInfoVisible = false;
		resizeGL(w, h); // resize scene
        update(); // draw to the buffer
		qfb.release();
		image = qfb.toImage();

		isInfoVisible = true;
		resizeGL(width(), height());
        update();
	}
	return(image);
}

//
////------------------------------------------------------------------------------
//// Create GLC_Object to display
//void RepoRenderingWidget::addBoundingBox(
//	const double lx,
//	const double ly,
//	const double lz,
//	const std::vector<double>& transformationMatrix)
//{
//	GLC_Box* pBox = new GLC_Box(lx, ly, lz);
//	GLC_3DViewInstance box(pBox);
//
//	// Rotate and translate the box by the transformation matrix
//	box.multMatrix(GLC_Matrix4x4(&transformationMatrix[0]));
//
//	// Replace GLC_Box default material
//	GLC_Material * material = new GLC_Material(Qt::gray);
//	material->setOpacity(0.1);
//	box.geomAt(0)->replaceMasterMaterial(material);
//
//	// Add the box to the collection
//	glcViewCollection.add(box);
//}
//
//void RepoRenderingWidget::clearBoundingBoxes()
//{
//	glcViewCollection.clear();
//}
//
//void RepoRenderingWidget::toggleOctree()
//{
//	if (glcViewCollection.isEmpty())
//	{
//		GLC_Material* mat = new GLC_Material(Qt::red);
//		mat->setOpacity(0.1);
//		GLC_SpacePartitioning* spacePartitioning = glcWorld.collection()->spacePartitioningHandle();
//		if (spacePartitioning)
//		{
//			GLC_Octree* octree = dynamic_cast<GLC_Octree*>(spacePartitioning);
//			if (octree)
//			{
//				octree->createBox(mat, &glcViewCollection);
//
//			}
//		}
//	}
//	else
//		glcViewCollection.clear();
//    update();
//}

//------------------------------------------------------------------------------
//
// User interaction
//
//------------------------------------------------------------------------------
void RepoRenderingWidget::keyPressEvent(QKeyEvent *e)
{
	//switch (e->key())
	//{
	//case Qt::Key_0:
	//	renderer->setShaderID(0);
 //       update();
	//	break;
	//case Qt::Key_1:
	//	if (shaders.size() > 0)
	//	{
	//		shaderID = shaders[0]->id();
 //           update();
	//	}
	//	break;
	//case Qt::Key_2:
	//	if (shaders.size() > 1)
	//	{
	//		shaderID = shaders[1]->id();
 //           update();
	//	}
	//	break;
	//case Qt::Key_3:
	//	if (shaders.size() > 2)
	//	{
	//		shaderID = shaders[2]->id();
 //           update();
	//	}
	//	break;
	//case Qt::Key_Minus:
	//case Qt::Key_Underscore:
	//	glcViewport.cameraHandle()->zoom(1 / ZOOM_FACTOR);
 //       update();
	//	emit cameraChangedSignal(*glcViewport.cameraHandle());
	//	break;
	//case Qt::Key_Plus:
	//case Qt::Key_Equal:
	//	glcViewport.cameraHandle()->zoom(ZOOM_FACTOR);
	//	emit cameraChangedSignal(*glcViewport.cameraHandle());
 //       update();
	//	break;
	//case Qt::Key_A:
	//	if ((e->modifiers() == Qt::ControlModifier))
	//	{
	//		if (glcWorld.collection()->selectionSize() ==
	//			glcWorld.collection()->drawableObjectsSize())
	//			glcWorld.unselectAll();
	//		else
	//			glcWorld.selectAllWith3DViewInstanceInCurrentShowState();
 //           update();
	//	}
	//case Qt::Key_T:
	//	if (glIsEnabled(GL_TEXTURE_2D))
	//		glDisable(GL_TEXTURE_2D);
	//	else
	//		glEnable(GL_TEXTURE_2D);
 //       update();
	//	break;
	//case Qt::Key_C:
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
 //       update();
	//	break;
	//case Qt::Key_L:
	//	if (glIsEnabled(GL_LIGHTING))
	//		glDisable(GL_LIGHTING);
	//	else
	//		glEnable(GL_LIGHTING);
 //       update();
	//	break;
	//case Qt::Key_W:
	//{
	//	isWireframe = !isWireframe;
	//	setMode(isWireframe ? GL_LINE : GL_FILL);
 //       update();
	//	break;
	//}
	//case Qt::Key_P:
	//	glcViewport.setToOrtho(!glcViewport.useOrtho());
 //       update();
	//	break;
	//case Qt::Key_R:
	//{
	//	QHash<QString, GLC_Mesh*>::iterator it;
	//	for (it = glcMeshes.begin(); it != glcMeshes.end(); ++it)
	//	{
	//		GLC_Mesh *glcMesh = it.value();
	//		float r = (float)rand() / (RAND_MAX);
	//		float g = (float)rand() / (RAND_MAX);
	//		float b = (float)rand() / (RAND_MAX);
	//		// Always [r,g,b,a] hence times 4
	//		QVector<GLfloat> vector(glcMesh->VertexCount() * 4);
	//		for (unsigned int i = 0; i < vector.size(); i += 4)
	//		{
	//			vector[i + 0] = r;
	//			vector[i + 1] = g;
	//			vector[i + 2] = b;
	//			vector[i + 3] = 0.9f;
	//		}
	//		setGLCMeshColors(glcMesh->name(), vector);
	//	}
 //       update();
	//	break;
	//}
	//case Qt::Key_Q:
	//{
	//	QColor color = QColorDialog::getColor(Qt::white,
	//		this,
	//		"Color picker");
	//	if (color.isValid())
	//	{
	//		setBackgroundColor(color);
	//	}
	//	break;
	//}
	//case  Qt::Key_O:
	//{
	//	toggleOctree();
	//}
	//break;
	//case Qt::Key_F1: // Points
	//{
	//	setMode(GL_POINT);
	//	setRenderingFlag(glc::ShadingFlag);
 //       update();
	//	break;
	//}
	//case Qt::Key_F2: // Triangle wireframe
	//{
	//	setMode(GL_LINE);
	//	setRenderingFlag(glc::ShadingFlag);
 //       update();
	//	break;
	//}
	//case Qt::Key_F3: // Shading with polygon wireframe
	//{
	//	setMode(GL_FILL);
	//	setRenderingFlag(glc::WireRenderFlag);
 //       update();
	//	break;
	//}
	//case Qt::Key_F4: // Shading
	//{
	//	setMode(GL_FILL);
	//	setRenderingFlag(glc::ShadingFlag);
 //       update();
	//	break;
	//}
	//}
	// Pass on the event to parent.
    QOpenGLWidget::keyPressEvent(e);
}
void RepoRenderingWidget::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case (Qt::RightButton) :
		this->setCursor(Qt::ClosedHandCursor);
        renderer->startNavigation(renderer::NavMode::TURNTABLE, e->x(), e->y());
        update();
		break;
	case (Qt::LeftButton) :
		this->setCursor(Qt::SizeAllCursor);
        renderer->startNavigation(renderer::NavMode::PAN, e->x(), e->y());
        update();
		break;
	case (Qt::MidButton) :
		this->setCursor(Qt::CrossCursor);
        renderer->startNavigation(renderer::NavMode::FLY, e->x(), e->y());
        update();
		break;
	}

	mousePressed = true;

	// Pass on the event to parent.
    QOpenGLWidget::mousePressEvent(e);
}
void RepoRenderingWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	//if (Qt::LeftButton == e->button())
	//{
	//	bool multiSelection = (
	//		(e->modifiers() == Qt::ControlModifier) ||
	//		(e->modifiers() == Qt::ShiftModifier));
	//	select(e->x(), e->y(), multiSelection, e);
	//}

	// Pass on the event to parent.
    QOpenGLWidget::mouseDoubleClickEvent(e);
}
void RepoRenderingWidget::mouseMoveEvent(QMouseEvent * e)
{

	if (mousePressed && renderer->move(e->x(), e->y()))
	{
		//in Navigation mode
		update();
		emit cameraChangedSignal(renderer->getCurrentCamera());
	}

	// Pass on the event to parent.
    QOpenGLWidget::mouseMoveEvent(e);
}
void RepoRenderingWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if (mousePressed)
	{
		renderer->stopNavigation();
		this->setCursor(Qt::ArrowCursor);
		mousePressed = false;
		update();
	}
	
	// Pass on the event to parent.
    QOpenGLWidget::mouseReleaseEvent(e);
}
void RepoRenderingWidget::wheelEvent(QWheelEvent * e)
{
	if (!renderer->increaseFlyVelocity(e->delta() < 0 ? 1.0 / 1.3 : 1.3))
	{
		renderer->zoom(e->delta() > 0 ? ZOOM_FACTOR : 1 / ZOOM_FACTOR);
		update();
	}

	// Pass on the event to parent.
    QOpenGLWidget::wheelEvent(e);
}

//void RepoRenderingWidget::select(int x, int y, bool multiSelection,
//    QMouseEvent *event)
//{
//    const bool spacePartitioningIsUsed = glcWorld.collection()->spacePartitioningIsUsed();
//    if (spacePartitioningIsUsed)
//    {
//        GLC_Frustum selectionFrustum(glcViewport.selectionFrustum(x, y));
//        glcWorld.collection()->updateInstanceViewableState(selectionFrustum);
//        glcWorld.collection()->setSpacePartitionningUsage(false);
//    }
//
////	setAutoBufferSwap(false);
////    setUpdatesEnabled(false);
////    glcWorld.collection()->setLodUsage(true, &glcViewport);
//
//    GLC_uint selectionID = glcViewport.renderAndSelect(x, y);
//
////    glcWorld.collection()->setLodUsage(false, &glcViewport);
//
//
//
//    if (spacePartitioningIsUsed)
//    {
//        glcWorld.collection()->updateInstanceViewableState(glcViewport.frustum());
//        glcWorld.collection()->setSpacePartitionningUsage(true);
//    }
//
//    std::clog << "Selection ID: " << selectionID << std::endl;
//    select(selectionID, multiSelection);
//}

//void RepoRenderingWidget::select(GLC_uint selectionID,
//    bool multiSelection,
//    bool unselectSelected,
//    bool isUpdate)
//{
    //if (glcWorld.containsOccurrence(selectionID))
    //{
    //    if (unselectSelected
    //        && (!glcWorld.isSelected(selectionID))
    //        && (glcWorld.selectionSize() > 0)
    //        && (!multiSelection))
    //    {
    //        glcWorld.unselectAll();
    //        emit selectionChanged(this, getSelectionList());
    //        //emit unselectAll();
    //    }
    //    if (!glcWorld.isSelected(selectionID))
    //    {
    //        glcWorld.select(selectionID);
    //        emit selectionChanged(this, getSelectionList());
    //        //emit updateSelection(m_World.collection()->selection());
    //    }
    //    else if (unselectSelected && glcWorld.isSelected(selectionID) && multiSelection)
    //    {
    //        glcWorld.unselect(selectionID);
    //        emit selectionChanged(this, getSelectionList());
    //        //emit updateSelection(m_World.collection()->selection());
    //    }
    //    else
    //    {
    //        if (unselectSelected)
    //            glcWorld.unselectAll();
    //        glcWorld.select(selectionID);

    //        emit selectionChanged(this, getSelectionList());
    //        //emit updateSelection(m_World.collection()->selection());
    //    }
    //}
    //else if (unselectSelected && (glcWorld.selectionSize() > 0) && (!multiSelection))
    //{
    //    // if a geometry is selected, unselect it
    //    glcWorld.unselectAll();
    //    emit selectionChanged(this, getSelectionList());
    //    //emit unselectAll();
    //}

    //if (isUpdate)
    //    update();
//}

/*
void RepoRenderingWidget::select(
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
//------------------------------------------------------------------------------
//
// Scene Management
//
//------------------------------------------------------------------------------
//
//void RepoRenderingWidget::extractMeshes(GLC_StructOccurrence * occurrence)
//{
//	if (occurrence)
//	{
//		// Store the occurrence in a hash map.
//		QString occurrenceName = occurrence->name();
//		glcOccurrences.insert(occurrenceName, occurrence);
//
//		if (occurrence->structInstance() &&
//			occurrence->structInstance()->structReference())
//		{
//			GLC_StructReference * glcReference = occurrence->structInstance()->structReference();
//			if (!glcReference->representationIsEmpty())
//			{
//				GLC_3DRep * pRep = dynamic_cast<GLC_3DRep*>
//					(glcReference->representationHandle());
//				if (pRep)
//				{
//					for (int i = 0; i < pRep->numberOfBody(); ++i)
//					{
//						GLC_Mesh * glcMesh = dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));
//
//						if (glcMesh)
//						{
//							glcMesh->setColorPearVertex(true);
//							glcMeshes.insert(glcMesh->name(), glcMesh);
//							glcMeshesIds.insert(glcMesh->name(), occurrence->id());
//							glcMeshOccurences.insert(glcMesh->name(), occurrence);
//						}
//					}
//				}
//			}
//		}
//
//		//----------------------------------------------------------------------
//		// Children
//        QList<GLC_StructOccurrence*> children = occurrence->children();
//        QList<GLC_StructOccurrence*>::iterator it;
//		for (it = children.begin(); it != children.end(); ++it)
//		{
//            GLC_StructOccurrence *child = *it;
//			extractMeshes(child);
//		}
//	}
//}
//
