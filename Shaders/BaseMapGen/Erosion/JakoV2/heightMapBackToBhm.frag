#version 410

uniform sampler2D terrain; //rgb r rock, g water, b suspended sediments
uniform sampler2D bhmCopy; //copy of bhm

out vec4 bhm;

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    float erodedHeight = texelFetch(terrain, fragCoord, 0).r - 1000;
    //erodedHeight += texelFetch(terrain, fragCoord, 0).g * 0.5; //add some of the water to the height
    //erodedHeight += texelFetch(terrain, fragCoord, 0).g * 1.0; //add ALL of the water to the height

    float water = texelFetch(terrain, fragCoord, 0).g;
    water = max(0,water);
    
    float bhmHeight = texelFetch(bhmCopy, fragCoord, 0).r;
    
    erodedHeight /= 500.0;
    float sediments = max(0,erodedHeight - bhmHeight);// + bhm.g;
    //sediments += max(0,water - 1.5);
    
    
    bhm = vec4(erodedHeight, sediments,0,0);
    
    if(texelFetch(terrain, fragCoord, 0).a < 0)
    {
        bhm.r = 3.0;
    }
    
    if(isnan(bhm.r))
    {
        bhm.r = 4.0;
    }
    if(isinf(bhm.r))
    {
        bhm.r = -2.0;
    }
    
    
    if(isnan(water) || isinf(water))
    {
        bhm.g = 0;
        bhm.r = 0.0;
    }
    
    if(texelFetch(terrain, fragCoord, 0).g < 0)
    {
        bhm.r = -3.0;
    }
}
/*
 
 this weird bug again
 
 

 
 
 */
