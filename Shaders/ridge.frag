#version 410 core

out uvec4 out_Color;
uniform float seed;

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

float noiseSeeded(vec2 co, float seed)
{
    seed = fract(sin(seed)) * 12.36345;
    co *= 0.05;
    co += vec2(3 * seed, 12 - seed);
    co = fract(co) * 21.345;
    
    return snoise(co);
}

float randO(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898 + seed,78.233 - seed))) * 43758.5453);
}

float rand2(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(rand(co.xy) * 100,rand(co.yx) * 100))) * 438.53);
}

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    uint fout = 9U;
    
    if(rand2(gl_FragCoord.xy - ivec2(seed + 2, seed * 3)) > 0.02)
    {
        fout = 0U;
    }
    
    if(rand2(gl_FragCoord.xy + ivec2(seed * 23, seed + 16)) > 0.05)
    {
        fout = 0U;
    }
    
    vec2 ofst = vec2(noiseSeeded(gl_FragCoord.xy * 0.002,seed * 3), noiseSeeded(gl_FragCoord.xy * 0.002,seed - 3)) * 800;
    
    if(noiseSeeded((gl_FragCoord.xy + ofst) * 0.0010 ,seed) > -0.6) //[-1 to 1]
    {
        fout = 0U;
    }
    out_Color = uvec4(fout);
}







