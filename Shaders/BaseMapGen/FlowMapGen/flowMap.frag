#version 410 core

uniform int frameCount;

uniform usampler2D heightMap; // R 16ui height
uniform usampler2D flowMap;  //GL_RGB16UI, R for current water, G for flow trace accumulator, B for chosen direction of flow

out uvec4 flowMapOut;

ivec2 decodeDir(uint dir)
{
    if(dir == 0U)
    {
        return ivec2(1,1);
    }
    if(dir == 1U)
    {
        return ivec2(1,0);
    }
    if(dir == 2U)
    {
        return ivec2(1,-1);
    }
    if(dir == 3U)
    {
        return ivec2(0,-1);
    }
    if(dir == 4U)
    {
        return ivec2(-1,-1);
    }
    if(dir == 5U)
    {
        return ivec2(-1,0);
    }
    if(dir == 6U)
    {
        return ivec2(-1,1);
    }
    return ivec2(0,1);
}

uint encodeDir(ivec2 dir)
{
    if(dir == ivec2(1,1))
    {
        return 0U;
    }
    if(dir == ivec2(1,0))
    {
        return 1U;
    }
    if(dir == ivec2(1,-1))
    {
        return 2U;
    }
    if(dir == ivec2(0,-1))
    {
        return 3U;
    }
    if(dir == ivec2(-1,-1))
    {
        return 4U;
    }
    if(dir == ivec2(-1,0))
    {
        return 5U;
    }
    if(dir == ivec2(-1,1))
    {
        return 6U;
    }
    return 7U;
}

//receive flow from a neighbor at offset
uint receiveFrom(ivec2 coord, ivec2 offset)
{
    ivec3 neighborToRecieveFrom = ivec3(texelFetch(flowMap, coord + offset, 0).xyz);
    ivec2 neighborsChosenDirection = decodeDir(uint(neighborToRecieveFrom.z));
    
    int minusOne = -1;
    
    if(offset == (neighborsChosenDirection * minusOne))
    {
        return uint(neighborToRecieveFrom.x);
    }
    return 0U;
}

float randomOneLiner(vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898,78.233)))* 43758.5453123);
}

uint chooseFlowDir(ivec2 coord)
{
    uint ourHeight = texelFetch(heightMap, coord, 0).x;
    float diffs[8];
    //compute the difference in height between us and the 8 neighbors
    for(int i=0;i<8;i++)
    {
        float randomizer = 0.0;
        
        
        if(frameCount % 8 == i)
        {
            randomizer = 10000.0 * sin(frameCount); //Remember to re enbale it maybe
        }
        
        
        //float randomizer = (randomOneLiner(vec2(coord + vec2(i,frameCount)) * 0.01) - 0.5) * 10000.0;
        
        float distanceBetweenUsAndNeighbor_i = length(decodeDir(uint(i)));

        
        float neighbor_iHeight = float(texelFetch(heightMap, coord + decodeDir(uint(i)), 0).x);
        
        diffs[i] = (float(ourHeight) + randomizer - neighbor_iHeight) / distanceBetweenUsAndNeighbor_i;
    }
    uint maxdir = 8U;
    float maxdiff = 0;
    //find the lowest neighbor
    for(uint i=0U;i<8U;i++)
    {
        if(diffs[i] > maxdiff)
        {
            maxdir = i;
            maxdiff = diffs[i];
        }
    }
    
    return maxdir; //maxdir will contain 8U if all neigbors were higher than us so no flow, chosen direction else
}


void main(void)
{
    
    ivec2 iCoord = ivec2(gl_FragCoord.xy); //cast to int

    //recuperer from neigbors
    uint rcvd = 0;/*receiveFrom(iCoord,decodeDir(0U)) +
    receiveFrom(iCoord,decodeDir(1U)) +
    receiveFrom(iCoord,decodeDir(2U)) +
    receiveFrom(iCoord,decodeDir(3U)) +
    receiveFrom(iCoord,decodeDir(4U)) +
    receiveFrom(iCoord,decodeDir(5U)) +
    receiveFrom(iCoord,decodeDir(6U)) +
    receiveFrom(iCoord,decodeDir(7U));*/
    
    //rcvd = 0;
    rcvd += receiveFrom(iCoord,ivec2(1,1));
    rcvd += receiveFrom(iCoord,ivec2(1,0));
    rcvd += receiveFrom(iCoord,ivec2(1,-1));
    rcvd += receiveFrom(iCoord,ivec2(0,-1));
    rcvd += receiveFrom(iCoord,ivec2(-1,-1));
    rcvd += receiveFrom(iCoord,ivec2(-1,0));
    rcvd += receiveFrom(iCoord,ivec2(-1,1));
    rcvd += receiveFrom(iCoord,ivec2(0,1));
    //rcvd = 0;
    
    
    //accumuler dans channel G
    uint Gchannel = texelFetch(flowMap, iCoord, 0).y + rcvd;
    //Gchannel = uint(float(texelFetchOffset(heightMap, iCoord + decodeDir(uint(0)), 0, ivec2(0)).x) * 0.00065);
    
    //rain
    rcvd += 1U;
    
    
    //choose flow direction
    uint chosenDir = chooseFlowDir(iCoord);
    
    //chosenDir = ivec2(0);
    
    //if no adjacent cell was lower than us, there is no flow, water value is reset to zero
    if(chosenDir == 8U)
    {
        rcvd = 0;
        chosenDir = 0U;
    }
    
    flowMapOut = ivec4(rcvd,Gchannel,chosenDir,1);
}









