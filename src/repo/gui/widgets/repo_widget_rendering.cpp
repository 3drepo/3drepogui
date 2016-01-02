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

Rendering3DWidget::Rendering3DWidget(QWidget* parent, Renderer rType, const QString& windowTitle)
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
    this->setWindowIcon(repo::gui::primitive::RepoFontAwesome::getInstance().getIcon(
                            repo::gui::primitive::RepoFontAwesome::fa_cube));
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setFocusPolicy(Qt::StrongFocus);

	// To register mouse events on move (by default only on press).
	this->setMouseTracking(true);

	instantiateRenderer(rType);

	//--------------------------------------------------------------------------
    QObject::connect(
		renderer, &renderer::AbstractRenderer::repaintNeeded,
        this, &Rendering3DWidget::repaintCurrent);

	QObject::connect(
		renderer, &renderer::AbstractRenderer::cameraChangedSignal,
		this, &Rendering3DWidget::broadcastCameraChange);


}

void Rendering3DWidget::repaintCurrent()
{
    makeCurrent();
    paintGL();
    doneCurrent();
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
	}
	else
	{
		repoLogError("Unsupported Rendering type: " + (int) rendType);
	}
}

void Rendering3DWidget::initializeShaders()
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
}


void Rendering3DWidget::resizeGL(int width, int height)
{
    makeCurrent();
	renderer->resizeWindow(width, height);
	
}

//  ========== Not used at all. commenting them out for now. =========
//void Rendering3DWidget::reframe(const GLC_BoundingBox& boundingBox)
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
//void Rendering3DWidget::reframeOnSelection()
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

void Rendering3DWidget::broadcastCameraChange(const repo::gui::renderer::CameraSettings &camera, const bool &emitSignal)
{
	emit cameraChangedSignal(camera, emitSignal);
}

void Rendering3DWidget::setCamera(const repo::gui::renderer::CameraSettings &camera, const bool &emitSignal)
{
	renderer->setCamera(camera, emitSignal);
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

//void Rendering3DWidget::setGLCMeshColors(
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
//void Rendering3DWidget::setGLCMeshOpacity(const QString &name, qreal opacity)
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
//void Rendering3DWidget::setGLCOccurrenceRenderProperties(
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
//void Rendering3DWidget::setGLCOccurrenceOpacity(
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
//void Rendering3DWidget::setGLCOccurenceVisibility(const QString &occurrenceName, bool visible)
//{
//    QHash<QString, GLC_StructOccurrence *>::iterator it = glcOccurrences.find(occurrenceName);
//	if (glcOccurrences.end() != it)
//	{
//        GLC_StructOccurrence * oc = it.value();
//		oc->setVisibility(visible);
//	}
//}

void Rendering3DWidget::setInfoVisibility(const bool visible)
{
	isInfoVisible = visible;
}

void Rendering3DWidget::setBackgroundColor(
	const QColor &color,
    const bool isupdate)
{
	renderer->setBackgroundColor(color);
    if (isupdate)
        update();
}

void Rendering3DWidget::linkCameras(
	const Rendering3DWidget *widget,
	const bool & on) const
{
	if (on)
	{
		connect(
			this, &Rendering3DWidget::cameraChangedSignal,
			widget, &Rendering3DWidget::setCamera);
		// TODO: align all views
	}
	else
	{
		disconnect(
			this, &Rendering3DWidget::cameraChangedSignal,
			widget, &Rendering3DWidget::setCamera);
	}
}

//------------------------------------------------------------------------------
//
// Setters
//
//------------------------------------------------------------------------------

void Rendering3DWidget::setRepoScene(repo::core::model::RepoScene *repoScene)
{
	
	connect(
		renderer, &renderer::AbstractRenderer::modelLoadProgress,
		this, &Rendering3DWidget::rendererProgress);

	connect(
		this, &Rendering3DWidget::cancelRenderingOps,
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

//std::vector<std::string> Rendering3DWidget::getSelectionList() const
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
//repo::core::model::RepoNode* Rendering3DWidget::getSelectedNode() const
//{
//	repo::core::model::RepoNode *node = 0;
//	if (glcWorld.selectionSize() > 0)
//	{
//        GLC_StructOccurrence* o = glcWorld.selectedOccurrenceList().first();
//	}
//	return node;
//}
//
//GLC_Mesh* Rendering3DWidget::getGLCMesh(const QString &name) const
//{
//	GLC_Mesh * glcMesh = 0;
//	QHash<QString, GLC_Mesh*>::const_iterator it = glcMeshes.find(name);
//	if (glcMeshes.end() != it)
//		glcMesh = it.value();
//	return glcMesh;
//}

QImage Rendering3DWidget::renderQImage(int w, int h)
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
//void Rendering3DWidget::addBoundingBox(
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
//void Rendering3DWidget::clearBoundingBoxes()
//{
//	glcViewCollection.clear();
//}
//
//------------------------------------------------------------------------------
//
// User interaction
//
//------------------------------------------------------------------------------
void Rendering3DWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
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
		QColor color = QColorDialog::getColor(Qt::white,
			this,
			"Color picker");
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
	case Qt::Key_F1: // Points
	{

		renderer->renderingMode(renderer::RenderMode::POINT);
        update();
		break;
	}
	case Qt::Key_F2: // Triangle wireframe
	{
		renderer->renderingMode(renderer::RenderMode::WIREFRAME);
        update();
		break;
	}
	case Qt::Key_F3: // Shading with polygon wireframe
	{	
		renderer->renderingMode(renderer::RenderMode::WIREFRAME_SHADING);
        update();
		break;
	}
	case Qt::Key_F4: // Shading
	{
		renderer->renderingMode(renderer::RenderMode::SHADING);
        update();
		break;
	}
	}
	// Pass on the event to parent.
    QOpenGLWidget::keyPressEvent(e);
}
void Rendering3DWidget::mousePressEvent(QMouseEvent *e)
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
	
	if (mousePressed && renderer->move(e->x(), e->y()))
	{
		//in Navigation mode
		update();
		emit cameraChangedSignal(renderer->getCurrentCamera(), false);
	}

	// Pass on the event to parent.
    QOpenGLWidget::mouseMoveEvent(e);
}
void Rendering3DWidget::mouseReleaseEvent(QMouseEvent *e)
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

void Rendering3DWidget::select(int x, int y, bool multiSelection,
    QMouseEvent *event)
{
	renderer->selectComponent(x, y, multiSelection);
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
