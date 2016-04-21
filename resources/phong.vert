varying vec3 N;
varying vec3 v;
varying vec3 LightDir;

uniform mat4 modelview_matrix;

void main(void)  
{     
	gl_TexCoord[0]= gl_MultiTexCoord0;
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);
	gl_ClipVertex = modelview_matrix * gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
	LightDir=vec3(gl_LightSource[0].position); 
}  
