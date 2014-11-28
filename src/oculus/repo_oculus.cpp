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

#include "repo_oculus.h"


#if defined(Q_OS_LINUX)
	#include <QX11Info>
    #include <qpa/qplatformnativeinterface.h>
#endif

#include <cstdio>

#include "../primitives/repo_fontawesome.h"

void printOvrMatrix(const ovrMatrix4f &mat)
{
	for(int r = 0; r < 4; r++)
	{
		for(int c= 0; c < 4; c++)
		{
			printf("%f ", mat.M[r][c]);
		}
		printf("\n");
	}
	printf("\n");
}

void updateCam(const ovrEyeRenderDesc &eye, GLC_Camera *cam)
{
	OVR::Matrix4f proj = ovrMatrix4f_Projection(eye.Fov, 0.01f, 10000.0f, true);

	GLC_Point3d eyePos(eye.HmdToEyeViewOffset.x,
						eye.HmdToEyeViewOffset.y,
						eye.HmdToEyeViewOffset.z);

	GLC_Vector3d up(proj.M[1][0], proj.M[1][1], -proj.M[1][2]);
	GLC_Vector3d right(proj.M[0][0], proj.M[0][1], proj.M[0][2]);
	GLC_Point3d forward = up^right;

	cam->setCam(eyePos, forward, up);
}

repo::gui::RepoOculus::RepoOculus(QWidget *parent, const QString &windowTitle)
    : QGLWidget(new GLC_Context(QGLFormat::defaultFormat()), parent)
    , glcLight()
    , glcViewport()
    , glcMoverController()
    , textID(-1)
{

    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setWindowIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_eye));
    setMouseTracking(true);
    setWindowTitle(windowTitle);

	fbos[0] = 0;
	fbos[1] = 0;



    makeCurrent();

    GLC_Context::current();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    fbo = new QOpenGLFramebufferObject(1182, 1461, format);

    //--------------------------------------------------------------------------
    // GLC settings
    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0); // Red colour
    glcMoverController = GLC_Factory::instance()->createDefaultMoverController(
        repColor, &glcViewport);


    // Connect slots
    connect(&glcViewport, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
    connect(&glcMoverController, SIGNAL(repaintNeeded()), this, SLOT(updateGL()));

    glcViewport.setBackgroundColor(Qt::white);
    glcLight.setPosition(1.0, 1.0, 1.0);

    //--------------------------------------------------------------------------
    // Oculus settings
    //initializeOVR();





    setAutoBufferSwap(false);
   // resizeGL(400,400);
}

repo::gui::RepoOculus::~RepoOculus()
{
    ovrHmd_Destroy(hmd);
    ovr_Shutdown();

    glcWorld.clear();
}

//------------------------------------------------------------------------------
//
// Protected
//
//------------------------------------------------------------------------------

void repo::gui::RepoOculus::initializeGL()
{
    glcViewport.cameraHandle()->setDefaultUpVector(glc::Y_AXIS);
    glcViewport.initGl();
}

void repo::gui::RepoOculus::initializeOVR()
{
    // See https://forums.oculus.com/viewtopic.php?f=17&t=8305
    makeCurrent();   // Sets this window to the active OpenGL target
    ovr_Initialize();
    //int hmdCount = ovrHmd_Detect();
    //std::cout << "Number of available HMDs: " << hmdCount << std::endl;

    hmd = ovrHmd_Create(0);
    if (!hmd)
    {
        std::cerr << "Oculus Rift not detected." << std::endl;
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }
    else if (hmd->ProductName[0] == '\0')
    {
        std::cerr << "Oculus Rift detected, display not enabled." << std::endl;
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }\

    if (hmd)
    {
        std::cout << "Vendor ID: " << hmd->VendorId << std::endl;
        std::cout << "Product name: " << hmd->ProductName[0] << std::endl;
        ovrSizei resolution = hmd->Resolution;
        std::cout << "HMD resolution: " << resolution.w << " x " << resolution.h << std::endl;
    }

    // Start the sensor which provides the Rift’s pose and motion.
    ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
        ovrTrackingCap_MagYawCorrection |
        ovrTrackingCap_Position, 0);


    //--------------------------------------------------------------------------
    // Configure Stereo settings.
    ovrSizei recommendedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
    ovrSizei recommendedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);

    renderTargetSize.w = (recommendedTex0Size.w + recommendedTex1Size.w)/2;
    renderTargetSize.h = qMax(recommendedTex0Size.h, recommendedTex1Size.h);


    //--------------------------------------------------------------------------
    // Prepare OpenGL frame buffer objects for texture rendering
    // See Oculus SDK page 29
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    fbos[0] = new QOpenGLFramebufferObject(renderTargetSize.w, renderTargetSize.h, format);
    fbos[1] = new QOpenGLFramebufferObject(renderTargetSize.w, renderTargetSize.h, format);
	renderTargetSize.w = fbos[0]->width();
    renderTargetSize.h = fbos[0]->height();

    std::cout << "WH: " <<  renderTargetSize.w << " x " << renderTargetSize.h << std::endl;

    //--------------------------------------------------------------------------

	resize(renderTargetSize.w, renderTargetSize.h);

	//printf("WH: %d %d\n",width(), height());
	//printf("WHI: %d %d\n", renderTargetSize.w, renderTargetSize.h);

    //--------------------------------------------------------------------------
    // Initialize eye rendering information.
    // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
    ovrFovPort eyesFov[2] = { hmd->DefaultEyeFov[0], hmd->DefaultEyeFov[1] } ;


    //--------------------------------------------------------------------------
    // Rendering parameters
    // SDK Docs page 31

    // Left eye
    eyeTextureGL[0].OGL.Header.API = ovrRenderAPI_OpenGL;
    eyeTextureGL[0].OGL.Header.TextureSize = renderTargetSize;
    eyeTextureGL[0].OGL.Header.RenderViewport.Pos = OVR::Vector2i(0,0);
    eyeTextureGL[0].OGL.Header.RenderViewport.Size = OVR::Sizei(renderTargetSize.w, renderTargetSize.h);
   // eyeTextureGL[0].OGL.TexId = fbos[0]->texture();

    // Right eye
    eyeTextureGL[1] = eyeTextureGL[0];
    eyeTextureGL[1].OGL.Header.RenderViewport.Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
    eyeTextureGL[1].OGL.Header.RenderViewport.Size = eyeTextureGL[0].OGL.Header.RenderViewport.Size;
  //  eyeTextureGL[1].OGL.TexId = fbos[1]->texture();

	glcViewport.setWinGLSize(renderTargetSize.w, renderTargetSize.h);

	//printf("FBO %d %d\n", glcViewport.viewHSize(), glcViewport.viewHSize());

    //--------------------------------------------------------------------------
    // Configure OVR for OpenGL
    const int backBufferMultisample = 0;
    ovrGLConfig cfg;
    cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
    cfg.OGL.Header.RTSize = OVR::Sizei(hmd->Resolution.w, hmd->Resolution.h);
    cfg.OGL.Header.Multisample = backBufferMultisample;



//	void *winHandle = QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArrayLiteral("handle"), w);
	#if defined(Q_OS_LINUX)
        cfg.OGL.Win = (Window)(winId());
    #elif defined(Q_OS_WIN)
        cfg.OGL.Window = reinterpret_cast<HWND>(winId());
    #endif

	makeCurrent();

	// cfg.OGL.WglContext = wglGetCurrentContext();
	// cfg.OGL.GdiDc = wglGetCurrentDC();
	#if defined(Q_OS_WIN)
		cfg.OGL.DC = wglGetCurrentDC();
	    std::cout << "Window: " << cfg.OGL.Window << std::endl;
		std::cout << "DC: " << cfg.OGL.DC << std::endl;
	#elif defined(Q_OS_LINUX)
		cfg.OGL.Disp = QX11Info::display();
	#endif

    if(!ovrHmd_ConfigureRendering(
                hmd,
                &cfg.Config,
                ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
                eyesFov,
                eyeRenderDesc))
    {
        std::cerr << "Failed to configure rendering." << std::endl;
    }


	#if defined(Q_OS_WIN)
		// Direct rendering from a window handle to the Hmd.
		// Not required if ovrHmdCap_ExtendDesktop flag is set.
		if (false)
			ovrHmd_AttachToWindow(hmd, cfg.OGL.Window, NULL, NULL);
	#endif


    //--------------------------------------------------------------------------
    // TESTER Texture

    QImage img(":/images/3drepo-bg.png");
      if(img.isNull()){
        qDebug() << "Failed loading image";
      }


      glEnable(GL_TEXTURE_2D);
      eyeTextureGL[0].OGL.TexId = QGLWidget::context()->bindTexture(img, GL_TEXTURE_2D, GL_RGBA);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      Q_ASSERT(!glGetError());

}


void repo::gui::RepoOculus::mousePressEvent(QMouseEvent *e)
{
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
    QGLWidget::mousePressEvent(e);
}
void repo::gui::RepoOculus::mouseMoveEvent(QMouseEvent * e)
{
    if (glcMoverController.hasActiveMover() &&
        glcMoverController.move(GLC_UserInput(e->x(), e->y())))
    {
        updateGL();
    }
    QGLWidget::mouseMoveEvent(e);
}
void repo::gui::RepoOculus::mouseReleaseEvent(QMouseEvent *e)
{
    if (glcMoverController.hasActiveMover())
    {
        this->setCursor(Qt::ArrowCursor);
        glcMoverController.setNoMover();
        updateGL();
    }
    QGLWidget::mouseReleaseEvent(e);
}
void repo::gui::RepoOculus::wheelEvent(QWheelEvent * e)
{
    glcViewport.cameraHandle()->zoom(e->delta() > 0 ? 1.2 : 1/1.2);
    updateGL();
    QGLWidget::wheelEvent(e);
}



void repo::gui::RepoOculus::paintGL()
{
//    static ovrPosef eyeRenderPose[2];
//    static ovrTrackingState hmdState;
//    static ovrTexture eyeTexture[2];

//    /glEnable(GL_TEXTURE_2D);











    makeCurrent();
    fbo->bind();
    resizeGL(1182, 1461);

  //  updateGL();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glcViewport.setDistMinAndMax(glcWorld.boundingBox());
    glcWorld.collection()->updateInstanceViewableState();
    GLC_Context::current()->glcLoadIdentity();


    glcLight.glExecute();
    //updateCam(eyeRenderDesc[eye], glcViewport.cameraHandle());
    glcViewport.glExecuteCam();
    glcViewport.useClipPlane(true);

    glcWorld.render(0, glc::ShadingFlag);
    glcMoverController.drawActiveMoverRep();

    QImage image = fbo->toImage();
    image.save("test.jpg");

    fbo->release();
    resizeGL(size().width(), size().height());


/*
    try
    {
        makeCurrent();

//        glcViewport.setDistMinAndMax(glcWorld.boundingBox());
//        glcWorld.collection()->updateInstanceViewableState();

//        GLC_Context::current()->glcLoadIdentity();
//        glcLight.glExecute();
//        glcViewport.glExecuteCam();
//        glcViewport.useClipPlane(true);
//        glcWorld.render(0, glc::ShadingFlag);
//        glcMoverController.drawActiveMoverRep();




        //----------------------------------------------------------------------
        // Oculus SDK Docs page 32

        const int ms(1000/60);
        QTimer::singleShot(ms, this, SLOT(update()));

      //  ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrame(hmd, 0);

        ovrVector3f hmdToEyeViewOffset[2] = {
            eyeRenderDesc[0].HmdToEyeViewOffset,
            eyeRenderDesc[1].HmdToEyeViewOffset };
        ovrHmd_GetEyePoses(hmd, 0,
                           hmdToEyeViewOffset, eyeRenderPose, &hmdState);

        glEnable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ovrPosef headPose[2];
        for (int eyeIndex = 0; eyeIndex < ovrEye_Count; ++eyeIndex)
        {

            //makeCurrent();

//            ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];

//			ovrEyeRenderDesc *currEye = &eyeRenderDesc[eye];

//            OVR::Quatf orientation = OVR::Quatf(headPose[eye].Orientation);
//            OVR::Matrix4f proj = ovrMatrix4f_Projection(
//                        eyeRenderDesc[eye].Fov, 0.01f, 10000.0f, true);
//            //ovrPosef    eyePose = ovrHmd_BeginEyeRender(hmd, eye);

//			ovrVector3f WorldEyePos = eyeRenderDesc[eye].HmdToEyeViewOffset;



//			WorldEyePos.x *= -1;
//			WorldEyePos.y *= -1;
//			WorldEyePos.z *= -1;

//			printf("EYE: %f %f %f\n", WorldEyePos.x, WorldEyePos.y, WorldEyePos.z);

//            // Assign quaternion result directly to view (translation is ignored).
//            //OVR::Matrix4f view = OVR::Matrix4f(orientation.Inverted()) * OVR::Matrix4f::Translation(WorldEyePos);

//			OVR::Matrix4f view = OVR::Matrix4f::Translation(WorldEyePos);


			//printOvrMatrix(view);


//           glViewport(eyeRenderDesc[eye].DistortedViewport.Pos.x,
//                      eyeRenderDesc[eye].DistortedViewport.Pos.y,
//                      eyeRenderDesc[eye].DistortedViewport.Size.w,
//                      eyeRenderDesc[eye].DistortedViewport.Size.h);

//			printf("%d - %d %d %d %d\n", eye,
//					eyeRenderDesc[eye].DistortedViewport.Pos.x,
//                      eyeRenderDesc[eye].DistortedViewport.Pos.y,
//                      eyeRenderDesc[eye].DistortedViewport.Size.w,
//                      eyeRenderDesc[eye].DistortedViewport.Size.h);

//           glMatrixMode(GL_PROJECTION);
//           glLoadIdentity();
//           glMultMatrixf(&(proj.Transposed().M[0][0]));
//           glMatrixMode(GL_MODELVIEW);
//           glLoadIdentity();
//           glTranslatef(-WorldEyePos.x, -WorldEyePos.y, -WorldEyePos.z);
//           glMultMatrixf(&(view.Transposed().M[0][0]));






            // Create the framebuffer


            // See Oculus SDK page 29
//            QOpenGLFramebufferObjectFormat format;
//            format.setAttachment(QOpenGLFramebufferObject::Depth);
//            QOpenGLFramebufferObject fbo(renderTargetSize.w, renderTargetSize.h, format);




            //makeCurrent();
          //  fbo.bind();
          //  glcViewport.setWinGLSize(fbos[eye]->width(), fbos[eye]->height());
			//parentWidget()->resize(fbos[eye]->width(), fbos[eye]->height());
            //resizeGL(fbos[eye]->width(), fbos[eye]->height());

            //------------------------------------------------------------------
            // RENDER here
//			glViewport(currEye->DistortedViewport.Pos.x,
//				currEye->DistortedViewport.Pos.y,
//				currEye->DistortedViewport.Size.w,
//				currEye->DistortedViewport.Size.h);

            glcViewport.setDistMinAndMax(glcWorld.boundingBox());
            glcWorld.collection()->updateInstanceViewableState();
            GLC_Context::current()->glcLoadIdentity();
            glcLight.glExecute();

            //updateCam(eyeRenderDesc[eye], glcViewport.cameraHandle());

            glcViewport.glExecuteCam();
            glcViewport.useClipPlane(true);
            glcWorld.render(0, glc::ShadingFlag);
            glcMoverController.drawActiveMoverRep();

			//------------------------------------------------------------------


			//printf("FBO %d %d\n", glcViewport.viewHSize(), glcViewport.viewHSize());

//            QImage image = fbo.toImage();
//            image.save("test.png");

//            fbo.release();
            //resizeGL(size().width(), size().height());




           //ovrHmd_EndEyeRender(oculus, eye, eyePose, &eyeTextureGL[eye].Texture);
         }
        // glDisable(GL_CULL_FACE);
        // glDisable(GL_DEPTH_TEST);


       // eyeTexture[0] = eyeTextureGL[0].Texture;
       // eyeTexture[1] = eyeTextureGL[1].Texture;

       // ovrHmd_EndFrame(hmd, headPose, eyeTexture);

         //glEnable(GL_CULL_FACE);
         //glEnable(GL_DEPTH_TEST);
         //glClearDepth(1.0);
    }
    catch (GLC_Exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
*/

    swapBuffers();
    doneCurrent();
}


void repo::gui::RepoOculus::resizeGL(int w, int h)
{
    glcViewport.setWinGLSize(w, h);
  //   glViewport(0,0,w, h);
}


void repo::gui::RepoOculus::setGLCWorld(GLC_World glcWorld)
{
    this->glcWorld = glcWorld;
    glcViewport.setDistMinAndMax(this->glcWorld.boundingBox());
    glcViewport.cameraHandle()->setIsoView();
    glcViewport.reframe(glcWorld.boundingBox());
    updateGL();
}
