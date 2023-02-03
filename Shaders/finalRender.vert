
#version 150 core

in vec2 in_Vertex;

out vec3 position; //world pos the position for the fragment shader
out vec3 origVertex;
//out float inoutscale;
//out vec3 inoutcampos;
//out float inouttime;
out vec2 fixedSamplingCoord;
out vec2 uvForStaticmapSampling;

//out float error; //0 if no error, 1 else

uniform mat4 modproj;
uniform float scale;
//uniform vec3 campos;
//uniform float time;
uniform vec2 offset;

//uniform sampler2D localHeightmap;
uniform sampler2D heightHorizontalDisplacementAO;

void main(void) {
    
    // hack to fix white lines
    float marginScale = 0.01f; // 1 percent margin on each side
    vec2 samplingCoord = in_Vertex / 10000.0f;
    samplingCoord *= (1 - marginScale * 2);
    samplingCoord += vec2(marginScale);
    
    //here we should skew one out of two vertices in function of the dst to cam to stitch between LOD
    //vec3 worldPosition = vec3(in_Vertex * scale + offset, texture(localHeightmap,samplingCoord).x); // 10000.0f is map diameter
    vec3 worldPosition = vec3(in_Vertex * scale + offset, texture(heightHorizontalDisplacementAO,samplingCoord).x); // 10000.0f is map diameter

    worldPosition.xy += texture(heightHorizontalDisplacementAO,samplingCoord).yz;
    
    // all the out stuff
    gl_Position = modproj * vec4(worldPosition,1.0); //transform to screen space and send out
    
    position = worldPosition;
    origVertex = vec3(in_Vertex,0);
    fixedSamplingCoord = samplingCoord;
    uvForStaticmapSampling = ((in_Vertex * scale + offset) / 10000.0f) + vec2(0.5f);
}









