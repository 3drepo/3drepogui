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


repo::gui::RepoOculus::RepoOculus(QWidget *parent, const QString &windowTitle)
    : QGLWidget(parent)
{
   // setAutoBufferSwap(false);
    resizeGL(400,400);


    init();
}

repo::gui::RepoOculus::~RepoOculus()
{
    ovrHmd_Destroy(hmd);
    ovr_Shutdown();

}

void repo::gui::RepoOculus::init()
{
    ovr_Initialize();
    std::cout << "Detected: " << ovrHmd_Detect() << std::endl;


    hmd = ovrHmd_Create(0);


    if (!hmd)
    {
        std::cerr << "Oculus Rift not detected." << std::endl;
    }
    else if (hmd->ProductName[0] == '\0')
        std::cerr << "Rift detected, display not enabled." << std::endl;







    if (hmd)
    {

        std::cout << "Vendor ID: " << hmd->VendorId << std::endl;

        std::cout << "Product name: " << hmd->ProductName[0] << std::endl;
        // Get more details about the HMD.
        ovrSizei resolution = hmd->Resolution;
        std::cout << "HMD resolution: " << resolution.w << " x " << resolution.h << std::endl;
    }
    else
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);



    // Start the sensor which provides the Rift’s pose and motion.
    ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
        ovrTrackingCap_MagYawCorrection |
        ovrTrackingCap_Position, 0);


    // Do something with the HMD.

}
