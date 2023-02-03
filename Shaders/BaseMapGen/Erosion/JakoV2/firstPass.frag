
#version 410

uniform sampler2D terrain;
uniform sampler2D waterOutflow;

uniform int iteration;
uniform float global_rain_rate;

layout(location = 0) out vec4 terrainOut;
layout(location = 1) out vec4 waterOutflowOut;

float rng3(vec2 pos, int i)
{
    vec2 tmp = vec2(pos.x + i * 12.34, pos.y - i * 43.21);
    return fract(sin(dot(tmp ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
    //constant params
    //const float Kr = 0.1 * 0.05 * 0.05 * 10 * 5; //Rain rate
    float Kr = 0.005 * 0.05 * 5 * global_rain_rate; //Rain rate [1 - 10]
    
    //float Kr = 0;
    if(rng3(gl_FragCoord.xy,iteration) > 0.99)
    {
        //Kr = 0.1 * 0.05 * 0.05 * 100;
    }
    
    
    const vec2 right = vec2(1,0);
    const vec2 left = vec2(-1,0);
    const vec2 top = vec2(0,1);
    const vec2 bottom = vec2(0,-1);
    const float invTextureSize = 1.0/2048.0;
    
    //sample from terrains
    vec4 hereTerrain = texture(terrain, gl_FragCoord.xy * invTextureSize).rgba; //r rocks ,g water, b suspended sediments, a hardness
    vec3 rightTerrain = texture(terrain, (gl_FragCoord.xy + right) * invTextureSize).rgb;
    vec3 leftTerrain = texture(terrain, (gl_FragCoord.xy + left) * invTextureSize).rgb;
    vec3 topTerrain = texture(terrain, (gl_FragCoord.xy + top) * invTextureSize).rgb;
    vec3 bottomTerrain = texture(terrain, (gl_FragCoord.xy + bottom) * invTextureSize).rgb;
    
    //sample our previous flow
    vec4 previousFlow = texture(waterOutflow, gl_FragCoord.xy * invTextureSize).rgba;

    //rain
    hereTerrain.g = max(0,hereTerrain.g);
    float d1 = hereTerrain.g + Kr;
    float d1Right = rightTerrain.g + Kr;
    float d1Left = leftTerrain.g + Kr;
    float d1Top = topTerrain.g + Kr;
    float d1Bottom = bottomTerrain.g + Kr;
    
    //water out flow
    vec4 neighborsDelta;
    neighborsDelta.x = (hereTerrain.r + d1) - (rightTerrain.r + d1Right);
    neighborsDelta.y = (hereTerrain.r + d1) - (leftTerrain.r + d1Left);
    neighborsDelta.z = (hereTerrain.r + d1) - (topTerrain.r + d1Top);
    neighborsDelta.w = (hereTerrain.r + d1) - (bottomTerrain.r + d1Bottom);
    
    //previousFlow *= 0.99;
    vec4 outFlows = max(vec4(0), previousFlow + 0.2 * neighborsDelta);
    //vec4 outFlows = max(vec4(0), 0.2 * neighborsDelta);

    float sumOfConsideredFlow = (outFlows.x + outFlows.y + outFlows.z + outFlows.w);
    float Kfactor = clamp((d1 / (sumOfConsideredFlow + 0.000001)) - 0.000001,0,1.0);
    
    outFlows *= Kfactor;
    
    /*
    float hardness = 0.5;
    if(int(hereTerrain.r) % 200 > 100 )
    {
        hardness = 3.5;
    }
    
    hardness = 2.0;
    */
    
    terrainOut = vec4(hereTerrain.r,d1,hereTerrain.b,hereTerrain.a);
    //terrainOut = vec4(hereTerrain.r,d1,hereTerrain.b,hardness);

    waterOutflowOut = outFlows;
    
    /*
    if(outFlows.x + outFlows.y + outFlows.z + outFlows.w > d1)
    {
        terrainOut.r = 5000;
    }
    
    if(isnan(waterOutflowOut.x) || isinf(waterOutflowOut.x))
    {
        waterOutflowOut.x = 0;
        terrainOut.r = 5000;
    }
    if(isnan(waterOutflowOut.y) || isinf(waterOutflowOut.y))
    {
        waterOutflowOut.y = 0;
        terrainOut.r = 5000;
    }
    if(isnan(waterOutflowOut.z) || isinf(waterOutflowOut.z))
    {
        waterOutflowOut.z = 0;
        terrainOut.r = 5000;
    }
    if(isnan(waterOutflowOut.z) || isinf(waterOutflowOut.z))
    {
        waterOutflowOut.z = 0;
        terrainOut.r = 5000;
    }
    
    if(isnan(terrainOut.r) || isinf(terrainOut.r))
    {
        terrainOut.r = 5000;
    }
    
    if(isnan(terrainOut.g) || isinf(terrainOut.g))
    {
        terrainOut.g = 0;
        terrainOut.r = 5000;
    }
    
    if(isnan(terrainOut.b) || isinf(terrainOut.b))
    {
        terrainOut.b = 0;
        terrainOut.r = 5000;
    }
    
    if(terrainOut.g < 0)
    {
        terrainOut.g = 0.0;
    }
     
    if(terrainOut.b < 0)
    {
        terrainOut.b = 0.0;
    }
    */
}

