#version 150 core

out vec4 lightMap;

uniform sampler2D heightMap;
uniform sampler2D shadowHeightMap;

void main(void)
{
    float invTextureSize = 1.0/2048.0;
    vec2 samplingCoord = gl_FragCoord.xy * invTextureSize;
    float shadowHeight = texture(shadowHeightMap,samplingCoord).r - texture(heightMap,samplingCoord).r;
    
    shadowHeight = clamp(shadowHeight,0.0,0.005);
    shadowHeight *= 200.0;
    
    shadowHeight = 1.0 - shadowHeight; //transform to allow simple mutltipication in the rendering pass
        
    lightMap = vec4(shadowHeight);
}

