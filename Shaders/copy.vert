
#version 150 core

in vec3 in_Vertex;

uniform float mode;
out float inoutmode;

void main(void) {
    gl_Position = vec4(in_Vertex,1.0);
    inoutmode = mode;
}