//
// Minnaert reflection model (per-pixel)
//
// by
// Massimiliano Corsini 
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

uniform sampler2D tex;
uniform bool useTexture;

void main (void)
{
	float m = 1.2; 
	vec3 N = normalize(normal);
	vec3 V = normalize(-vpos);
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);

	// diffuse color
	vec3 kd;
	if (useTexture)
	{
		vec3 texColor= vec3(texture2D(tex, gl_TexCoord[0].xy));
		kd = vec3(gl_LightSource[0].diffuse.rgb * texColor);
	}
	else
	{
		kd = vec3(gl_LightSource[0].diffuse.rgb * gl_FrontMaterial.diffuse.rgb);
	}

	// Minnaert reflection model
	float NdotV = dot(N,V);
	float NdotL = dot(N,L);

	gl_FragColor = vec4(pow(max(NdotV * NdotL, 0.0), m) * kd, gl_FrontMaterial.diffuse.a);
}
