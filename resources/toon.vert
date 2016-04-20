
uniform mat4 modelview_matrix;
uniform mat4 mvp_matrix;
uniform mat3 inv_modelview_matrix;


varying vec3 Normal;
varying vec3 LightDir;

void main(void)
{
	gl_TexCoord[0]= gl_MultiTexCoord0;
	Normal = normalize(inv_modelview_matrix * gl_Normal);
	gl_ClipVertex	= modelview_matrix * gl_Vertex;
	gl_Position = mvp_matrix * gl_Vertex;
	LightDir=vec3(gl_LightSource[0].position);
}
