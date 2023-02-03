#version 410

//textures
uniform sampler2D localNormalmap;
uniform sampler2D albedoRoughnessMap;

// in from vertex shader
in vec3 positionInOut;
in vec2 fixedSamplingCoord;

layout(location = 0) out vec3 position;
layout(location = 1) out vec4 albedoRoughness;
layout(location = 2) out vec3 normal;

void main(void)
{
    position = positionInOut;
    albedoRoughness = texture(albedoRoughnessMap,fixedSamplingCoord);
    //albedoRoughness = vec4(1.0,0.5,0.0,1.0);
    normal = texture(localNormalmap,fixedSamplingCoord).xyz;
}

