#version 150 core

uniform usampler2D terrain;//RG16UI //r is height, g is hardness
out uvec4 out_Color; //pipes en RGB32UI // 8 * 8bits + B for height transfer

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    //check surrounding terrain
    uint us =       texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xx).r; // middle
    uint hardness = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xx).g; // middle

    //neighbors
    uint nb1 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xy).r; // top
    uint nb2 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yx).r; // right
    uint nb3 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.xz).r; // bottom
    uint nb4 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zx).r; // left
    
    uint nb5 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yy).r; // top-right
    uint nb6 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.yz).r; // bottom-right
    uint nb7 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zz).r; // bottom-left
    uint nb8 = texelFetchOffset(terrain, fragCoord, 0, zeroOneMinusOne.zy).r; // top-left
    
    //determine where to flow, accounting from hardness
    
    uint pipe1 = 0U;
    uint pipe2 = 0U;
    uint pipe3 = 0U;
    uint pipe4 = 0U;
    uint pipe5 = 0U;
    uint pipe6 = 0U;
    uint pipe7 = 0U;
    uint pipe8 = 0U;
    
    ///TODO
    ///TODO
    ///TODO
    
    hardness += 40U;
    hardness = uint(float(hardness) / 4.0);
    
    //hardness = 100U;
    
    pipe1 = uint(max(float(us) - float(nb1) - float(hardness),0.0f));
    pipe2 = uint(max(float(us) - float(nb2) - float(hardness),0.0f));
    pipe3 = uint(max(float(us) - float(nb3) - float(hardness),0.0f));
    pipe4 = uint(max(float(us) - float(nb4) - float(hardness),0.0f));

    pipe5 = uint(max(float(us) - float(nb5) - float(hardness)* 1.4142135624,0.0f));
    pipe6 = uint(max(float(us) - float(nb6) - float(hardness)* 1.4142135624,0.0f));
    pipe7 = uint(max(float(us) - float(nb7) - float(hardness)* 1.4142135624,0.0f));
    pipe8 = uint(max(float(us) - float(nb8) - float(hardness)* 1.4142135624,0.0f));
    /*
    if(float(us) - float(nb1) > float(hardness))
    {
        pipe1 = uint((float(us) - float(nb1)) * 0.1);
    }
    if(float(us) - float(nb2) > float(hardness))
    {
        pipe2 = uint((float(us) - float(nb2)) * 0.1);
    }
    if(float(us) - float(nb3) > float(hardness))
    {
        pipe3 = uint((float(us) - float(nb3)) * 0.1);
    }
    if(float(us) - float(nb4) > float(hardness))
    {
        pipe4 = uint((float(us) - float(nb4)) * 0.1);
    }
    if(float(us) - float(nb5) > float(hardness) * 1.4142135624)
    {
        pipe5 = uint((float(us) - float(nb5)) * 0.1);
    }
    if(float(us) - float(nb6) > float(hardness) * 1.4142135624)
    {
        pipe6 = uint((float(us) - float(nb6)) * 0.1);
    }
    if(float(us) - float(nb7) > float(hardness) * 1.4142135624)
    {
        pipe7 = uint((float(us) - float(nb7)) * 0.1);
    }
    if(float(us) - float(nb8) > float(hardness) * 1.4142135624)
    {
        pipe8 = uint((float(us) - float(nb8)) * 0.1);
    }
     */
    
    // this step normalize the pipes 'vector' but it shouldnt, the total amount should be 0.5*max height diff, not a fixed value like it is now
    // not done, but maybe not ideal
    float maxPipe = max(max(max(max(max(max(max(pipe1,pipe2),pipe3),pipe4),pipe5),pipe6),pipe7),pipe8);
    
    float pipeSum = pipe1 + pipe2 + pipe3 + pipe4 + pipe5 + pipe6 + pipe7 + pipe8;
    pipeSum /= maxPipe * 0.5;
    
    pipe1 = uint(float(pipe1) / pipeSum);
    pipe2 = uint(float(pipe2) / pipeSum);
    pipe3 = uint(float(pipe3) / pipeSum);
    pipe4 = uint(float(pipe4) / pipeSum);
    pipe5 = uint(float(pipe5) / pipeSum);
    pipe6 = uint(float(pipe6) / pipeSum);
    pipe7 = uint(float(pipe7) / pipeSum);
    pipe8 = uint(float(pipe8) / pipeSum);

    
    
    //encode flow
    uint pipeR = pipe1 + (pipe2 + (pipe3 + (pipe4) * 256U) * 256U) * 256U;
    uint pipeG = pipe5 + (pipe6 + (pipe7 + (pipe8) * 256U) * 256U) * 256U;
    
    //output
    out_Color = uvec4(pipeR,pipeG,us,0U);
}
