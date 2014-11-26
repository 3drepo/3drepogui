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

#ifndef REPO_GLCCAMERA_H
#define REPO_GLCCAMERA_H
//------------------------------------------------------------------------------
#include "assimp/scene.h"
#include "geometry/../GLC_Geometry"
#include "viewport/../GLC_Camera"

namespace repo {
namespace gui {

//! 3D wireframe representation of a GLC camera in space relative to parent transformation.
class RepoGLCCamera : public GLC_Geometry
{

public :

	RepoGLCCamera(
		const GLC_Point3d &position,
		const float depth, 
		const float halfWidth, 
		const float halfHeight,
		const GLC_Matrix4x4 &transformation);

	virtual ~RepoGLCCamera();

	//! Returns a bounding box of this wireframe camera representation
	virtual const GLC_BoundingBox& boundingBox();
	
	//! Returns a clone
	virtual GLC_Geometry* clone() const;
	
private :
    
	//! Renders the wireframe presentation as a line strip.
	virtual void glDraw(const GLC_RenderProperties&);
	
	//! Creates the wireframe representation from the given GLC Camera.
	void createWire();

	float depth;

	float halfWidth;

	float halfHeight;
	
	GLC_Point3d position;

	GLC_Matrix4x4 transformation;
	
}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_GLCCAMERA_H
