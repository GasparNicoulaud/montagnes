
#version 150 core

in vec2 in_Vertex;

out vec3 pos; //the position for the fragment shader
//out vec3 normal;
out vec3 origVertex;
out float inoutscale;
out vec3 inoutcampos;
out float inouttime;
out float inoutUD; //inout upside down
//out float error; //0 if no error, 1 else

uniform mat4 modproj;
uniform mat4 modelpos;
uniform float scale;
uniform vec3 campos;
uniform float time;
uniform float isUpsideDown;
uniform vec2 offset;

uniform sampler2D matexture;

void main(void) {
    
    //here we should skew one out of two vertices in function of the dst to cam to stitch between LOD
    
    vec4 posFromTexture = vec4(in_Vertex,(texture(matexture, in_Vertex).x)*(1.0f/scale) * 0.000005,1.0);
    
    vec4 transformedPos = modelpos * posFromTexture; //place the vertex in the worldpos
    
    // all the out stuff
    gl_Position = modproj * transformedPos; //transform to screen space and send out
    
    //normal = normalize(cross(b - posFromTexture.xyz, c - posFromTexture.xyz));
    pos = transformedPos.xyz;
    origVertex = vec3(in_Vertex,posFromTexture.z);
    inoutscale = scale;
    inoutcampos = campos;
    inouttime = time;
    inoutUD = isUpsideDown;
}









