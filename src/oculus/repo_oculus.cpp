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

#include "../primitives/repo_fontawesome.h"

repo::gui::RepoOculus::RepoOculus(QWidget *parent, const QString &windowTitle)
    : QGLWidget(parent)
    , glcLight()
    , glcViewport()
{

    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setWindowIcon(RepoFontAwesome::getInstance().getIcon(RepoFontAwesome::fa_eye));
    setMouseTracking(true);
    setWindowTitle(windowTitle);

    setAutoBufferSwap(false);
    resizeGL(400,400);

    glcViewport.setBackgroundColor(Qt::white);
    glcLight.setPosition(1.0, 1.0, 1.0);

    initializeOVR();
    initializeTex();
}

repo::gui::RepoOculus::~RepoOculus()
{
    ovrHmd_Destroy(hmd);
    ovr_Shutdown();
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
    this->makeCurrent();   // Sets this window to the active OpenGL target
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
\

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

    renderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
    renderTargetSize.h = qMax(recommendedTex0Size.h, recommendedTex1Size.h);


    //--------------------------------------------------------------------------
    // Initialize eye rendering information.
    // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
    ovrFovPort eyesFov[2] = { hmd->DefaultEyeFov[0], hmd->DefaultEyeFov[1] } ;


    //--------------------------------------------------------------------------
    // Rendering parameters
//    eyes[0].

//    //left eye
//    eyes[0].Eye = ovrEye_Left;
//    eyes[0].Fov = hmd->DefaultEyeFov[0];
//    eyes[0].TextureSize = renderTargetSize;
//    eyes[0].RenderViewport.Pos = OVR::Vector2i(0,0);
//    eyes[0].RenderViewport.Size = OVR::Sizei(renderTargetSize.w/2, renderTargetSize.h);

//    //right eye
//    eyes[1].Eye = ovrEye_Right;
//    eyes[1].Fov = oculusInfos.DefaultEyeFov[1];
//    eyes[1].TextureSize = renderTargetSize;
//    eyes[1].RenderViewport.Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
//    eyes[1].RenderViewport.Size = eyes[0].RenderViewport.Size;




    //--------------------------------------------------------------------------
    // Configure OVR for OpenGL
    const int backBufferMultisample = 0;
    ovrGLConfig cfg;
    cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
    cfg.OGL.Header.RTSize = OVR::Sizei(hmd->Resolution.w, hmd->Resolution.h);
    cfg.OGL.Header.Multisample = backBufferMultisample;
    cfg.OGL.Window = reinterpret_cast<HWND>(winId());
    makeCurrent();



   // cfg.OGL.WglContext = wglGetCurrentContext();
   // cfg.OGL.GdiDc = wglGetCurrentDC();
    cfg.OGL.DC = wglGetCurrentDC();

    std::cout << "Window: " << cfg.OGL.Window << std::endl;
    std::cout << "DC: " << cfg.OGL.DC << std::endl;

    if(!ovrHmd_ConfigureRendering(
                hmd,
                &cfg.Config,
                ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
                eyesFov,
                eyeRenderDesc))
    {
        std::cerr << "Failed to configure rendering." << std::endl;
    }


    // Left eye
    eyeTextureGL[0].OGL.Header.API = ovrRenderAPI_OpenGL;
    eyeTextureGL[0].OGL.Header.TextureSize = renderTargetSize;
    eyeTextureGL[0].OGL.Header.RenderViewport.Pos = OVR::Vector2i(0,0);
    eyeTextureGL[0].OGL.Header.RenderViewport.Size = OVR::Sizei(renderTargetSize.w/2, renderTargetSize.h);
    eyeTextureGL[0].OGL.TexId = textID;

    // Right eye
    eyeTextureGL[1] = eyeTextureGL[0];
    eyeTextureGL[1].OGL.Header.RenderViewport.Pos = OVR::Vector2i((renderTargetSize.w + 1) / 2, 0);
    eyeTextureGL[1].OGL.Header.RenderViewport.Size = eyeTextureGL[0].OGL.Header.RenderViewport.Size;


}

void repo::gui::RepoOculus::initializeTex()
{
    QImage img(":/images/3drepo-bg.png");
    if(img.isNull())
        std::cerr << "Failed loading image" << std::endl;
    else
    {
        glEnable(GL_TEXTURE_2D);
        textID = QGLWidget::context()->bindTexture(img, GL_TEXTURE_2D, GL_RGBA);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        Q_ASSERT(!glGetError());
    }
}

void repo::gui::RepoOculus::resizeGL(int w, int h)
{

}

void repo::gui::RepoOculus::paintGL()
{
    try
    {
        //glcViewport.setDistMinAndMax(glcWorld.boundingBox());
        //glcWorld.collection()->updateInstanceViewableState();

        //----------------------------------------------------------------------
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //----------------------------------------------------------------------
        // Load identity matrix
        //GLC_Context::current()->glcLoadIdentity();
        //glEnable(GL_MULTISAMPLE);


        //----------------------------------------------------------------------
        // Define the light
        //glcLight.glExecute();

        //----------------------------------------------------------------------
        // Define view matrix
       // glcViewport.glExecuteCam();
    }
    catch (GLC_Exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    swapBuffers();
    doneCurrent();
}
