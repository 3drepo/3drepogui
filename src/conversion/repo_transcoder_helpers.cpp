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

#include "repo_transcoder_helpers.h"
#include <iostream>
#include <glc_factory.h>
#include "../primitives/repo_glccamera.h"

namespace repo {
namespace gui {

QColor toQColor(const aiColor3D &c, float scale)
{
    return QColor(
        std::min(int (c.r * scale * 255), 255),
        std::min(int (c.g * scale * 255), 255),
        std::min(int (c.b * scale * 255), 255));
}

QColor toQColor(const aiColor4D &c)
{
    return QColor(
        std::min(int (c.r * 255), 255),
        std::min(int (c.g * 255), 255),
        std::min(int (c.b * 255), 255),
        std::min(int (c.a * 255), 255));
}

QVector<GLfloat> toGLCVector(
    const aiVector3D * vertexArray,
    unsigned int arraySize,
    RepoOrdinates ordinatesType)
{
    QVector<GLfloat> glcVector(arraySize * ordinatesType); // always [x,y,z]
    for (unsigned int i = 0, j = 0; i < glcVector.size(); i += ordinatesType, ++j)
    {
        // This switch deliberately falls through to capture XYZ, XY or just X
        // depending on the need. I.e. UV textures would be only XY.
        switch(ordinatesType)
        {
            case XYZ :
                glcVector[i + 2] = ((GLfloat) (vertexArray[j].z));
            case XY :
                glcVector[i + 1] = ((GLfloat) (vertexArray[j].y));
            case X :
                glcVector[i + 0] = ((GLfloat) (vertexArray[j].x));
        }
    }
    return glcVector;
}

QVector<GLfloat> toGLCVector(
    const aiColor4t<float> * colorArray,
    unsigned int arraySize)
{
    QVector<GLfloat> glcVector(arraySize * 4); // always [r,g,b,a]
    for (unsigned int i = 0, j = 0; i < glcVector.size(); i += 4, ++j)
    {
        glcVector[i + 0] = ((GLfloat) (colorArray[j].r));
        glcVector[i + 1] = ((GLfloat) (colorArray[j].g));
        glcVector[i + 2] = ((GLfloat) (colorArray[j].b));
        glcVector[i + 3] = ((GLfloat) (colorArray[j].a));
    }
    return glcVector;
}

QList<GLuint> toGLCList(
    const QList<GLfloat> & verticesList,
    const aiFace * facesAarray,
    unsigned int size)
{
    QList<GLuint> glcList;
    for (unsigned int i = 0; i < size; ++i)
    {
        const aiFace assimpFace = facesAarray[i];
        QList<GLuint> glcFaceIndices;
        glcFaceIndices.reserve(assimpFace.mNumIndices);

        //---------------------------------------------------------------------
        // Copy all assimp indices of a single face to a QList
        std::copy(
            assimpFace.mIndices + 0,
            assimpFace.mIndices + assimpFace.mNumIndices,
            std::back_inserter(glcFaceIndices));

        //---------------------------------------------------------------------
        // GLC 2.5.0 can render only up to triangles,
        // hence triangulate all non-tri faces
        if (glcFaceIndices.size() > 3)
            glc::triangulatePolygon(& glcFaceIndices, verticesList);

        glcList.append(glcFaceIndices);
    }
    return glcList;
}

GLC_Matrix4x4 toGLCMatrix
    (const aiMatrix4x4 & assimpMatrix)
{
    // Assimp's is row-major while GLC is column-major
    double * temp = new double[16];
    //-------------------------------------------------------------------------
    temp[0]  = assimpMatrix.a1;
    temp[4]  = assimpMatrix.a2;
    temp[8]  = assimpMatrix.a3;
    temp[12] = assimpMatrix.a4;
    //-------------------------------------------------------------------------
    temp[1]  = assimpMatrix.b1;
    temp[5]  = assimpMatrix.b2;
    temp[9]  = assimpMatrix.b3;
    temp[13] = assimpMatrix.b4;
    //-------------------------------------------------------------------------
    temp[2]  = assimpMatrix.c1;
    temp[6]  = assimpMatrix.c2;
    temp[10] = assimpMatrix.c3;
    temp[14] = assimpMatrix.c4;
    //-------------------------------------------------------------------------
    temp[3]  = assimpMatrix.d1;
    temp[7]  = assimpMatrix.d2;
    temp[11] = assimpMatrix.d3;
    temp[15] = assimpMatrix.d4;
    //-------------------------------------------------------------------------
    GLC_Matrix4x4 glcMatrix(temp);
    delete [] temp;
    return glcMatrix;
}

GLC_Point3d toGLCPoint(const aiVector3D &v)
{
    return GLC_Point3d(v.x, v.y, v.z);
}

} // end namespace gui
} // end namespace repo
