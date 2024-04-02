varying vec4 V;
varying vec3 N;
 
void main() 
{
	V = gl_Vertex;
	N = gl_Normal;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}