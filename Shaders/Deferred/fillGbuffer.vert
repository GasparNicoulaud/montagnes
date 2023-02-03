#version 410

in vec2 in_Vertex;

uniform mat4 modproj;
uniform float scale;
uniform vec2 offset;

uniform sampler2D heightHorizontalDisplacementAO;

out vec3 positionInOut;
out vec2 fixedSamplingCoord;

void main(void) {
    
    // hack to fix white lines
    float marginScale = 0.01f; // 1 percent margin on each side
    vec2 samplingCoord = in_Vertex / 10000.0f;
    samplingCoord *= (1 - marginScale * 2);
    samplingCoord += vec2(marginScale);
    
    vec3 worldPosition = vec3(in_Vertex * scale + offset, texture(heightHorizontalDisplacementAO,samplingCoord).x); // 10000.0f is map diameter

    worldPosition.xy += texture(heightHorizontalDisplacementAO,samplingCoord).yz;
    
    // all the out stuff
    gl_Position = modproj * vec4(worldPosition,1.0); //transform to screen space and send out
    positionInOut = worldPosition;
    fixedSamplingCoord = samplingCoord;
}









