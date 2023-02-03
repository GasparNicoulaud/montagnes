#version 150 core

in vec3 in_Vertex;

uniform mat4 inversemodproj;
uniform vec3 campos;

out vec3 inoutposMinusCampos;
out float inoutposMinusCamposLENGTH;


void main(void) {
    gl_Position = vec4(in_Vertex.xyz,1.0);
    
    vec4 worldpos = inversemodproj * gl_Position;
    worldpos.w = 1.0/worldpos.w;
    worldpos.xyz *= worldpos.w;
    inoutposMinusCampos = worldpos.xyz - campos;
    inoutposMinusCamposLENGTH = length(inoutposMinusCampos);
}

/*padding
 
 
 
 
 
 
 
 */
