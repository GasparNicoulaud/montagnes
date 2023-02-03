#version 150 core

in vec3 in_Vertex;

uniform mat4 MVP;
uniform float zOffset;

out vec3 inoutpos;

void main()
{
    vec4 transformedpos = vec4(in_Vertex * 10,1.0f);
    //transformedpos.x += 20;
    //transformedpos.z += 150;
    transformedpos.z += zOffset - 40.0;// * 500;
    
    //transformedpos.z = 0;
    
    transformedpos += vec4(vec3(0.2001),0);
    gl_Position = MVP * transformedpos;
    inoutpos = transformedpos.xyz;
}

/*
 padding
 
 
 
 */
