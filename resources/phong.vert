varying vec3 N;
varying vec3 v;
varying vec3 LightDir;


void main(void)  
{     
	gl_TexCoord[0]= gl_MultiTexCoord0;

	// Vertex normal
	N = normalize(gl_NormalMatrix * gl_Normal);

	// Vertex position in eye space
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       

	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 

	LightDir=vec3(gl_LightSource[0].position); 
}  
