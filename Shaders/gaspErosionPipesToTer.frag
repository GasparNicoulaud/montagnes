#version 150 core

uniform usampler2D pipes;//RGB32UI RG for pipe data, B = terrain height transfer from previous state
out uvec4 out_Color; //terrain en RG16UI //bitmask so only r channel does anything

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    // read from outgoing pipes
    uvec3 ourPipes = texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xx).rgb; // middle
    uint prevHeight = ourPipes.b;
    
    uint sentPipe1 = ourPipes.r % 256U;
    uint sentPipe2 = (ourPipes.r % 65536U)/256U;
    uint sentPipe3 = (ourPipes.r % 16777216U)/65536U;
    uint sentPipe4 = ourPipes.r / 16777216U;
    
    uint sentPipe5 = ourPipes.g % 256U;
    uint sentPipe6 = (ourPipes.g % 65536U)/256U;
    uint sentPipe7 = (ourPipes.g % 16777216U)/65536U;
    uint sentPipe8 = ourPipes.g / 16777216U;
    
    uint totalSentInPipes = sentPipe1 + sentPipe2 + sentPipe3 + sentPipe4 + sentPipe5 + sentPipe6 + sentPipe7 + sentPipe8;

    // remove outgoing pipes content from us
    prevHeight -= totalSentInPipes;
    
    // read from incoming pipes
    uint rcvdPipe1 = texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xz).r % 256U; // from bottom
    uint rcvdPipe2 = (texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zx).r % 65536U)/256U; // from left
    uint rcvdPipe3 = (texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.xy).r % 16777216U)/65536U; // from top
    uint rcvdPipe4 = texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yx).r / 16777216U; // from right
    
    uint rcvdPipe5 = texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zz).g % 256U; // from bottom-left
    uint rcvdPipe6 = (texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.zy).g % 65536U)/256U; // from top-left
    uint rcvdPipe7 = (texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yy).g % 16777216U)/65536U; // from top-right
    uint rcvdPipe8 = texelFetchOffset(pipes, fragCoord, 0, zeroOneMinusOne.yz).g / 16777216U; // from bottom-right
    
    uint totalRcvdInPipes = rcvdPipe1 + rcvdPipe2 + rcvdPipe3 + rcvdPipe4 + rcvdPipe5 + rcvdPipe6 + rcvdPipe7 + rcvdPipe8;
    
    // add from incoming pipes
    out_Color = uvec4(prevHeight + totalRcvdInPipes);
}
