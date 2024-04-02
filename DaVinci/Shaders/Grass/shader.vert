uniform float elapsedTime;

void main(void)
{
	vec4 oldVertex = gl_Vertex;
		 
	mat4 newMatrix = mat4(vec4(gl_MultiTexCoord2.xyz, 0.0),
						  vec4(gl_Normal, 0.0),
						  vec4(gl_MultiTexCoord1.xyz, 0.0),
						  0.0, 0.0, 0.0, 1.0);
						  
	if(gl_MultiTexCoord0.y > 0.2)
	{
	  float cosine   = cos(elapsedTime)*gl_MultiTexCoord0.z*10.0;
	  oldVertex.x   += cosine;
	  oldVertex.z   += cosine;
	}  

	vec4 finalVertex      = newMatrix  * oldVertex;
		 finalVertex.xyz += gl_MultiTexCoord3.xyz;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position    = gl_ModelViewProjectionMatrix* finalVertex;
}