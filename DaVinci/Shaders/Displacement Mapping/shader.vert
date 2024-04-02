varying vec3 lightVec; 
varying vec3 eyeVec;
varying vec2 texCoord;
attribute vec3 tangent; 

uniform sampler2D displacementMap;


#define textureSize 1024.0
#define texelSize 1.0 / 1024.0

vec4 tex2D_bilinear( uniform sampler2D tex, vec2 t )
{
	vec2 f = fract( t.xy * textureSize );
	vec4 t00 = texture2D( tex, t );
	vec4 t10 = texture2D( tex, t + vec2( texelSize, 0.0 ));
	vec4 tA = mix( t00, t10, f.x );
	vec4 t01 = texture2D( tex, t + vec2( 0.0, texelSize ) );
	vec4 t11 = texture2D( tex, t + vec2( texelSize, texelSize ) );
	vec4 tB = mix( t01, t11, f.x );
	return mix( tA, tB, f.y );
}	


void main(void)
{
	vec4 displacedVertexPos;
	vec4 displacementVec;
	float f;
	
	texCoord = gl_MultiTexCoord0.xy;
	
	displacementVec = tex2D_bilinear(displacementMap, gl_MultiTexCoord0.xy );
	//f = (0.30*displacementVec.x + 0.59*displacementVec.y + 0.11*displacementVec.z);
	f = displacementVec.x;
	
	displacedVertexPos = vec4(gl_Normal * f * 10.0, 0.0) + gl_Vertex;
	
	gl_Position = gl_ModelViewProjectionMatrix * displacedVertexPos;
	
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * tangent);
	vec3 b = cross(n, t);
	
	vec3 vVertex = vec3(gl_ModelViewMatrix * displacedVertexPos);
	vec3 tmpVec = gl_LightSource[0].position.xyz - vVertex;

	lightVec.x = dot(tmpVec, t);
	lightVec.y = dot(tmpVec, b);
	lightVec.z = dot(tmpVec, n);

	tmpVec = -vVertex;
	eyeVec.x = dot(tmpVec, t);
	eyeVec.y = dot(tmpVec, b);
	eyeVec.z = dot(tmpVec, n);
}