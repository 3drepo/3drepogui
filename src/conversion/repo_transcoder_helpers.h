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

#ifndef REPO_TRANSCODER_HELPERS_H
#define REPO_TRANSCODER_HELPERS_H

#include <string>
//------------------------------------------------------------------------------
#include <assimp/scene.h> 
//------------------------------------------------------------------------------
#include <QObject>
#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QHash>
#include <QColor>
//------------------------------------------------------------------------------
#include <GLC_Material>
#include <GLC_World>
#include "glc_config.h"
#include "geometry/glc_mesh.h"
#include "sceneGraph/glc_structoccurence.h"
#include "maths/glc_geomtools.h"
//------------------------------------------------------------------------------
namespace repo {
namespace gui {

//--------------------------------------------------------------------------
//
//  Helpers
//
//--------------------------------------------------------------------------

/*!
 * Enum indicating which coordinates to extract from Assimp aiVector3D
 * Texture coordinates for example can represent U, UV or UVW values
 * hence the need to distinguish.
 */
typedef enum { X = 1, XY  = 2, XYZ = 3 } RepoOrdinates;

/*! 
 * Returns a GLC Material given an Assimp material and a fileName map of
 * textures.
 */
GLC_Material * toGLCMaterial(const aiMaterial *, const QHash<QString, GLC_Texture>&);

//! Returns a GLC 3DRep given an Assimp mesh and a vector of GLC Materials.
GLC_3DRep* toGLCMesh(const aiMesh *, const QVector<GLC_Material*> &,
                            const std::string &namePrefix);

//! Returns a GLC Mesh given an Assimp camera.
GLC_3DRep* toGLCCamera(const aiCamera *);

//! Returns QColor out of Assimp's color.
QColor toQColor(const aiColor3D &, float scale = 1.0);

//! Returns QColor out of Assimp's color.
QColor toQColor(const aiColor4D &);

//! Returns a vector out of Assimp's vertices/normals/textures.
QVector<GLfloat> toGLCVector(
        const aiVector3D * vertexArray,
        unsigned int arraySize,
        RepoOrdinates);

//! Returns a vector or r, g, b, a triplets out of Assimp colours.
QVector<GLfloat> toGLCVector(
        const aiColor4t<float> * colorArray,
        unsigned int arraySize);

//! Returns a vector of vertex indices out of Assimp faces array.
QList<GLuint> toGLCList(
        const QList<GLfloat> & verticesList,
        const aiFace * facesAarray,
        unsigned int size);

//! Returns GLC matrix out of Assimp matrix.
GLC_Matrix4x4 toGLCMatrix(const aiMatrix4x4 &);

//! Returns GLC point out of Assimp vector3D.
GLC_Point3d toGLCPoint(const aiVector3D &);

} // end namespace gui
} // end namespace repo

#endif // end REPO_TRANSCODER_HELPERS_H
