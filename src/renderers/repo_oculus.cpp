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

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <GLC_ContextManager>

#include <cmath>

#if defined(Q_OS_LINUX)
	#include <X11/X.h>
	#include <QX11Info>
#endif

#include <cstdio>

#include "../primitives/repo_fontawesome.h"

template <typename T>
void printOvrMatrix(T *mat)
{
	for(int r = 0; r < 4; r++)
	{
		for(int c= 0; c < 4; c++)
		{
			printf("%f ", (float)(mat[r + 4 * c]));
		}
		printf("\n");
	}
	printf("\n");
}

repo::gui::RepoOculus::RepoOculus(QWidget *parent, const QGLFormat &format, const QString &windowTitle)
    : QGLWidget(new GLC_Context(format), parent)
    , glcLight()
    , glcViewport()
    , glcMoverController()
    //, texId(-1)
    //, fbo(0)
{
	texId[0] = 0;
	texId[1] = 0;

    isRenderOVR = true;

    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setWindowIcon(getIcon());
    setMouseTracking(true);
    setWindowTitle(windowTitle);

	//--------------------------------------------------------------------------
    // Oculus settings
    initializeOVR();

	//--------------------------------------------------------------------------
    // GLC settings
    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0); // Red colour
    glcViewport.setBackgroundColor(Qt::white);
    glcLight.setPosition(1.0, 1.0, 1.0);

	glcViewport.setViewAngle(eyeFov[0]);
	glcViewport.forceAspectRatio(aspectRatio);

    glcMoverController = GLC_Factory::instance()->createDefaultMoverController(
        repColor, &glcViewport);


    // Connect slots
    connect(&glcViewport, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
    //connect(&glcMoverController, SIGNAL(repaintNeeded()), this, SLOT(updateGL()));

    glcViewport.setBackgroundColor(Qt::white);
    glcLight.setPosition(1.0, 1.0, 1.0);

    setAutoBufferSwap(false);
   // resizeGL(400,400);

    std::cerr << "Double buffering on: " << (doubleBuffer() ? "true" : "false") << std::endl;
}

repo::gui::RepoOculus::~RepoOculus()
{
    ovrHmd_Destroy(hmd);
    ovr_Shutdown();

    glcWorld.clear();
    delete fbo;
}

QGLFormat repo::gui::RepoOculus::singleBufferFormat()
{
    QGLFormat format;
    format.setDoubleBuffer(false);
    return format;
}

QIcon repo::gui::RepoOculus::getIcon()
{
    return RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_eye);
}

//------------------------------------------------------------------------------
//
// Slots
//
//------------------------------------------------------------------------------
void repo::gui::RepoOculus::setCamera(const GLC_Camera& camera)
{
    glcViewport.cameraHandle()->setCam(camera);
    updateGL();
}

//------------------------------------------------------------------------------
//
// Protected
//
//------------------------------------------------------------------------------

void repo::gui::RepoOculus::initializeGL()
{
    glc::Y_AXIS;
    glcViewport.cameraHandle()->setDefaultUpVector(glc::Y_AXIS);
	//glc::Y_AXIS);
    glcViewport.initGl();

	// Enable VBOs and other settings.
	GLC_State::setVboUsage(true);
	GLC_State::setDefaultOctreeDepth(10);
	GLC_State::setPixelCullingUsage(true);
	GLC_State::setFrustumCullingUsage(true);
	GLC_State::setSpacePartionningUsage(true);
	GLC_State::setCacheUsage(true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

/*
    //--------------------------------------------------------------------------
    // Prepare OpenGL frame buffer objects for texture rendering
    // See Oculus SDK page 29
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setMipmap(false);
    format.setSamples(0);
    format.setInternalTextureFormat(GL_RGBA);
    fbo = new QOpenGLFramebufferObject(renderTargetSize.w , renderTargetSize.h, format);
*/


    //--------------------------------------------------------------------------
    // Test with pixel buffer
    QGLFormat pbufferFormat;
    pbufferFormat.setSampleBuffers(false);
    pbuffer[0] = new QGLPixelBuffer(QSize(renderTargetSize[0].w, renderTargetSize[0].h), pbufferFormat, this);
	pbuffer[1] = new QGLPixelBuffer(QSize(renderTargetSize[1].w, renderTargetSize[1].h), pbufferFormat, this);

    pbuffer[0]->makeCurrent();
	texId[0] = pbuffer[0]->generateDynamicTexture();

    pbuffer[1]->makeCurrent();
	texId[1] = pbuffer[1]->generateDynamicTexture();

	makeCurrent();

    eyeTextureGL[0].OGL.TexId = texId[0]; //fbo->texture();
    eyeTextureGL[1].OGL.TexId = texId[1]; //fbo->texture();

    // The actual RT size may be different due to HW limits.
    //renderTargetSize.w = fbo->size().width();
    //renderTargetSize.h = fbo->size().height();
    //--------------------------------------------------------------------------

	renderTargetSize[0].w = pbuffer[0]->size().width();
	renderTargetSize[0].h = pbuffer[0]->size().height();
	renderTargetSize[1].w = pbuffer[1]->size().width();
	renderTargetSize[1].h = pbuffer[1]->size().height();


     const char* version = (const char*)glGetString(GL_VERSION);
     std::cout << "GL_VERSION: " << version << std::endl;


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
    }

    if (hmd)
    {
        std::cout << "Vendor ID: " << hmd->VendorId << std::endl;
        std::cout << "Product name: " << hmd->ProductName[0] << std::endl;
        ovrSizei resolution = hmd->Resolution;
        std::cout << "HMD resolution: " << resolution.w << " x " << resolution.h << std::endl;
    }


	ovrFovPort fovLeft  = hmd->DefaultEyeFov[ovrEye_Left];
	ovrFovPort fovRight = hmd->DefaultEyeFov[ovrEye_Right];

	ovrFovPort fovMax;
	fovMax.UpTan = qMax(fovLeft.UpTan, fovRight.UpTan);
	fovMax.DownTan = qMax(fovLeft.DownTan, fovRight.DownTan);
	fovMax.LeftTan = qMax(fovLeft.LeftTan, fovRight.LeftTan);
	fovMax.RightTan = qMax(fovLeft.RightTan, fovRight.RightTan);

	float combinedTanHalfFovHorizontal = qMax(fovMax.LeftTan, fovMax.RightTan);
	float combinedTanHalfFovVertical = qMax ( fovMax.UpTan, fovMax.DownTan );

	ovrFovPort fovBoth;
	fovBoth.LeftTan = fovBoth.RightTan = combinedTanHalfFovHorizontal;
	fovBoth.UpTan = fovBoth.DownTan = combinedTanHalfFovVertical;

    //--------------------------------------------------------------------------
    // Configure Stereo settings.
    ovrSizei recommendedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, fovBoth, 1.0f);
    ovrSizei recommendedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, fovBoth, 1.0f);

    renderTargetSize[0].w = recommendedTex0Size.w;
    renderTargetSize[0].h = qMax(recommendedTex0Size.h, recommendedTex1Size.h);

	renderTargetSize[1].w = recommendedTex1Size.w;
	renderTargetSize[1].h = renderTargetSize[0].h;


	float horizontalFullFovInDegrees = (2.0f * atanf ( combinedTanHalfFovHorizontal)) * (180.0 / 3.14159265);

	eyeFov[0] = horizontalFullFovInDegrees;
	eyeFov[1] = horizontalFullFovInDegrees;

	aspectRatio = combinedTanHalfFovHorizontal / combinedTanHalfFovVertical;

    //--------------------------------------------------------------------------
    // Initialize eye rendering information.
    // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
    ovrFovPort eyesFov[2] = { fovBoth, fovBoth } ;

    //--------------------------------------------------------------------------
    // Rendering parameters
    // SDK Docs page 31

    // Left eye
    eyeTextureGL[0].OGL.Header.API = ovrRenderAPI_OpenGL;
    eyeTextureGL[0].OGL.Header.TextureSize = renderTargetSize[0];
    eyeTextureGL[0].OGL.Header.RenderViewport.Pos = OVR::Vector2i(0,0);
    eyeTextureGL[0].OGL.Header.RenderViewport.Size = OVR::Sizei(renderTargetSize[0].w, renderTargetSize[0].h);

    // Right eye
    eyeTextureGL[1].OGL.Header.API = ovrRenderAPI_OpenGL;
    eyeTextureGL[1].OGL.Header.TextureSize = renderTargetSize[1];
    eyeTextureGL[1].OGL.Header.RenderViewport.Pos = OVR::Vector2i(0,0); //OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
    eyeTextureGL[1].OGL.Header.RenderViewport.Size = OVR::Sizei(renderTargetSize[1].w, renderTargetSize[1].h);
	//eyeTextureGL[1].OGL.TexId = fbo->texture();


    //--------------------------------------------------------------------------
    // Configure OVR for OpenGL
    const int backBufferMultisample = 0;
    ovrGLConfig cfg;
    cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
    cfg.OGL.Header.RTSize = OVR::Sizei(hmd->Resolution.w, hmd->Resolution.h);
    cfg.OGL.Header.Multisample = backBufferMultisample;
	#if defined(Q_OS_LINUX)
		cfg.OGL.Disp = QX11Info::display();
		cfg.OGL.Win  = (Window)(winId());
	#elif defined(Q_OS_WIN)
		cfg.OGL.Window = reinterpret_cast<HWND>(winId());
		cfg.OGL.DC = wglGetCurrentDC();
	#endif

//	void *winHandle = QGuiApplication::platformNativeInterface()->nativeResourceForWindow(QByteArrayLiteral("handle"), w);
//	#if defined(Q_OS_LINUX)
//        cfg.OGL.Win = (Window)(winId());
//    #elif defined(Q_OS_WIN)
//        cfg.OGL.Window = reinterpret_cast<HWND>(winId());
//    #endif



//    cfg.OGL.WglContext = wglGetCurrentContext();
//    cfg.OGL.GdiDc = wglGetCurrentDC();

	#if defined(Q_OS_WIN)
	    std::cout << "Window: " << cfg.OGL.Window << std::endl;
		std::cout << "DC: " << cfg.OGL.DC << std::endl;
	#elif defined(Q_OS_LINUX)
		cfg.OGL.Disp = QX11Info::display();
	#endif







    //--------------------------------------------------------------------------
    // Configure Oculus Rendering
    if(isRenderOVR && !ovrHmd_ConfigureRendering(
                hmd,
                &cfg.Config,
                ovrDistortionCap_Chromatic, //ovrDistortionCap_NoRestore, // | ovrDistortionCap_TimeWarp,
                eyesFov,
                eyeRenderDesc))
    {
        std::cerr << "Failed to configure rendering." << std::endl;
    }




        //--------------------------------------------------------------------------
        //
        #if defined(Q_OS_WIN)
            // Direct rendering from a window handle to the Hmd.
            // Not required if ovrHmdCap_ExtendDesktop flag is set.

            if (!(hmd->HmdCaps & ovrHmdCap_ExtendDesktop))
            {
                    ovrHmd_AttachToWindow(hmd, cfg.OGL.Window, NULL, NULL);
                    std::cerr << "window attached" << std::endl;
            }
        #endif


    //--------------------------------------------------------------------------
    // Start the sensor which provides the Rift’s pose and motion.
    ovrHmd_ConfigureTracking(hmd,
                             ovrTrackingCap_Orientation |
                             ovrTrackingCap_MagYawCorrection |
                             ovrTrackingCap_Position, 0);



    std::cerr << "GL Error: " << glGetError()  << std::endl;

    //--------------------------------------------------------------------------
    // TESTER Texture

	/*
	std::cout << " Loading image ... " << std::endl;
    QImage img("/home/tscully/3drepo/3drepogui/3drepo-bg.bmp");
      if(img.isNull()){
        qDebug() << "Failed loading image";
      }


      glEnable(GL_TEXTURE_2D);
    //  texId = QGLWidget::context()->bindTexture(img, GL_TEXTURE_2D, GL_RGBA);

      std::cerr << "Test texture ID: " << texId << std::endl;

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      Q_ASSERT(!glGetError());
	*/
}

void repo::gui::RepoOculus::keyPressEvent(QKeyEvent *e)
{
    ovrHmd_DismissHSWDisplay(hmd);
    switch(e->key())
    {
        case Qt::Key_R :
        {
			std::cout << " You pressed the R key " << std::endl;
            glcViewport.cameraHandle()->setIsoView();
            glcViewport.reframe(glcWorld.boundingBox());

			ovrVector3f WorldEyePos = eyeRenderPose[0].Position;
			GLC_Matrix4x4 trans(WorldEyePos.x,WorldEyePos.y,WorldEyePos.z);
			GLC_Matrix4x4 orient(&(OVR::Matrix4f(eyeRenderPose[0].Orientation).M[0][0]));

			GLC_Matrix4x4 newMV = (orient * trans).inverted();
			glcViewport.cameraHandle()->move(newMV);

			//updateGL();
        }
        break;
        case Qt::Key_F11 :
        case Qt::Key_Escape :
            this->deleteLater();
            break;
    }
}



void repo::gui::RepoOculus::mousePressEvent(QMouseEvent *e)
{
    ovrHmd_DismissHSWDisplay(hmd);
    switch (e->button())
    {
        case (Qt::RightButton):
            this->setCursor(Qt::ClosedHandCursor);
            glcMoverController.setActiveMover(
                GLC_MoverController::TrackBall,
                GLC_UserInput(e->x(), e->y()));
            //updateGL();
            break;
        case (Qt::LeftButton):
            this->setCursor(Qt::SizeAllCursor);
            glcMoverController.setActiveMover(
                GLC_MoverController::Pan,
                GLC_UserInput(e->x(), e->y()));
            //updateGL();
            break;
        case (Qt::MidButton):
            this->setCursor(Qt::CrossCursor);
            glcMoverController.setActiveMover(
                GLC_MoverController::Fly,
                GLC_UserInput(e->x(), e->y()));
            //updateGL();
            break;
    }
    QGLWidget::mousePressEvent(e);
}

void repo::gui::RepoOculus::mouseMoveEvent(QMouseEvent *e)
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

void repo::gui::RepoOculus::wheelEvent(QWheelEvent *e)
{
    glcViewport.cameraHandle()->zoom(e->delta() > 0 ? 1.2 : 1/1.2);
    updateGL();
    QGLWidget::wheelEvent(e);
}


void repo::gui::RepoOculus::paintGL()
{
	const int ms(1000/75);
	QTimer::singleShot(ms, this, SLOT(update()));

    try
    {
        ovrTexture eyeTexture[2];
        ovrPosef headPose[2];

        //----------------------------------------------------------------------
        // See offscreen rendering:
        // http://qt-project.org/doc/qt-5/qoffscreensurface.html
        // http://qt-project.org/doc/qt-5/qtquick-scenegraph-textureinthread-example.html
        //----------------------------------------------------------------------


        // TODO: TRY with pixel buffer instead of FBO:
        // http://qt-project.org/doc/qt-4.8/opengl-pbuffers-glwidget-cpp.html
        // QGLPixelBuffer: http://qt-project.org/doc/qt-5/qglpixelbuffer.html


        ovrHmd_BeginFrame(hmd, 0);


        ovrVector3f hmdToEyeViewOffset[2] = {
            eyeRenderDesc[0].HmdToEyeViewOffset,
            eyeRenderDesc[1].HmdToEyeViewOffset };
        ovrHmd_GetEyePoses(hmd, 0,
                           hmdToEyeViewOffset, eyeRenderPose, &hmdState);

        //----------------------------------------------------------------------
        // Render to off-screen buffer.
        glEnable(GL_TEXTURE_2D);

        // push the projection matrix and the entire GL state before
        // doing any rendering into our framebuffer object
//        glPushAttrib(GL_ALL_ATTRIB_BITS);
//        glPushMatrix();



//        makeCurrent();
//        fbo->bind();

        for (int eyeIndex = 0; eyeIndex < ovrEye_Count; ++eyeIndex)
        {

            //makeCurrent();

            ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];


			pbuffer[eye]->makeCurrent();

            #ifdef defined(Q_OS_WIN)
                pbuffer[eye]->bindToDynamicTexture(texId[eye]);
            #endif

			//resizeGL(fbo->width(), fbo->height());

			//if (eye == 1)
			//updateCam(eyeRenderDesc[eye], glcViewport.cameraHandle());

			// Render scene
			paintGLC(eye);

			//QImage tester = pbuffer->toImage();
			//tester.save("pbuffer.jpg");




            // See http://qt-project.org/doc/qt-5/qglpixelbuffer.html#bindToDynamicTexture
            // for the explanation of linux vs windows directives.
            #if defined(Q_OS_LINUX)
                pbuffer[eye]->updateDynamicTexture(eyeTextureGL[eye].OGL.TexId);
            #elif defined(Q_OS_WIN)
                pbuffer[eye]->releaseFromDynamicTexture();
            #endif

			//fbo->release();
			makeCurrent();
			resizeGL(width(), height());


		}


//        glPopAttrib();
//        glPopMatrix();


        eyeTexture[0] = eyeTextureGL[0].Texture;
        eyeTexture[1] = eyeTextureGL[1].Texture;


        //----------------------------------------------------------------------
//        glDisable(GL_CULL_FACE);
//        glDisable(GL_DEPTH_TEST);



        if (isRenderOVR)
            ovrHmd_EndFrame(hmd, headPose, eyeTexture);

//        glEnable(GL_CULL_FACE);
//        glEnable(GL_DEPTH_TEST);
//        glClearDepth(1.0);



    }
    catch (GLC_Exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

  //  GLC_Context::current()->doneCurrent();
  //  doneCurrent();



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

    swapBuffers();
    doneCurrent();
    */
}

void repo::gui::RepoOculus::paintGLC(int eye)
{
    glClearColor(1,1,1,1);
    glEnable(GL_LIGHTING);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

    glcViewport.setDistMinAndMax(glcWorld.boundingBox());

    glcWorld.collection()->updateInstanceViewableState();
    GLC_Context::current()->glcLoadIdentity();
    //glLoadIdentity();

    glcLight.glExecute();

	//printOvrMatrix(glcViewport.cameraHandle()->modelViewMatrix().getData());
    //updateCam(eyeRenderDesc[eye], glcViewport.cameraHandle());
   //ovrVector3f WorldEyePos = eyeRenderDesc[eye].HmdToEyeViewOffset;

	//ovrVector3f ViewAdjust = eyeRenderDesc[eye].ViewAdjust;

	//glcViewport.cameraHandle()->translate(GLC_Vector3d(ViewAdjust.x, ViewAdjust.y, ViewAdjust.z));

	ovrVector3f WorldEyePos = eyeRenderPose[eye].Position;

	GLC_Matrix4x4 mv = glcViewport.cameraHandle()->modelViewMatrix();
	GLC_Matrix4x4 trans(WorldEyePos.x,WorldEyePos.y,WorldEyePos.z);
	GLC_Matrix4x4 orient(&(OVR::Matrix4f(eyeRenderPose[eye].Orientation).M[0][0]));

	GLC_Matrix4x4 newMV = orient * trans * mv;

	//printf("%f %f %f\n", WorldEyePos.x, WorldEyePos.y, WorldEyePos.z);
/*
	glcViewport.cameraHandle()->translate(GLC_Vector3d(WorldEyePos.x, WorldEyePos.y, WorldEyePos.z));
	GLC_Matrix4x4 orient(&(OVR::Matrix4f(eyeRenderPose[eye].Orientation).M[0][0]));

	//glMultMatrixf(&(OVR::Matrix4f(eyeRenderPose[eye].Orientation).M[0][0]));

	glcViewport.cameraHandle()->move(orient.inverted());
	glcViewport.glExecuteCam();
*/
	GLC_Context::current()->glcMultMatrix(newMV);
	//glMultMatrix4f(&(eyeRenderPose[eye].translation[0][0])));
	glcViewport.useClipPlane(true);

	glcViewport.setViewAngle(eyeFov[0]);
	//glcViewport.forceAspectRatio(aspectRatio);

	resizeGL(renderTargetSize[0].w, renderTargetSize[0].w / aspectRatio);
    glcWorld.render(0, glc::ShadingFlag);
    glcMoverController.drawActiveMoverRep();

    //QImage image = fbo->toImage();
    //image.save("test.jpg");

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
    makeCurrent();
    glcViewport.cameraHandle()->setIsoView();
    glcViewport.reframe(glcWorld.boundingBox());
    updateGL();
}
