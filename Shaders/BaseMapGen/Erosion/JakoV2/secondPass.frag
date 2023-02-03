
#version 410

uniform sampler2D terrain;
uniform sampler2D waterOutflow;

layout(location = 0) out vec4 terrainOut;
layout(location = 1) out vec4 waterOutflowOut;

//terrains are r : height, g, hardness
float getThermalComponentFrom(vec2 neighborTerrain, vec2 ourTerrain, float neighborTerrainDist, float thermalFlow)
{
    
    float talusOfNeighbor = neighborTerrain.g;
    talusOfNeighbor *= neighborTerrainDist;
    float talusHere = ourTerrain.g;
    talusHere *= neighborTerrainDist;
    
    float thermalDelta = 0;
    if(neighborTerrain.r > talusOfNeighbor + ourTerrain.r)
    {
        thermalDelta += (neighborTerrain.r - (talusOfNeighbor + ourTerrain.r)) * thermalFlow;
    }
    if(ourTerrain.r > talusHere + neighborTerrain.r)
    {
        thermalDelta -= (ourTerrain.r - (talusHere + neighborTerrain.r)) * thermalFlow;
    }
    return thermalDelta;
}

void main(void)
{
    //constant params
    float DeltaT = 0.5; //[ 0 to 0.05 ] Time increment
    float Kc = 1.0; //[ 0.1 to 3] Sediment Capacity
    float Ks = 0.05 * 5; //[ 0.1 to 2 ] Soil suspension rate
    float Kd = 0.05 * 5; //[ 0.1 to 3 ] Sediment deposition rate
    float thermalFlow = 0.15; //max 0.15 ?
    
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const vec3 zeroOneMinusOne = vec3(0,1,-1);
    const float invTextureSize = 1.0/2048.0;
    const vec2 right = vec2(1,0);
    const vec2 left = vec2(-1,0);
    const vec2 top = vec2(0,1);
    const vec2 bottom = vec2(0,-1);
    
    //terrains
    vec4 hereTerrain = texelFetch(terrain, fragCoord, 0).rgba; //r rocks ,g water, b suspended sediments, a hardness
    float originalHardness = hereTerrain.a;
    vec4 rightTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.yx) * invTextureSize).rgba;
    vec4 leftTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.zx) * invTextureSize).rgba;
    vec4 topTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.xy) * invTextureSize).rgba;
    vec4 bottomTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.xz) * invTextureSize).rgba;
    vec4 rightTopTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.yy) * invTextureSize).rgba;
    vec4 leftTopTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.zy) * invTextureSize).rgba;
    vec4 leftBottomTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.zz) * invTextureSize).rgba;
    vec4 rightBottomTerrain = texture(terrain, (gl_FragCoord.xy - zeroOneMinusOne.yz) * invTextureSize).rgba;
    
    /*
    float stratBegin = 1700;
    float stratEnd = 1720;
    float stratHardness = 5.0;
    if(hereTerrain.r > stratBegin && hereTerrain.r < stratEnd)
    {
        hereTerrain.a += stratHardness;
    }
    if(rightTerrain.r > stratBegin && rightTerrain.r < stratEnd)
    {
        rightTerrain.a += stratHardness;
    }
    if(leftTerrain.r > stratBegin && leftTerrain.r < stratEnd)
    {
        leftTerrain.a += stratHardness;
    }
    if(topTerrain.r > stratBegin && topTerrain.r < stratEnd)
    {
        topTerrain.a += stratHardness;
    }
    if(bottomTerrain.r > stratBegin && bottomTerrain.r < stratEnd)
    {
        bottomTerrain.a += stratHardness;
    }
    if(rightTopTerrain.r > stratBegin && rightTopTerrain.r < stratEnd)
    {
        rightTopTerrain.a += stratHardness;
    }
    if(leftTopTerrain.r > stratBegin && leftTopTerrain.r < stratEnd)
    {
        leftTopTerrain.a += stratHardness;
    }
    if(leftBottomTerrain.r > stratBegin && leftBottomTerrain.r < stratEnd)
    {
        leftBottomTerrain.a += stratHardness;
    }
    if(rightBottomTerrain.r > stratBegin && rightBottomTerrain.r < stratEnd)
    {
        rightBottomTerrain.a += stratHardness;
    }
    */
    
    //flows
    vec4 ourPreviousFlows = texelFetch(waterOutflow, fragCoord, 0).rgba; //right left top bottom
    float flowFromRight = texture(waterOutflow, (gl_FragCoord.xy + right) * invTextureSize).y;
    float flowFromLeft = texture(waterOutflow, (gl_FragCoord.xy + left) * invTextureSize).x;
    float flowFromTop = texture(waterOutflow, (gl_FragCoord.xy + top) * invTextureSize).w;
    float flowFromBottom = texture(waterOutflow, (gl_FragCoord.xy + bottom) * invTextureSize).z;
    
    //thermal erosion
    float thermalErosionDelta = 0;
    thermalErosionDelta += getThermalComponentFrom(rightTerrain.ra, hereTerrain.ra, 1.0, thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(leftTerrain.ra, hereTerrain.ra, 1.0, thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(topTerrain.ra, hereTerrain.ra, 1.0, thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(bottomTerrain.ra, hereTerrain.ra, 1.0, thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(rightTopTerrain.ra, hereTerrain.ra, sqrt(2.0), thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(leftTopTerrain.ra, hereTerrain.ra, sqrt(2.0), thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(leftBottomTerrain.ra, hereTerrain.ra, sqrt(2.0), thermalFlow);
    thermalErosionDelta += getThermalComponentFrom(rightBottomTerrain.ra, hereTerrain.ra, sqrt(2.0), thermalFlow);
    
    
    //update water height
    float outFlowSum = ourPreviousFlows.x + ourPreviousFlows.y + ourPreviousFlows.z + ourPreviousFlows.w;
    float inFlowSum = flowFromRight + flowFromLeft + flowFromTop + flowFromBottom;
    float DeltaV = DeltaT * (inFlowSum - outFlowSum);
    
    hereTerrain.g = max(0,hereTerrain.g);
    float d2 = hereTerrain.g + DeltaV;
    
    //computation of velocity field
    float DeltaWx = ((flowFromLeft - ourPreviousFlows.y) + (ourPreviousFlows.x - flowFromRight));
    float DeltaWy = ((flowFromBottom - ourPreviousFlows.w) + (ourPreviousFlows.z - flowFromTop));
    vec2 velocityFieldVector = vec2(DeltaWx,DeltaWy);
    
    //computation of dissolution capacity
    const vec2 size = vec2(2.0,0.0);
    vec3 va = normalize(vec3(size.xy,rightTerrain.r-leftTerrain.r));
    vec3 vb = normalize(vec3(size.yx,topTerrain.r-bottomTerrain.r));
    //vec3 va = normalize(vec3(size.xy,leftTerrain.r-rightTerrain.r));
    //vec3 vb = normalize(vec3(size.yx,bottomTerrain.r-topTerrain.r));
    vec3 surfaceNormal = normalize(cross(va,vb));
    //float tiltAngle = acos(dot(surfaceNormal, vec3(0,0,1)));
    
    float tiltDot = dot(surfaceNormal, vec3(0,0,1));
    float tiltFactor = sqrt(max(0.05,0.6 - tiltDot * tiltDot));
    //tiltFactor = 1.0;
    //float C = Kc * sin(max(tiltAngle,0.001)) * length(velocityFieldVector);
    float C = Kc * tiltFactor * length(velocityFieldVector);
    
    //C = Kc * length(velocityFieldVector);
    //C = Kc * tiltFactor * d2;
    //C = clamp(C,0,d2);
    //C = clamp(C,0,2.0);

    //float C = Kc * length(velocityFieldVector);
    // */
    
    //sin(acos(x)) = sqrt(1 - x * x) , so we could optimze the acos away
    
    //velocityFieldVector *= 1.0;
    //velocityFieldVector *= 10.0;
    if(length(velocityFieldVector) > 1)
    {
        //velocityFieldVector /= length(velocityFieldVector);
        //terrainOut.r = 5000;
        //hereTerrain.a = -100;
    }
    
    if(isnan(length(velocityFieldVector)) || isinf(length(velocityFieldVector)))
    {
        velocityFieldVector = vec2(0);
        terrainOut.r = 5000;
    }
    
    //float newSediments = texture(terrain, (gl_FragCoord.xy - velocityFieldVector * 1.0 ) * invTextureSize).b;
    //hereTerrain.b = newSediments;
    hereTerrain.b = 0;
    //hereTerrain.b *= 0.5;
    hereTerrain.b += d2 * 0.5;

    /*
    float epsilon = 0.000000000001;
    float rcvdSediments = flowFromLeft * (leftTerrain.b/(leftTerrain.g + epsilon));
    rcvdSediments += flowFromRight * (rightTerrain.b/(rightTerrain.g + epsilon));
    rcvdSediments += flowFromTop * (topTerrain.b/(topTerrain.g + epsilon));
    rcvdSediments += flowFromBottom * (bottomTerrain.b/(bottomTerrain.g + epsilon));
    float ourSedConcentration = hereTerrain.b/(hereTerrain.g + epsilon);
    float sentSediments = (ourPreviousFlows.x + ourPreviousFlows.y + ourPreviousFlows.z + ourPreviousFlows.w) * ourSedConcentration;
    float sedDelta = rcvdSediments - sentSediments;
    hereTerrain.b += sedDelta;
    //hereTerrain.b = rcvdSediments;
    hereTerrain.b = max(hereTerrain.b,0);
    */
    
    float s1 = hereTerrain.b;
    float d3 = d2;
    float Rhardness = hereTerrain.a;
    Rhardness = max(0.1,Rhardness);
    
    float erodingFactor = max(0.1,3.5 - Rhardness);
    //erodingFactor = 0.2;
    //erodingFactor = 1.0;
    //Rhardness = clamp(sin(gl_FragCoord.y * 0.01) + cos(gl_FragCoord.x * 0.01),0.1,2.0);
    //computation of sediment dissolution and deposition
    if(hereTerrain.b < C)
    {
        hereTerrain.r -= erodingFactor * Ks * (C - hereTerrain.b);
        //s1 += erodingFactor * Ks * (C - hereTerrain.b);
        //d3 = d2 + erodingFactor * Ks * (C - hereTerrain.b);
    }
    else
    {
        hereTerrain.r += Kd * (hereTerrain.b - C);
        s1 -= Kd * (hereTerrain.b - C);
        d3 = d2 - Kd * (hereTerrain.b - C);
    }
        
    /*
    if(d2 < 0)
    {
        hereTerrain.r = -100;
    }
    */
    
    //evaporation
    //d3 *= 0.99999;
    //d3 *= 0.995;
    //d3 *= 0.998;
    //d3 -= 0.00015;

    //prevent negative water
    d3 = max(0,d3);
    //prevent negative sed
    s1 = max(0,s1);
    
    //rock softening by water, and hardening when dry
    /*
    originalHardness -= min(d3,0.05) * 0.01;
    originalHardness += 0.0001;
    originalHardness = clamp(originalHardness,1.0,5.0);
    */
    
    hereTerrain.r = max(hereTerrain.r,min(rightTerrain.r,min(leftTerrain.r,min(topTerrain.r,bottomTerrain.r))));
    
    terrainOut = vec4(hereTerrain.r + thermalErosionDelta,d3,s1,originalHardness);

    waterOutflowOut = ourPreviousFlows;
    
    terrainOut.r = clamp(terrainOut.r, 0, 3000);
    
    //terrainOut.r = clamp(terrainOut.r, 1000, 1500);
    
    /*
    if(terrainOut.g <= 0)
    {
        //terrainOut.r = 5000;
    }
    if(terrainOut.b <= 0)
    {
        //terrainOut.r = 5000;
    }
    */
    
    /*
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
    */

}

