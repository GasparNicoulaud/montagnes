#version 150 core

in vec3 in_Vertex;
in vec3 in_Colors;
in vec2 in_UV;

uniform mat4 view;
uniform mat4 proj;

out vec3 inoutColor;
out vec2 inoutUV;

void main(void) {
    
    vec3 CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);
    
    vec3 starCenter = vec3(in_Vertex.xy - in_UV, in_Vertex.z); //star center in galaxy coord
    
    vec3 outputpos = starCenter + CameraRight_worldspace * in_UV.x + CameraUp_worldspace * in_UV.y;
    
    // all the out stuff
    gl_Position = proj * view * vec4(outputpos,1.0); //m * v * p in reverse
    inoutColor = in_Colors;
    inoutUV = in_UV;
}









//pading??







//pading?? more required




//pading??
