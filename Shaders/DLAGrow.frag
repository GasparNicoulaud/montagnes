#version 410 core

out uvec4 out_Color;
uniform usampler2D sampler;
uniform float time;

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
//

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    
    // Other corners
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    
    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    
    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
    
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    
    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    
    // Compute final noise value at P
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float rand2(vec2 co, float seed)
{
    seed = fract(sin(seed)) * 12.36345;
    co *= 0.01;
    co += vec2(3 * seed, 12 - seed);
    co = fract(co) * 21.345;
    
    return fract(snoise(co) * 12.987);
}

uint checkSurroundings(const ivec3 zeroOneMinusOne, ivec2 fragCoord)
{
    uint nbc = 0;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,7)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,6)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,5)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,4)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,3)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-7,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 7,2)).r & 1U;
    
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-7,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 7,1)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-7,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,0)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 7,0)).r & 1U;
    
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-7)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-7)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-6)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-6)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-5)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,-5)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,-4)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,-4)).r & 1U;
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,-3)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,-3)).r & 1U;
   
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-7,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,-2)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 7,-2)).r & 1U;
    
    
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-7,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-6,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 5,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 6,-1)).r & 1U;
    nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 7,-1)).r & 1U;
    
    return nbc;
}

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    uint uoutput = 0U;

    
    uint us = texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xx).r;
    
    if((us & 1U) == 1U) //ridge
    {
        uoutput = us;
    }
    else if(us == 2U) //we are a moving particle
    {
        uint neighboursCount = 0U; //neighbors that are ridges
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xy).r & 1U; // top
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yx).r & 1U; // right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xz).r & 1U; // bottom
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zx).r & 1U; // left
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yy).r & 1U; // top-right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zy).r & 1U; // bottom-right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zz).r & 1U; // bottom-left
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yz).r & 1U; // top-left
        if(neighboursCount == 1U) //if we have at least one neighbor ridge, become a ridge
        {
            uoutput = 9U;
        }
        else if (neighboursCount == 0U) // if not we flag ourselves as a leaving particle
        {
            uoutput = 4U;
        }
        
    }
    else if(us == 0U) //no particle and no ridge here
    {
        //should we welcome a new particle from a neigbor moving ?
        uint neighboursCount = 0U; //neighbors that are leaving particle
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xy).r & 4U; // top
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yx).r & 4U; // right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xz).r & 4U; // bottom
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zx).r & 4U; // left
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yy).r & 4U; // top-right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zy).r & 4U; // bottom-right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zz).r & 4U; // bottom-left
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yz).r & 4U; // top-left
        if(neighboursCount > 0U)
        {
            float probOfTurningOn = (neighboursCount / 4U) * 0.125;
            if(rand2(gl_FragCoord.xy,time) < probOfTurningOn)
            {
                uoutput = 2U; //new  moving particle
            }
        }
        else if(checkSurroundings(zeroOneMinusOne,fragCoord) == 0)// no moving neighbors, we should check surrounding and have a prob of popping if no ridge in sight
        {
            uoutput = 2U; //new  moving particle
        }
    }
    out_Color = uvec4(uoutput);
}




// code for rad 4 circel
/*
 
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,4)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,4)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,4)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,3)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,2)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,1)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,0)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,0)).r & 1U;
 
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-4)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-4)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-4)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-3)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-3)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-2)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-2)).r & 1U;
 
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 0,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 1,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 2,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 3,-1)).r & 1U;
 nbc += texelFetchOffset(sampler, fragCoord, 0, ivec2( 4,-1)).r & 1U;
 
 */

