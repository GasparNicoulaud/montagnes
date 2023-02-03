#version 410

uniform sampler2D heightRocksSediments;
uniform sampler2D waterPipes;
uniform sampler2D sedimentPipes;

layout(location = 0) out vec2 heightRocksSedimentsOut;
layout(location = 1) out vec4 waterPipesOut;
layout(location = 2) out vec4 sedimentPipesOut;

uniform int iterCounter;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
    //constant params
    
    float rainAmount = 0.001;
    
    if(rand(gl_FragCoord.xy + rand(vec2(iterCounter % 123,sqrt(iterCounter)))) > 0.99)
    {
        rainAmount = 0.1;
    }
    
    float evaporationRate = 0.99;
    float sedimentRateOfRepose = 1.0;
    float rockRateOfRepose = 2.5;
    
    sedimentRateOfRepose = 0.5;
    //rockRateOfRepose = 1.0;
    
    //get data from textures
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    //heightmap
    vec2 here = texelFetch(heightRocksSediments, fragCoord, 0).rg; //r rocks , g sediments
    float heightHere = here.r + here.g;
    float right = texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.yx).r
                + texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.yx).g;
    float left = texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.zx).r
               + texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.zx).g;
    float top = texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.xy).r
              + texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.xy).g;
    float bottom = texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.xz).r
                 + texelFetchOffset(heightRocksSediments, fragCoord, 0, zeroOneMinusOne.xz).g;
    float lowestNeighbor = min(left,min(right,min(top,bottom)));
    float howHigherWeAreThanLowestNeighbor = heightHere - lowestNeighbor;
    
    vec2 slopeDirection = normalize(vec2(right - left, top - bottom));
    
    //slopeDirection = vec2(right - left, top - bottom);
    //slopeDirection /= abs(slopeDirection.x) + abs(slopeDirection.y);
    
    //this forces to chooose a direction, fixes bugs bug not ideal
    if(left < right && left < top && left < bottom && left < heightHere)
    {
        slopeDirection = vec2(1,0);
    }
    else if(right < top && right < bottom && right < heightHere)
    {
        slopeDirection = vec2(-1,0);
    }
    else if(top < bottom && top < heightHere)
    {
        slopeDirection = vec2(0,1);
    }
    else if(bottom < heightHere)
    {
        slopeDirection = vec2(0,-1);
    }
    
    
    /*vec4 directionFactor = vec4(0); //hot one encoding for where we will flow (could be all 0 if we are lower than all neighbors)
    if(left < right && left < top && left < bottom && left < heightHere)
    {
        directionFactor.x = 1.0;
    }
    else if(right < top && right < bottom && right < heightHere)
    {
        directionFactor.y = 1.0;
    }
    else if(top < bottom && top < heightHere)
    {
        directionFactor.z = 1.0;
    }
    else if(bottom < heightHere)
    {
        directionFactor.w = 1.0;
    }*/
    
    //vec4 directionFactor;
    /*
    directionFactor.x = max(0,dot(slopeDirection,vec2( 1, 0)));
    directionFactor.y = max(0,dot(slopeDirection,vec2(-1, 0)));
    directionFactor.z = max(0,dot(slopeDirection,vec2( 0, 1)));
    directionFactor.w = max(0,dot(slopeDirection,vec2( 0,-1)));
    
    directionFactor = normalize(directionFactor);
    */
    
    ///*
    vec4 directionFactor;
    directionFactor.x = max(0,slopeDirection.x); //factor toward left
    directionFactor.y = max(0,slopeDirection.x * -1); //factor toward right
    directionFactor.z = max(0,slopeDirection.y); //factor toward bottom
    directionFactor.w = max(0,slopeDirection.y * -1); //factor toward top
    
    if(howHigherWeAreThanLowestNeighbor <= 0)
    {
        directionFactor = vec4(0);
    }
    // */
    
    directionFactor = clamp(directionFactor,0,1);
    
    directionFactor = vec4(0); //hot one encoding for where we will flow (could be all 0 if we are lower than all neighbors)
    if(left < right && left < top && left < bottom && left < heightHere)
    {
        directionFactor.y = 1.0;
    }
    else if(right < top && right < bottom && right < heightHere)
    {
        directionFactor.x = 1.0;
    }
    else if(top < bottom && top < heightHere)
    {
        directionFactor.z = 1.0;
    }
    else if(bottom < heightHere)
    {
        directionFactor.w = 1.0;
    }
    
    //water
    float waterLevel = texelFetchOffset(waterPipes, fragCoord, 0, zeroOneMinusOne.yx).x
                     + texelFetchOffset(waterPipes, fragCoord, 0, zeroOneMinusOne.zx).y
                     + texelFetchOffset(waterPipes, fragCoord, 0, zeroOneMinusOne.xy).z
                     + texelFetchOffset(waterPipes, fragCoord, 0, zeroOneMinusOne.xz).w;
    waterLevel += rainAmount; //rain;
    waterLevel *= evaporationRate; //evaporate;
    //waterLevel = sqrt(waterLevel);
    //waterLevel = min(waterLevel,10.0); //prevent water from getting too high and fucking shit up
    
    //sediments
    float sedimentReceived = texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.yx).x //left pipe of right cell
                           + texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.zx).y //right pipe of left cell
                           + texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.xy).z //bottom pipe of top cell
                           + texelFetchOffset(sedimentPipes, fragCoord, 0, zeroOneMinusOne.xz).w; //top pipe of bottom cell
    //add recieved sediments to our height and sediment
    here.g += sedimentReceived;
    
    //debug
    //here.r += sedimentReceived;
    
    float sedimentsToGoIntoPipe = 0;
    
    float sedToGoRight = 0;
    float sedToGoLeft = 0;
    float sedToGoTop = 0;
    float sedToGoBottom = 0;
    
    /*
    float watToGoRight = max((heightHere - right) * 0.125, 0.0) * waterLevel;
    float watToGoLeft = max((heightHere - left) * 0.125, 0.0) * waterLevel;
    float watToGoTop = max((heightHere - top) * 0.125, 0.0) * waterLevel;
    float watToGoBottom = max((heightHere - bottom) * 0.125, 0.0) * waterLevel;
    */
    
    if(here.g > 0.0) //if we have enough sediments to erode it
    {
        //sedimentsToGoIntoPipe += waterLevel * clamp(howHigherWeAreThanLowestNeighbor * 0.1, 0.0, 0.5);
        //sedimentsToGoIntoPipe += clamp((howHigherWeAreThanLowestNeighbor - sedimentRateOfRepose) * 0.3, 0.0, 3.0);
        //here.g -= sedimentsToGoIntoPipe; //update sediment amount
        
        /*
        here.g -= sedimentsToGoIntoPipe * directionFactor.x; //update sediment amount
        here.g -= sedimentsToGoIntoPipe * directionFactor.y; //update sediment amount
        here.g -= sedimentsToGoIntoPipe * directionFactor.z; //update sediment amount
        here.g -= sedimentsToGoIntoPipe * directionFactor.w; //update sediment amount
         */
        
        //thermal
        /*
        sedToGoRight = clamp((heightHere - right - sedimentRateOfRepose) * 0.2, 0.0, 999);
        sedToGoLeft = clamp((heightHere - left - sedimentRateOfRepose) * 0.2, 0.0, 999);
        sedToGoTop = clamp((heightHere - top - sedimentRateOfRepose) * 0.2, 0.0, 999);
        sedToGoBottom = clamp((heightHere - bottom - sedimentRateOfRepose) * 0.2, 0.0, 999);
         */
        
        //hydraulic
        sedToGoRight += waterLevel * clamp((heightHere - right) * 0.1, 0.0, 0.1);
        sedToGoLeft += waterLevel * clamp((heightHere - left) * 0.1, 0.0, 0.1);
        sedToGoTop += waterLevel * clamp((heightHere - top) * 0.1, 0.0, 0.1);
        sedToGoBottom += waterLevel * clamp((heightHere - bottom) * 0.1, 0.0, 0.1);
        
        here.g -= sedToGoRight + sedToGoLeft + sedToGoTop + sedToGoBottom;
    }
    else //else we will erode rock
    {
        //sedimentsToGoIntoPipe += waterLevel * clamp(howHigherWeAreThanLowestNeighbor * 0.1, 0.0, 0.05);
        //sedimentsToGoIntoPipe += clamp((howHigherWeAreThanLowestNeighbor - rockRateOfRepose) * 0.3, 0.0, 999);
        //here.r -= sedimentsToGoIntoPipe; //update rock amount
        
        /*
        here.r -= sedimentsToGoIntoPipe * directionFactor.x; //update rock amount
        here.r -= sedimentsToGoIntoPipe * directionFactor.y; //update rock amount
        here.r -= sedimentsToGoIntoPipe * directionFactor.z; //update rock amount
        here.r -= sedimentsToGoIntoPipe * directionFactor.w; //update rock amount
         */
        
        //thermal
        /*
        sedToGoRight = clamp((heightHere - right - rockRateOfRepose) * 0.2, 0.0, 1.0);
        sedToGoLeft = clamp((heightHere - left - rockRateOfRepose) * 0.2, 0.0, 1.0);
        sedToGoTop = clamp((heightHere - top - rockRateOfRepose) * 0.2, 0.0, 1.0);
        sedToGoBottom = clamp((heightHere - bottom - rockRateOfRepose) * 0.2, 0.0, 1.0);
         */
        
        //hydraulic
        sedToGoRight += waterLevel * clamp((heightHere - right) * 0.1, 0.0, 0.1);
        sedToGoLeft += waterLevel * clamp((heightHere - left) * 0.1, 0.0, 0.1);
        sedToGoTop += waterLevel * clamp((heightHere - top) * 0.1, 0.0, 0.1);
        sedToGoBottom += waterLevel * clamp((heightHere - bottom) * 0.1, 0.0, 0.1);
        
        here.r -= sedToGoRight + sedToGoLeft + sedToGoTop + sedToGoBottom;
    }
    
    /*sedToGoRight = clamp((heightHere - right - rockRateOfRepose) * 0.2, 0.0, 999);
    sedToGoLeft = clamp((heightHere - left - rockRateOfRepose) * 0.2, 0.0, 999);
    sedToGoTop = clamp((heightHere - top - rockRateOfRepose) * 0.2, 0.0, 999);
    sedToGoBottom = clamp((heightHere - bottom - rockRateOfRepose) * 0.2, 0.0, 999);
    
    here.r -= sedToGoRight + sedToGoLeft + sedToGoTop + sedToGoBottom;
    */
    
    heightRocksSedimentsOut = here;
    waterPipesOut = directionFactor * waterLevel;
    //sedimentPipesOut = directionFactor * sedimentsToGoIntoPipe;
    
    //heightRocksSedimentsOut.g = max(0,heightRocksSedimentsOut.g);
    //waterPipesOut = clamp(waterPipesOut,0,1);
    //sedimentPipesOut = clamp(sedimentPipesOut,0,1);
    //waterPipesOut = vec4(watToGoLeft,watToGoRight,watToGoBottom,watToGoTop);
    //sedimentPipesOut = vec4(0);
    sedimentPipesOut = vec4(sedToGoLeft,sedToGoRight,sedToGoBottom,sedToGoTop);
}

