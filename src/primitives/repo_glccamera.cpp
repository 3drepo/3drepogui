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

#include "repo_glccamera.h"


repo::gui::RepoGLCCamera::RepoGLCCamera(
	const GLC_Point3d &position, 
	const float depth, 
	const float halfWidth, 
	const float halfHeight,
	const GLC_Matrix4x4 &transformation)
	: position(position)
	, depth(depth)
	, halfWidth(halfWidth)
	, halfHeight(halfHeight)
	, transformation(transformation)
	, GLC_Geometry("Camera", true) 
{
}

repo::gui::RepoGLCCamera::~RepoGLCCamera() {}

const GLC_BoundingBox& repo::gui::RepoGLCCamera::boundingBox()
{
	if (NULL == m_pBoundingBox)
    {
		m_pBoundingBox= new GLC_BoundingBox();
        if (m_WireData.isEmpty()) 
			createWire();
		m_pBoundingBox->combine(m_WireData.boundingBox());
        m_pBoundingBox->combine(GLC_Vector3d(0.0, 0.0, -2 * glc::EPSILON));
	}
	return *m_pBoundingBox;
}

GLC_Geometry* repo::gui::RepoGLCCamera::clone() const
{
	return new RepoGLCCamera(*this);
}

void repo::gui::RepoGLCCamera::glDraw(const GLC_RenderProperties& renderProperties)
{
	if (m_WireData.isEmpty())
		createWire();
	m_WireData.glDraw(renderProperties, GL_LINE_STRIP);
}

void repo::gui::RepoGLCCamera::createWire()
{
    GLfloatVector vertices;

	GLC_Matrix4x4 rotation = transformation.rotationMatrix().transpose(); //camera.modelViewMatrix().rotationMatrix().transpose(); //.invert();
	GLC_Point3d eye = position; // == rotation * GLC_Point3d(0,0,0) + camera.eye()

	GLC_Point3d frustrumTopLeft = rotation * GLC_Point3d(-halfWidth, halfHeight, depth) + position; 
	GLC_Point3d frustrumTopRight = rotation * GLC_Point3d(halfWidth, halfHeight, depth) + position; 
	GLC_Point3d frustrumBottomRight = rotation * GLC_Point3d(halfWidth, -halfHeight, depth) + position; 
	GLC_Point3d frustrumBottomLeft = rotation * GLC_Point3d(-halfWidth, -halfHeight, depth) + position; 

	float triangleHalfBase = halfWidth * 0.7f; // Half of the side of the triangle
	float triangleHeight = triangleHalfBase; // sqrt(triangleHalfBase * 2.0f * triangleHalfBase * 2.0f - triangleHalfBase * triangleHalfBase);
	float verticalOffset = halfHeight * 1.1f; // Y-axis offset to position triangle above viewport
	GLC_Point3d triangleLeft  = rotation * GLC_Point3d(-triangleHalfBase, verticalOffset, depth) + position; 
	GLC_Point3d triangleTop   = rotation * GLC_Point3d(0.0f, verticalOffset + triangleHeight, depth) + position; 
	GLC_Point3d triangleRight = rotation * GLC_Point3d(triangleHalfBase, verticalOffset, depth) + position; 

	//-------------------------------------------------------------------------
	// Frustrum arms
	vertices << eye.x() << eye.y() << eye.z(); // eye
	vertices << frustrumTopLeft.x() << frustrumTopLeft.y() << frustrumTopLeft.z(); // top left
	GLC_Geometry::addVerticeGroup(vertices);
	vertices.clear();
	
	vertices << eye.x() << eye.y() << eye.z(); // eye
	vertices << frustrumTopRight.x() << frustrumTopRight.y() << frustrumTopRight.z(); // top right
	GLC_Geometry::addVerticeGroup(vertices);
	vertices.clear();

	vertices << eye.x() << eye.y() << eye.z(); // eye
	vertices << frustrumBottomRight.x() <<frustrumBottomRight.y() << frustrumBottomRight.z(); // bottom right
	GLC_Geometry::addVerticeGroup(vertices);
	vertices.clear();

	vertices << eye.x() << eye.y() << eye.z(); // eye
	vertices << frustrumBottomLeft.x() << frustrumBottomLeft.y() << frustrumBottomLeft.z(); // bottom left
	GLC_Geometry::addVerticeGroup(vertices);
	vertices.clear();

	//-------------------------------------------------------------------------
	// Viewport 
	vertices << frustrumTopLeft.x() << frustrumTopLeft.y() << frustrumTopLeft.z(); // top left
	vertices << frustrumTopRight.x() << frustrumTopRight.y() << frustrumTopRight.z(); // top right
	vertices << frustrumBottomRight.x() << frustrumBottomRight.y() << frustrumBottomRight.z(); // bottom right
	vertices << frustrumBottomLeft.x() << frustrumBottomLeft.y() << frustrumBottomLeft.z(); // bottom left
	vertices << frustrumTopLeft.x() << frustrumTopLeft.y() << frustrumTopLeft.z(); // top left
	GLC_Geometry::addVerticeGroup(vertices);
	vertices.clear();

	//-------------------------------------------------------------------------
	// Up Triangle	
	vertices << triangleLeft.x() << triangleLeft.y() << triangleLeft.z(); // -1.0f << -0.4f << 0.3f; // left
	vertices << triangleTop.x() << triangleTop.y() << triangleTop.z(); // -1.0f << 0.4f << 0.3f; // top 
	vertices << triangleRight.x() << triangleRight.y() << triangleRight.z(); // -1.0f << 0.0f << 0.6f; // right
	vertices << triangleLeft.x() << triangleLeft.y() << triangleLeft.z(); // -1.0f << -0.4f << 0.3f; // left
	GLC_Geometry::addVerticeGroup(vertices);
	vertices.clear();
}
