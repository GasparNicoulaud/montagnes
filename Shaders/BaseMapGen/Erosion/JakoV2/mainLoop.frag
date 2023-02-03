#version 410

uniform sampler2D terrain;
uniform sampler2D waterOutflow;
uniform sampler2D soilFlowCardinals;
uniform sampler2D soilFlowCorners;

layout(location = 0) out vec3 terrainOut;
layout(location = 1) out vec4 waterOutflowOut;
layout(location = 2) out vec4 soilFlowCardinalsOut;
layout(location = 4) out vec4 soilFlowCornersOut;

void main(void)
{
    //constant params
    float DeltaT = 0.02; //[ 0 to 0.05 ] Time increment
    float Kr = 0.012; //[ 0 to 0.05 ] Rain rate
    float Ke = 0.015; //[ 0 to 0.05 ] Water evaporation rate
    float A = 20; //[ 0.1 to 60 ] Virtual pipe cross section area
    float g = 9.81 //[ 0.1 to 20 ] Gravity
    float Kc = 1 //[ 0.1 to 3] Sediment Capacity
    float Kt = 0.15 //[ 0 to 3 ] Thermal erosion rate
    float Ks = 0.5 //[ 0.1 to 2 ] Soil suspension rate
    float Kd = 1 //[ 0.1 to 3 ] Sediment deposition rate
    float Kh = 5 //[ 0 to 10 ] Sediment softening rate
    float Kdmax = 10 //[ 0 to 40 ] Maximal erosion depth
    float Ka = 0.8 //[ 0 to 1 ] Talus angle tangent coefficient
    float Ki = 0.1 //[ 0 to 1 ] Talus angle tangent bias
    
    
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    //terrains
    vec3 hereTerrain = texelFetch(terrain, fragCoord, 0).rgb; //r rocks ,g water, b suspended sediments
    vec3 rightTerrain = texelFetchOffset(waterOutflow, fragCoord, 0, zeroOneMinusOne.yx).rgb; //terrain of right cell
    vec3 leftTerrain = texelFetchOffset(waterOutflow, fragCoord, 0, zeroOneMinusOne.zx).rgb; //terrain of left cell
    vec3 topTerrain = texelFetchOffset(waterOutflow, fragCoord, 0, zeroOneMinusOne.xy).rgb; //terrain of top cell
    vec3 bottomTerrain = texelFetchOffset(waterOutflow, fragCoord, 0, zeroOneMinusOne.xz).rgb; //terrain of bottom cell
    
    //flows
    vec4 ourPreviousFlows = texelFetch(waterOutflow, fragCoord, 0).rgba; //right left top bottom
    float flowFromRight = texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.yx).x //left pipe of right cell
    float flowFromLeft =  texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.zx).y //right pipe of left cell
    float flowFromTop =  texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.xy).z //bottom pipe of top cell
    float flowFromBottom =  texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.xz).w; //top pipe of bottom cell

    //computation of water height change
    
    
    //update water height
    float DeltaV = DeltaT * (flowFromRight + flowFromLeft + flowFromTop + flowFromBottom - ourPreviousFlows.x - ourPreviousFlows.y - ourPreviousFlows.z - ourPreviousFlows.w);
    float d2 = hereTerrain.g + DeltaV;
    
    //computation of velocity field
    float DeltaWx = 0.5 * (flowFromLeft - ourPreviousFlows.y + ourPreviousFlows.x - flowFromRight);
    float DeltaWy = 0.5 * (flowFromBottom - ourPreviousFlows.w + ourPreviousFlows.z - flowFromTop);
    vec2 velocityField = vec2(DeltaWx,DeltaWy);
    
    //computation of dissolution capacity
    float C = Kc * length(velocityField);
    
    //computation of sediment dissolution and deposition
    if(hereTerrain.b < C)
    {
        hereTerrain.r -= DeltaT * Ks * (C - hereTerrain.b);
        s1 = hereTerrain.b + DeltaT * Ks * (C - hereTerrain.b);
        d3 = d2 + DeltaT * Ks * (C - hereTerrain.b);
    }
    else
    {
        hereTerrain.r += DeltaT * Kd * (hereTerrain.b - C);
        s1 = hereTerrain.b - DeltaT * Kd * (hereTerrain.b - C);
        d3 = d2 - DeltaT * Kd * (hereTerrain.b - C);
    }
    
    //move dissolved sediments along the water
    terrainOut.b = s1 * (fragCoord.x - velocityField.x * Delta)

    
    //rain
    float d1 = hereTerrain.g + Kr * DeltaT;
    float d1Right = rightTerrain.g + Kr * DeltaT;
    float d1Left = leftTerrain.g + Kr * DeltaT;
    float d1Top = topTerrain.g + Kr * DeltaT;
    float d1Bottom = bottomTerrain.g + Kr * DeltaT;
    
    //water out flow
    float deltaHR = hereTerrain.r + d1 - rightTerrain.r - d1Right;
    float deltaHL = hereTerrain.r + d1 - leftTerrain.r - d1Left;
    float deltaHT = hereTerrain.r + d1 - topTerrain.r - d1Top;
    float deltaHB = hereTerrain.r + d1 - bottomTerrain.r - d1Bottom;
    float outFlowRight = max(0, ourPreviousFlows.x + DeltaT * A * g * deltaHR);
    float outFlowLeft = max(0, ourPreviousFlows.y + DeltaT * A * g * deltaHL);
    float outFlowTop = max(0, ourPreviousFlows.z + DeltaT * A * g * deltaHT);
    float outFlowBottom = max(0, ourPreviousFlows.w + DeltaT * A * g * deltaHB);
    
    float Kfactor = max(1, d1 / ((outFlowRight + outFlowLeft + outFlowTop + outFlowBottom) * DeltaT));
    
    outFlowRight *= Kfactor;
    outFlowLeft *= Kfactor;
    outFlowTop *= Kfactor;
    outFlowBottom *= Kfactor;
    
    float DeltaV = DeltaT * (flowFromRight + flowFromLeft + flowFromTop + flowFromBottom - outFlowRight - outFlowLeft - outFlowTop - outFlowBottom);
    
}

