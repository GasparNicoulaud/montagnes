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

float liquidHeight(vec4 tx)
{
    return tx.r + tx.g + tx.b;
}

void main(void)
{
    vec4 locTer; //local terrain r : rock g : sediments b : water, a : rock hardness
    
    vec4 nbSHA, nbSHC; // neighbors solid Heights
    vec4 nbPA, nbPC; // neigors pipes Adjacent and Corners
    
    uvec4 sedPipeAdja, sedPipeCorn;
    
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
    
    //hydro erosion
    //(1)
    locTer.b += 1; //rain
    
    //(3)
    vec4 liquidDHA, liquidDHC; // liquid delta height
    liquidDHA.x = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xy)); // top
    liquidDHA.y = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yx)); // right
    liquidDHA.z = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xz)); // bottom
    liquidDHA.w = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zx)); // left
    
    liquidDHC.x = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yy)); // top-right
    liquidDHC.y = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zy)); // bottom-right
    liquidDHC.z = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zz)); // bottom-left
    liquidDHC.w = liquidHeight(locTer) - liquidHeight(texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yz)); // top-left
    
    //(2)
    vec4 watPipeAdja, watPipeCorn; //outflow
    watPipeAdja = max(liquidDHA, 0);
    watPipeCorn = max(liquidDHC * 0.7071067814, 0);
    
    //(4)
    float K = max(1,locTer.b/(dot(watPipeAdja,one) + dot(watPipeCorn,one)));
    //(5)
    watPipeAdja *= K;
    watPipeCorn *= K; //maybe we should clamp component so they stay under 255 for storing in pipe textures
    
    //(6)
    vec4 watnbPA, watnbPC; // water neigors pipes Adjacent and Corners
    watnbPA.x = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xy).x,2U); // top
    watnbPA.y = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yx).x,3U); // right
    watnbPA.z = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xz).x,0U); // bottom
    watnbPA.w = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zx).x,1U); // left
    
    watnbPC.x = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yy).y,2U); // top-right
    watnbPC.y = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zy).y,3U); // bottom-right
    watnbPC.z = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zz).y,0U); // bottom-left
    watnbPC.w = decodeOne(texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yz).y,1U); // top-left
    //(7)
    locTer.b += dot(watnbPA,one) + dot(watnbPC,one) - (dot(watPipeAdja,one) + dot(watPipeCorn,one));
    
    //(8)
    vec2 outVel = (watPipeAdja.x * zeroOneMinusOne.xy + watPipeAdja.y * zeroOneMinusOne.yx + watPipeAdja.z * zeroOneMinusOne.xz + watPipeAdja.w * zeroOneMinusOne.zx +
     watPipeCorn.x * watPipeCorn.xy + watPipeCorn.y * watPipeCorn.yx + watPipeCorn.z * watPipeCorn.xz + watPipeCorn.w * watPipeCorn.zx);
    vec2 inVel = (watnbPA.x * zeroOneMinusOne.xy + watnbPA.y * watnbPA.yx + watPipeAdja.z * watnbPA.xz + watPipeAdja.w * watnbPA.zx +
                   watnbPC.x * watPipeCorn.xy + watnbPC.y * watPipeCorn.yx + watnbPC.z * watPipeCorn.xz + watnbPC.w * watPipeCorn.zx) * -1;
    vec2 velocity = (outVel + inVel) * 0.0005;
    
    //(9)
    float C = length(velocity) * clamp(locTer.b,0,100);//sediment carry capacity
    
    float sedDeltaDeposition;

    sedDeltaDeposition = ((500 - C) * locTer.b) * 0.000005;

    float pipeRate = 0;
    sedPipeAdja.x = uint(max(0,dot(velocity,zeroOneMinusOne.xy)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    sedPipeAdja.y = uint(max(0,dot(velocity,zeroOneMinusOne.yx)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    sedPipeAdja.z = uint(max(0,dot(velocity,zeroOneMinusOne.xz)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    sedPipeAdja.w = uint(max(0,dot(velocity,zeroOneMinusOne.zx)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    
    sedPipeCorn.x = uint(max(0,dot(velocity,zeroOneMinusOne.yy)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    sedPipeCorn.y = uint(max(0,dot(velocity,zeroOneMinusOne.zy)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    sedPipeCorn.z = uint(max(0,dot(velocity,zeroOneMinusOne.zz)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    sedPipeCorn.w = uint(max(0,dot(velocity,zeroOneMinusOne.yz)) * max(0,sedDeltaDeposition * -1) * pipeRate);
    
    locTer.b *= 0.9;

    // thermal erosion :
    float sedToAdd = dot(nbPA,one) + dot(nbPC,one); //receive from neighbor pipes
    
    float usSolidHeightMinRateOfTalus = locTer.r + locTer.g - max(5,locTer.a - (locTer.g + sedToAdd) * 1.5); //hardness needs to be added here multipliyng this by localHardness()
    
    vec4 hdA = max(usSolidHeightMinRateOfTalus - nbSHA,0);
    vec4 hdC = max(usSolidHeightMinRateOfTalus - nbSHC,0) * 0.7071067814;
    
    // dividend minimum is 2.0, if incresed it reduces amount of matter transfered per step, 5.0 looks good
    //volume to move
    float deltaSOverSPEHD = max(max(max(max(max(max(max(hdA.x,hdA.y),hdA.z),hdA.w),hdC.x),hdC.y),hdC.z),hdC.w) / ((1.5 + lod) * (dot(hdA,one) + dot(hdC,one)));
    
    sedPipeAdja = uvec4(min(deltaSOverSPEHD * hdA + sedPipeAdja,255));
    sedPipeCorn = uvec4(min(deltaSOverSPEHD * hdC + sedPipeCorn,255));
    
    float totalSedSentInPipes = dot(sedPipeAdja,one) + dot(sedPipeCorn,one);
    
    locTer.g += sedToAdd - totalSedSentInPipes + sedDeltaDeposition;
    
    // if we dig to much sediment we need to to dig it from rock instead
    float toRemove = min(0, locTer.g); // always negative
    locTer.r += toRemove;// * 0.8; //addition but negative
    locTer.g -= toRemove; // reset us.sedheight to zero if it was negative before
    
    terrainOut = uvec4(locTer);
    
    //encode pipes out
    const uvec4 shifts = uvec4(0U,8U,16U,24U);
    
    uvec4 uwata = uvec4(watPipeAdja);
    uvec4 uwatc = uvec4(watPipeCorn);
    uwata <<= shifts;
    uwatc <<= shifts;
    pipesOut.x = uwata.x + uwata.y + uwata.z + uwata.w;
    pipesOut.y = uwatc.x + uwatc.y + uwatc.z + uwatc.w;
    
    sedPipeAdja <<= shifts;
    sedPipeCorn <<= shifts;
    pipesOut.z = sedPipeAdja.x + sedPipeAdja.y + sedPipeAdja.z + sedPipeAdja.w;
    pipesOut.w = sedPipeCorn.x + sedPipeCorn.y + sedPipeCorn.z + sedPipeCorn.w;
}








