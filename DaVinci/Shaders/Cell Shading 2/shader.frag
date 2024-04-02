varying vec2 texCoord;

uniform sampler2D colorMap;

void main()
{
   gl_FragColor = texture2D(colorMap, texCoord);
}
