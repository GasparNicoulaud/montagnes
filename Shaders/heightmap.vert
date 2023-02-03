
#version 150 core

in vec2 in_Vertex;

uniform mat4 view;
uniform mat4 modproj;
uniform mat4 modelpos;
uniform float scale;

out vec2 worldPos;

void main(void) {
    
    float zoomToGetMargin = 1.2; // zoom the geometry to get better results on borders of tiles, sadly doesnt seem to work
    float xTranslate = -0.1;
    
    vec4 transformedPos = modelpos * vec4((in_Vertex + vec2(xTranslate,0)) * zoomToGetMargin, 0.0, 1.0); //place the vertex in the worldpos
    
    worldPos = transformedPos.xy;
    transformedPos.xy = (in_Vertex + vec2(xTranslate,0))  * zoomToGetMargin;
    
    // all the out stuff
    gl_Position = modproj * view * transformedPos; //modproj
}



