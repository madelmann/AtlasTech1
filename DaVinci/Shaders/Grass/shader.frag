uniform sampler2D grass;
uniform float alphaBooster;

void main(void)
{
	vec4  grassSample = texture2D(grass, gl_TexCoord[0].xy);
	float newAlpha    = grassSample.a*alphaBooster;
	gl_FragColor      = vec4(grassSample.xyz, newAlpha);
}