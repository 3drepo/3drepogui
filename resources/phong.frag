varying vec3 N;
varying vec3 v; 
varying vec3 LightDir;

uniform sampler2D tex;
uniform bool useTexture; 
  
void main (void)  
{  
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
	vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
	vec3 R = normalize(-reflect(L,N));  
 
	//calculate Ambient Term:  
	vec4 Iamb = gl_FrontLightProduct[0].ambient;       
 
	//calculate Diffuse Term:  
	vec3 color;
	if (useTexture)
	{
		color = vec3(texture2D(tex, gl_TexCoord[0].xy));
	}
	else
	{
		color = vec3(gl_FrontLightProduct[0].diffuse);
	}
	vec4 Idiff = vec4(color * max(dot(N,L), 0.0), gl_FrontMaterial.diffuse.a);
	Idiff = clamp(Idiff, 0.0, 1.0);  

	// calculate Specular Term:
	vec4 Ispec = gl_FrontLightProduct[0].specular 
                * pow(max(dot(R,E),0.0), gl_FrontMaterial.shininess);
	Ispec = clamp(Ispec, 0.0, 1.0); 


	// write Total Color:  
	gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;     
}
