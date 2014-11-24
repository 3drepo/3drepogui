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
// Qt
#include <QGLWidget>


//------------------------------------------------------------------------------
// Oculus
//#include "OVR_CAPI.h"
#include "OVR.h"


#include <iostream>
//------------------------------------------------------------------------------

namespace repo {
namespace gui {

class RepoOculus : public QGLWidget
{
    Q_OBJECT

public :

    RepoOculus(QWidget *parent = 0, const QString &windowTitle = "");

    ~RepoOculus();

    void init();


private :

    ovrHmd hmd;

}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_OCULUS_H
