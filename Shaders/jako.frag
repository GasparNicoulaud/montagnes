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

float decodeOne(uint data, uint index) // decode a 32b unsigned int into four 8b unsigend ints and return the one queried
{
    return (data >> index * 8U) & 255U; //sometimes index is >= 4 and then we read data from the next channel, but it works fine ??? no
}

float solidHeight(vec4 tx)
{
    return tx.r + tx.g;
}

void main(void)
{
    vec4 locTer; //local terrain r : rock g : sediments b : water, a : rock hardness
    float pipeCompression = 2.0 - (lod/4.5);
    
    vec4 nbSHA, nbSHC; // neighbors solid Heights
    vec4 nbPA, nbPC; // neigors pipes Adjacent and Corners
    
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    const vec4 one = vec4(1);
    
    //decode ourself
    locTer = texelFetch(terrain, fragCoord, 0);
    
    //decode neighbors solidheight and pipes
    nbSHA.x = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xy)); // top
    nbSHA.y = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yx)); // right
    nbSHA.z = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xz)); // bottom
    nbSHA.w = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zx)); // left
    
    nbSHC.x = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yy)); // top-right
    nbSHC.y = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zy)); // bottom-right
    nbSHC.z = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zz)); // bottom-left
    nbSHC.w = solidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yz)); // top-left
    
    nbPA.x = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xy).z,2U); // top
    nbPA.y = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yx).z,3U); // right
    nbPA.z = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xz).z,0U); // bottom
    nbPA.w = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zx).z,1U); // left
    
    nbPC.x = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yy).w,2U); // top-right
    nbPC.y = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zy).w,3U); // bottom-right
    nbPC.z = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zz).w,0U); // bottom-left
    nbPC.w = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yz).w,1U); // top-left
    
    nbPA *= pipeCompression;
    nbPC *= pipeCompression;

    // thermal erosion :
    float sedToAdd = dot(nbPA,one) + dot(nbPC,one); //receive from neighbor pipes
    
    float usSolidHeightMinRateOfTalus = locTer.r + locTer.g - max(0,locTer.a - (locTer.g + sedToAdd) * 3); //hardness needs to be added here multipliyng this by localHardness()
    
    vec4 hdA = max(usSolidHeightMinRateOfTalus - nbSHA,0);
    vec4 hdC = max(usSolidHeightMinRateOfTalus - nbSHC,0) * 0.7071067814;
    
    // dividend minimum is 2.0, if incresed it reduces amount of matter transfered per step, 5.0 looks good
    //volume to move
    float deltaSOverSPEHD = max(max(max(max(max(max(max(hdA.x,hdA.y),hdA.z),hdA.w),hdC.x),hdC.y),hdC.z),hdC.w) / ((1.5 + lod) * (dot(hdA,one) + dot(hdC,one)));

    uvec4 sedPipeAdja = uvec4(min((deltaSOverSPEHD / pipeCompression) * hdA,255));
    uvec4 sedPipeCorn = uvec4(min((deltaSOverSPEHD / pipeCompression) * hdC,255));

    float totalSedSentInPipes = (dot(sedPipeAdja,one) + dot(sedPipeCorn,one)) * pipeCompression;
    
    locTer.g += sedToAdd - totalSedSentInPipes;
    
    // if we dig to much sediment we need to to dig it from rock instead
    float toRemove = min(0, locTer.g); // always negative
    locTer.r += toRemove * 1.5;// * 0.8; //addition but negative
    locTer.g -= toRemove; // reset us.sedheight to zero if it was negative before
    
    locTer.g -= min(floor(0.005 * locTer.g), 1);
    //locTer.g += 1;
    
    terrainOut = uvec4(locTer);
    
    //encode pipes out
    const uvec4 shifts = uvec4(0U,8U,16U,24U);
    sedPipeAdja <<= shifts;
    sedPipeCorn <<= shifts;
    pipesOut.z = sedPipeAdja.x + sedPipeAdja.y + sedPipeAdja.z + sedPipeAdja.w;
    pipesOut.w = sedPipeCorn.x + sedPipeCorn.y + sedPipeCorn.z + sedPipeCorn.w;
}








