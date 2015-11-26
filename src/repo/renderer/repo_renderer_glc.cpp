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
#include "../workers/repo_worker_glc_export.h"
#include <repo/core/model/bson/repo_bson_factory.h>
//------------------------------------------------------------------------------
#include <GLC_UserInput>
#include <GLC_Context>
#include <GLC_Octree>
#include <GLC_State>
#include <glc_renderstatistics.h>
//------------------------------------------------------------------------------

using namespace repo::gui::renderer;

GLCRenderer::GLCRenderer() :
  AbstractRenderer()
, glcLight()
, glcViewport()
, glcMoverController()
, glc3DWidgetManager(&glcViewport)
, renderingFlag(glc::ShadingFlag)
, shaderID(0)
{
	QObject::connect(&glcViewport, &GLC_Viewport::updateOpenGL,
		this, &AbstractRenderer::updateRenderer);


	//--------------------------------------------------------------------------
	// GLC settings
	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0); // Red colour
	glcMoverController = GLC_Factory::instance()->createDefaultMoverController(
		repColor, &glcViewport);
	QObject::connect(
		&glcMoverController, &GLC_MoverController::repaintNeeded,
		this, &AbstractRenderer::updateRenderer);

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

	QObject::connect(&glcMoverController, &GLC_MoverController::repaintNeeded,
		this, &AbstractRenderer::broadcastCameraChange);
}

GLCRenderer::~GLCRenderer()
{
	

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


CameraSettings GLCRenderer::getCurrentCamera()
{
	return convertToCameraSettings(glcViewport.cameraHandle());
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

void GLCRenderer::loadModel(repo::core::model::RepoScene *scene)
{
	//We have a scene, fire up the GLC worker to get a GLC World representation
	//--------------------------------------------------------------------------
	// Establish and connect the new worker.
	repo::worker::GLCExportWorker* worker =
		new repo::worker::GLCExportWorker(scene);
	connect(worker, &repo::worker::GLCExportWorker::finished,
		this, &GLCRenderer::setGLCWorld);
	connect(worker, &repo::worker::GLCExportWorker::progress, this, &GLCRenderer::workerProgress);

	QObject::connect(
		this, &AbstractRenderer::killWorker,
		worker, &repo::worker::GLCExportWorker::cancel, Qt::DirectConnection);

	//--------------------------------------------------------------------------
	// Fire up the asynchronous calculation.
	QThreadPool::globalInstance()->start(worker);
}

bool GLCRenderer::move(const int &x, const int &y)
{

	return 	glcMoverController.hasActiveMover() &&
		glcMoverController.move(GLC_UserInput(x, y));
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

void GLCRenderer::setGLCWorld(GLC_World &world)
{

	repoLog("Setting GLC World...");
	repoLog("\tGLC World empty: " + std::to_string(world.isEmpty()));
	repoLog("\tGLC World size: " + std::to_string(world.size()));
	repoLog("\tGLC World #vertex: " + std::to_string(world.numberOfVertex()));
	this->glcWorld = world;
	this->glcWorld.collection()->setLodUsage(true, &glcViewport);
	this->glcWorld.collection()->setVboUsage(true);

	this->glcWorld.collection()->setSpacePartitionningUsage(true);
	GLC_Octree* octree = new GLC_Octree(this->glcWorld.collection());
	octree->updateSpacePartitioning();
	this->glcWorld.collection()->bindSpacePartitioning(octree);
	this->glcWorld.collection()->updateSpacePartitionning();
	this->glcWorld.collection()->updateInstanceViewableState(glcViewport.frustum());

	glcViewport.setDistMinAndMax(this->glcWorld.boundingBox());
	setCamera(CameraView::ISO);
	//extractMeshes(this->glcWorld.rootOccurrence());
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

		//    painter->setPen(Qt::red);
		//    painter->drawText(QPointF(1.0f, 0.0f, 0.0f), "x");
		//    painter->setPen(Qt::darkGreen);
		//    painter->drawText(QPointF(0.0f, 1.0f, 0.0f), "y");
		//    painter->setPen(Qt::blue);
		//    painter->drawText(QPointF(0.0f, 0.0f, 1.0f), "z");

		glcUICollection.render(0, glc::ShadingFlag);

		//--------------------------------------------------------------------------
		// Restore 3D state
		glPopAttrib();
		glPopMatrix(); // restore model-view matrix

		//--------------------------------------------------------------------------
		// Display stats
		painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

		painter->setPen(Qt::gray);
		QString selectionName;
		if (glcWorld.selectionSize() > 0)
			selectionName = glcWorld.selectedOccurrenceList().first()->name();

		painter->drawText(9, 14, QString() +
			QChar(0x25B2) + " " +
			locale.toString((qulonglong)GLC_RenderStatistics::triangleCount()) +
			tr(" in ") +
			locale.toString((uint)GLC_RenderStatistics::bodyCount()) +
			tr(" objects"));
		painter->drawText(screenWidth - 50, 14, fpsCounter.getFPSString());

		//--------------------------------------------------------------------------
		// Display selection
		if (glcWorld.selectionSize() > 0)
			painter->drawText(9, screenHeight - 9, tr("Selected") + ": " + selectionName);


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
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

		if (painter)
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

void GLCRenderer::resizeWindow(const int &width, const int &height)
{
	glcViewport.setWinGLSize(width, height); // Compute window aspect ratio
}

void GLCRenderer::selectComponent(const int &x, const int &y, bool multiSelection)
{
	const bool spacePartitioningIsUsed = glcWorld.collection()->spacePartitioningIsUsed();
	if (spacePartitioningIsUsed)
	{
		GLC_Frustum selectionFrustum(glcViewport.selectionFrustum(x, y));
		glcWorld.collection()->updateInstanceViewableState(selectionFrustum);
		glcWorld.collection()->setSpacePartitionningUsage(false);
		glcWorld.collection()->updateInstanceViewableState(glcViewport.frustum());
		glcWorld.collection()->setSpacePartitionningUsage(true);
	}

	GLC_uint selectionID = glcViewport.renderAndSelect(x, y);

	if (glcWorld.containsOccurrence(selectionID))
	{
		if ((!glcWorld.isSelected(selectionID))
			&& (glcWorld.selectionSize() > 0)
			&& (!multiSelection))
		{
			glcWorld.unselectAll();
			//emit selectionChanged(this, getSelectionList());
		}
		if (!glcWorld.isSelected(selectionID))
		{
			glcWorld.select(selectionID);
			//emit selectionChanged(this, getSelectionList());
		}
		else if (glcWorld.isSelected(selectionID) && multiSelection)
		{
			glcWorld.unselect(selectionID);
			//emit selectionChanged(this, getSelectionList());
		}
		else
		{
			glcWorld.unselectAll();
			glcWorld.select(selectionID);

			//emit selectionChanged(this, getSelectionList());
		}
	}
	else if (glcWorld.selectionSize() && (!multiSelection))
	{		
		// if a geometry is selected, unselect it
		glcWorld.unselectAll();
		//emit selectionChanged(this, getSelectionList());
	}
	else
	{
		repoLogError("Failed to pin point object for selection");
	}
}

void GLCRenderer::setActivationFlag(const bool &flag)
{
	GLC_RenderStatistics::setActivationFlag(true);
}

void GLCRenderer::setAndInitShaders(QFile &vertexFile, QFile &fragmentFile, QOpenGLContext *context)
{
	if (GLC_State::glslUsed()) // && !GLC_State::selectionShaderUsed())
	{
		GLC_State::setSelectionShaderUsage(true);


		GLC_SelectionMaterial::setShaders(
			vertexFile,
			fragmentFile,
            context);

        GLC_SelectionMaterial::initShader(context);
	}
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

	emit cameraChangedSignal(camera);
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

	emit cameraChangedSignal(getCurrentCamera());
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

void GLCRenderer::zoom(const float &zoom)
{
	glcViewport.cameraHandle()->zoom(zoom);
	emit cameraChangedSignal(getCurrentCamera());
}