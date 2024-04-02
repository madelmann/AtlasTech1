#define MAX_LIGHTS 8
#define NUM_LIGHTS 1

varying vec3 lightVec[MAX_LIGHTS]; 
varying vec3 viewVec;

attribute vec4 glTangent4f; 

void main(void)
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * glTangent4f.xyz);
	vec3 b = cross(n, t);

	vec3 v;
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	int i;
	for (i = 0; i < NUM_LIGHTS; ++i)
	{
		vec3 lVec = gl_LightSource[i].position.xyz - vVertex;
		v.x = dot(lVec, t);
		v.y = dot(lVec, b);
		v.z = dot(lVec, n);
		lightVec[i] = v;
	}

	vec3 vVec = -vVertex;
	v.x = dot(vVec, t);
	v.y = dot(vVec, b);
	v.z = dot(vVec, n);
	viewVec = v;
}