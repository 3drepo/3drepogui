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

#ifndef REPO_OCULUS_H
#define REPO_OCULUS_H

//------------------------------------------------------------------------------
// STL
#include <iostream>

//------------------------------------------------------------------------------
// Core
#include <RepoGraphScene>

//------------------------------------------------------------------------------
// Qt
#include <QGLWidget>
#include <QGLFormat>
#include <QOpenGLFramebufferObject>

//------------------------------------------------------------------------------
// GLC Lib
#include <GLC_Light>
#include <GLC_Viewport>
#include <GLC_Exception>
#include <GLC_Factory>
#include <GLC_MoverController>
#include <GLC_Point3d>
#include <GLC_Vector3d>

//------------------------------------------------------------------------------
// Oculus
#include "OVR_CAPI.h"
#include "OVR.h"
#include "OVR_CAPI_GL.h"

namespace repo {
namespace gui {

class RepoOculus : public QGLWidget
{
    Q_OBJECT

public :

    RepoOculus(QWidget *parent = 0,
               const QGLFormat &format = QGLFormat::defaultFormat(),
               const QString &windowTitle = "");

    ~RepoOculus();

    void setGLCWorld(GLC_World);

protected :

    void initializeGL();

    void initializeOVR();

    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    void paintGL();

    void paintGLC(int eye);

    void resizeGL(int w, int h);

    void wheelEvent(QWheelEvent * e);






private :


    //ovrEyeDesc eyes[2]; // eyeRenderDesc


    ovrEyeRenderDesc   eyeRenderDesc[2];
    ovrRecti           eyeRenderViewport[2];


    //! Main light of the scene.
    GLC_Light glcLight;

    //! The viewport, in GLC lib attributed as glView.
    GLC_Viewport glcViewport;

    //! 3D world, the main scene to render.
    GLC_World glcWorld;

    //! The navigation controller of the scene (arc ball, fly etc).
    GLC_MoverController glcMoverController;


    ovrHmd hmd;


    ovrGLTexture eyeTextureGL[2];
    ovrSizei renderTargetSize[2];


    QGLPixelBuffer *pbuffer[2];


    GLuint texId[2];

    QOpenGLFramebufferObject *fbo;
   // QGLFramebufferObject *fbo;

    bool isRenderOVR;

    ovrPosef eyeRenderPose[2];
    ovrTrackingState hmdState;
}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_OCULUS_H
