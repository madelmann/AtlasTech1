#version 120
 
#extension GL_ARB_geometry_shader4 : enable
 
//input GL_TRIANGLES
//output GL_LINE_STRIP
 
varying in vec4 V[3];
varying in vec3 N[3];
 
uniform float nLength;
 
void createNormal( vec3 V, vec3 N )
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4( V, 1.0f );
    EmitVertex();
 
    gl_Position = gl_ModelViewProjectionMatrix * vec4( V + N * nLength, 1.0f );
    EmitVertex();
 
    EndPrimitive();
}
 
void main()
{
    for ( int i = 0; i < 3; ++i )
    {
        createNormal( V[i].xyz, N[i] );
    }
}