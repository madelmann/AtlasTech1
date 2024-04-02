uniform vec4  params;
uniform vec3  offset;
varying float intensitySqrd;
varying float intensity;

void main(void)
{
	vec2 newUV  = vec2((gl_Vertex.x - offset.x)/params.x,
					   (gl_Vertex.z - offset.z)/params.z);

	gl_TexCoord[0].x  = newUV.x + params.w*0.66;
	gl_TexCoord[0].y  = newUV.y + params.w*0.33;
	gl_TexCoord[0].xy *= 2.0;

	gl_TexCoord[0].z = newUV.x + params.w*1.33;
	gl_TexCoord[0].w = newUV.y + params.w*1.66;
	gl_TexCoord[0].zw *= 3.0;

	//Get the vertex height and scale it to 0 - 1 range
	intensity        = gl_Vertex.y/params.y;
	intensity        = exp(clamp(intensity, 0.0, 1.0))/2.8;
	intensitySqrd    = intensity*intensity;

	gl_Position      = ftransform();
}