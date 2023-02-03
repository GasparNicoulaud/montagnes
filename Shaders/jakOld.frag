#version 410 core

#extension GL_ARB_explicit_attrib_location : require

precision lowp float;
precision lowp int;
precision lowp uint;

uniform usampler2D terrain;
uniform usampler2D pipes;
uniform float lod;

layout(location = 0) out uvec4 terrainOut;
layout(location = 1) out uvec4 pipesOut;

/*
uvec4 encodePipes(uint sedPipes[8])
{
    uvec4 retval;
    retval.z = sedPipes[0] + (sedPipes[1] << 8) + (sedPipes[2] << 16) + (sedPipes[3] << 24);
    retval.w = sedPipes[4] + (sedPipes[5] << 8) + (sedPipes[6] << 16) + (sedPipes[7] << 24);
    return retval;
}
 */

float decodeOne(uint data, uint index) // decode a 32b unsigned int into four 8b unsigend ints and return the one queried
{
    return (data >> index * 8U) & 255U; //sometimes index is >= 4 and then we read data from the next channel, but it works fine ??? no
}

vec3 decodeNeighbor(uvec4 ter, uvec4 pipes, uint pipeIndex) // last param is index of the pipes that poinnts towards us
{
    vec3 tx;
    tx.rg = ter.rg;
    
    if(pipeIndex < 4U)
    {
        tx.b = decodeOne(pipes.z,pipeIndex);
    }
    else
    {
        tx.b = decodeOne(pipes.w,pipeIndex - 4U);
    }
    
    return tx;
}

float solidHeight(vec3 tx)
{
    return tx.r + tx.g;
}

void main(void)
{
    vec4 locTer; //local terrain r : rock g : sediments b : water, a : rock hardness
    uint sedPipes[8]; //our sedpipes
    vec3 nb[8];  // neighbors 0-3 are adjacent, 4-7 are corners, r : rock g : sed b : sed pipe toward us
    
    vec2 normFragCoord = gl_FragCoord.xy * 0.00048828125; //   =  1 / 2048
    const vec3 normZeroOneMinusOne = vec3(0,0.00048828125,-0.00048828125); // 0, 1, -1 normalized
    
    //read input
    
    //decode ourself
    locTer = texture(terrain ,normFragCoord);
    
    nb[0] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.xy + normFragCoord),texture(pipes, normZeroOneMinusOne.xy + normFragCoord),2U); // top
    nb[1] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.yx + normFragCoord),texture(pipes, normZeroOneMinusOne.yx + normFragCoord),3U); // right
    nb[2] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.xz + normFragCoord),texture(pipes, normZeroOneMinusOne.xz + normFragCoord),0U); // bottom
    nb[3] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.zx + normFragCoord),texture(pipes, normZeroOneMinusOne.zx + normFragCoord),1U); // left
    nb[4] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.yy + normFragCoord),texture(pipes, normZeroOneMinusOne.yy + normFragCoord),6U); // top-right
    nb[5] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.zy + normFragCoord),texture(pipes, normZeroOneMinusOne.zy + normFragCoord),7U); // bottom-right
    nb[6] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.zz + normFragCoord),texture(pipes, normZeroOneMinusOne.zz + normFragCoord),4U); // bottom-left
    nb[7] = decodeNeighbor(texture(terrain, normZeroOneMinusOne.yz + normFragCoord),texture(pipes, normZeroOneMinusOne.yz + normFragCoord),5U); // top-left
    
    
    // thermal erosion :
    int i;
    
    //float sedToAdd = 0;
    
    float sedToAdd = nb[0].b + nb[1].b + nb[2].b + nb[3].b + nb[4].b + nb[5].b + nb[6].b + nb[7].b; //receive from neighbor pipes
    
    float HmaxDiff = 0; // height difference between the current cell and its lowest neighbor
    float sumPositiveEnoughHeightDiffs = 0;
    float heightDiffsOverDstMinusTalusPos[8]; //heigh diff vetween us and ith cell divided by dst and minus talus angle and max(x,0)
    
    float usSolidHeightMinRateOfTalus = locTer.r + locTer.g - max(5,140 - (locTer.g + sedToAdd) * 1.5);
    
    //float OneOversqrt2 = 0.7071067814;
    
    sumPositiveEnoughHeightDiffs = heightDiffsOverDstMinusTalusPos[0] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[0]),0);
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[1] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[1]),0);
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[2] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[2]),0);
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[3] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[3]),0);
    
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[4] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[4]),0) * 0.7071067814;
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[5] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[5]),0) * 0.7071067814;
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[6] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[6]),0) * 0.7071067814;
    sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[7] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[7]),0) * 0.7071067814;
     
    /*
    HmaxDiff = max(heightDiffsOverDstMinusTalusPos[0],
                   max(heightDiffsOverDstMinusTalusPos[1],
                       max(heightDiffsOverDstMinusTalusPos[2],
                           max(heightDiffsOverDstMinusTalusPos[3],
                               max(heightDiffsOverDstMinusTalusPos[4],
                                   max(heightDiffsOverDstMinusTalusPos[5],
                                       max(heightDiffsOverDstMinusTalusPos[6],
                                               heightDiffsOverDstMinusTalusPos[7])))))));
     */
    
    /*
    HmaxDiff = max(heightDiffsOverDstMinusTalusPos[0],heightDiffsOverDstMinusTalusPos[1]);
    HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[2]);
    HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[3]);
    HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[4]);
    HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[5]);
    HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[6]);
    HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[7]);
     */
    
    
    for(i = 0; i < 8; i++)
    {
        //sumPositiveEnoughHeightDiffs += heightDiffsOverDstMinusTalusPos[i] = max(usSolidHeightMinRateOfTalus - solidHeight(nb[i]),0);
        HmaxDiff = max(HmaxDiff,heightDiffsOverDstMinusTalusPos[i]);
    }
    
    // dividend minimum is 2.0, if incresed it reduces amount of matter transfered per step, 5.0 looks good
    float deltaSOverSPEHD = HmaxDiff / ((1.5 + lod) * sumPositiveEnoughHeightDiffs);// volume to be moved, hardness needs to be added here multipliyng this by localHardness()
    
    float totalSedSentInPipes = 0;
    
     for(i = 0; i < 8; i++)
     {
         totalSedSentInPipes += sedPipes[i] = uint(min(deltaSOverSPEHD * heightDiffsOverDstMinusTalusPos[i],255)); //compressed pipeload
         //totalSedSentInPipes += sedPipes[i];
     }
    
    
    //float totalSedSentInPipes = sedPipes[0] + sedPipes[1] + sedPipes[2] + sedPipes[3] + sedPipes[4] + sedPipes[5] + sedPipes[6] + sedPipes[7];
    
    locTer.g += sedToAdd - totalSedSentInPipes;
    
    // if we dig to much sediment we need to to dig it from rock instead
    float toRemove = min(0, locTer.g); // always negative
    locTer.r += toRemove * 0.8; //addition but negative
    locTer.g -= toRemove; // reset us.sedheight to zero if it was negative before
    
    terrainOut = uvec4(locTer);
    
    pipesOut.z = sedPipes[0] + (sedPipes[1] << 8) + (sedPipes[2] << 16) + (sedPipes[3] << 24);
    pipesOut.w = sedPipes[4] + (sedPipes[5] << 8) + (sedPipes[6] << 16) + (sedPipes[7] << 24);
    //pipesOut = encodePipes(sedPipes);
}
