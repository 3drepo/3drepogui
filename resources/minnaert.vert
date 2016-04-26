//
// Minnaert reflection model (per-pixel)
//
// by
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

uniform mat4 modelview_matrix;
uniform mat4 mvp_matrix;
uniform mat3 inv_modelview_matrix;

varying vec3 normal;
varying vec3 vpos;

void main (void)
{
	gl_TexCoord[0]= gl_MultiTexCoord0;
	// vertex normal
	normal = normalize(inv_modelview_matrix * gl_Normal);
	
	// vertex position (in eye-space)
	vpos = vec3(modelview_matrix * gl_Vertex);
	gl_ClipVertex	= modelview_matrix * gl_Vertex;
	
	
	gl_Position = mvp_matrix * gl_Vertex;
}
