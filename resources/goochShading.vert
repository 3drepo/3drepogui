//
// Vertex shader for Gooch shading
//
// Author: Randi Rost
//
// Copyright (c) 2002-2005 3Dlabs Inc. Ltd. 
//
// See 3Dlabs-License.txt for license information
//
uniform mat4 modelview_matrix;
uniform mat4 mvp_matrix;
uniform mat3 inv_modelview_matrix;

varying float NdotL;
varying vec3  ReflectVec;
varying vec3  ViewVec;

void main()
{
	gl_TexCoord[0]= gl_MultiTexCoord0;
	vec3 LightPosition= gl_LightSource[0].position.xyz;
    vec3 ecPos      = vec3(modelview_matrix * gl_Vertex);
    gl_ClipVertex	= modelview_matrix * gl_Vertex;
    vec3 tnorm      = normalize(inv_modelview_matrix * gl_Normal);
    vec3 lightVec   = normalize(LightPosition - ecPos);
    ReflectVec      = normalize(reflect(-lightVec, tnorm));
    ViewVec         = normalize(-ecPos);
    NdotL           = (dot(lightVec, tnorm) + 1.0) * 0.4;
    gl_Position     = mvp_matrix * gl_Vertex;
}