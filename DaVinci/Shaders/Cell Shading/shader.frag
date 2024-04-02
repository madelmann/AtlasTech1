varying vec3 normal;
uniform vec4 light_position;
uniform vec4 color;
uniform sampler1D colorMap;

void main()
{
   float intensity = dot(light_position.xyz, normal);
   gl_FragColor = color * texture1D(colorMap, intensity);
}
