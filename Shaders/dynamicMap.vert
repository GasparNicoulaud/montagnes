#version 410 core

in vec2 in_Vertex;

uniform vec2 offset;
uniform float scale;

out vec2 worldPos;
//out vec2 worldPosDividedBy10000;

void main(void) {
    
    // hack to fix white lines
    float marginScale = 0.01f; // 1 percent margin on each side
    vec2 in_VertexFixed = in_Vertex;
    in_VertexFixed *= 1.0/(1.0 - 2 * marginScale);
    
    gl_Position = vec4(in_Vertex,0.f,1.f);
    
    // pass the worldpos to frag shader
    worldPos = offset + (scale * 5000) * (in_VertexFixed + vec2(1.0));
    worldPos = worldPos * 0.0001 + vec2(0.5);
}



