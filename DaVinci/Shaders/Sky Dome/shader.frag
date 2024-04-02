uniform sampler2D noise;
uniform vec4      skyInfo;
varying float     intensitySqrd,
				intensity;

void main(void)
{
	vec4 noise1 = texture2D(noise, gl_TexCoord[0].xy),
		 noise2 = texture2D(noise, gl_TexCoord[0].zw);

	vec4 cloudFrag = (noise1 + noise2) * intensitySqrd,
	cloudColor = vec4((1.0 - intensity) * skyInfo.x,
					  (1.0 - intensity) * skyInfo.y,
					  intensity  *skyInfo.z, 0.0);

	gl_FragColor = cloudColor * (1.0 - cloudFrag.x) + cloudFrag;
}