#version 150 core

uniform sampler2D bhm;
out ivec4 out_Color; //8bit uint
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

float fbm(vec2 co, float seed)
{
    float retval = 0;
    float freq = 1.0;
    for(int i = 0; i < 15; i++)
    {
        seed *= seed;
        retval += noiseSeeded(co * freq, seed)/freq;
        freq *= 2;
    }
    
    return retval;
}

float hardnessAt(float height, float seed)
{
    float array[50];
    
    for(int i = 0; i < 50; i++)
    {
        array[i] = clamp(abs(fbm(vec2(sin(i*13.12) * 23.4 * (seed + 3),i*12.13) - seed,seed + 23)),0,1.0);
    }
    
    height *= 10;
    height += 12;
    uint h1 = uint(height);
    h1 += 23U;
    h1 *= h1 + 132U;
    h1 %= 50U;
    
    float retval = array[h1];
    
    return retval;
}


void main(void)
{
    float terrainHeight = texture(bhm,gl_FragCoord.xy/2048.0f).r;
    
    float terrainHeightPlusOffset = terrainHeight
    + fbm(gl_FragCoord.xy/1000.0,seed * seed) * 0.3
    + fbm(gl_FragCoord.xy/50.0,seed + 14) * 0.05
    + fbm(gl_FragCoord.xy/10.0,seed - 14) * 0.03;
    
    //terrainHeightPlusOffset = terrainHeight;
    
    float hardnessNoise = (0.5 - abs(fbm(gl_FragCoord.xy/10.0,seed * -3))) * 0.3 + fbm(gl_FragCoord.xy/50.0,seed + 32) * 0.3 + fbm(gl_FragCoord.xy/25.0,seed - 24) * 0.3;
    
    //hardnessNoise = 0.0;
    //hardnessNoise = (-0.5 + sqrt(sqrt(abs(fbm(gl_FragCoord.xy/100.0,seed * -3))))) * 0.3 + sqrt(abs(fbm(gl_FragCoord.xy/100.0,seed + 32))) * 0.3 + fbm(gl_FragCoord.xy/50.0,seed - 24) * 0.3;
    
    float seedAbs = abs(seed) * 10.0;
    seedAbs = mod(seedAbs,10.0);
    
    float hardnessFloat = hardnessAt(terrainHeightPlusOffset,seedAbs) * 0.9 + pow(hardnessNoise,2) * 0.6;
    
    uint hardness = clamp(uint(255.0f * hardnessFloat),0U,255U);
    
    
    out_Color = ivec4(hardness);
}



/*
 
 
 
 padding
 
 
 
 
 */

