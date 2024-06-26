uniform sampler2D colorMap;
uniform sampler2D normalMap;

// New bumpmapping
varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;

void main()
{
	// lookup normal from normal map, move from [0, 1] to  [-1, 1] range, normalize
	vec3 normal = 2.0 * texture2D(normalMap, gl_TexCoord[0].st).rgb - 1.0;
	normal = normalize(normal);

	// compute diffuse lighting
	float lamberFactor = max (dot (lightVec, normal), 0.0) ;
	vec4 diffuseMaterial = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 diffuseLight = vec4(0.0, 0.0, 0.0, 0.0);

	// compute specular lighting
	vec4 specularMaterial;
	vec4 specularLight;
	float shininess;

	// compute ambient
	vec4 ambientLight = gl_LightSource[0].ambient;

	gl_FragColor = ambientLight;

	if (lamberFactor > 0.0)
	{
		diffuseMaterial = texture2D(colorMap, gl_TexCoord[0].st);
		diffuseLight  = gl_LightSource[0].diffuse;

		// In doom3, specular value comes from a texture 
		specularMaterial =  vec4(1.0);
		specularLight = gl_LightSource[0].specular;
		shininess = pow (max (dot (halfVec, normal), 0.0), 2.0);

		gl_FragColor =	diffuseMaterial * diffuseLight * lamberFactor;
		gl_FragColor +=	specularMaterial * specularLight * shininess;				
	}

	gl_FragColor +=	ambientLight;
}