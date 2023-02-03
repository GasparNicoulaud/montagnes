#version 410

uniform sampler2D bhm;
uniform int seed;

layout(location = 0) out vec4 terrain; //rgb r rock, g water, b suspended sediments, a hardness
layout(location = 1) out vec4 waterOutflow;
layout(location = 2) out vec2 velocityField;

// Modulo 289, optimizes to code without divisions
float mod289f(float x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// Permutation polynomial (ring size 289 = 17*17)
float permutef(float x) {
    return mod289f(((x*34.0)+1.0)*x);
}

// Hashed 2-D gradients with an extra rotation.
// (The constant 0.0243902439 is 1/41)
vec2 rgrad2(vec2 p, float rot) {
    /*#if 0
     // Map from a line to a diamond such that a shift maps to a rotation.
     float u = permute(permute(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
     u = 4.0 * fract(u) - 2.0;
     // (This vector could be normalized, exactly or approximately.)
     return vec2(abs(u)-1.0, abs(abs(u+1.0)-2.0)-1.0);
     #else */
    // For more isotropic gradients, sin/cos can be used instead.
    float u = permutef(permutef(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
    u = fract(u) * 6.28318530718; // 2*pi
    return vec2(cos(u), sin(u));
    //#endif
}

//
// 2-D tiling simplex noise with rotating gradients,
// but without the analytical derivative.
//
float psrnoise(vec2 pos, vec2 per, float rot) {
    // Offset y slightly to hide some rare artifacts
    pos.y += 0.001;
    // Skew to hexagonal grid
    vec2 uv = vec2(pos.x + pos.y*0.5, pos.y);
    
    vec2 i0 = floor(uv);
    vec2 f0 = fract(uv);
    // Traversal order
    vec2 i1 = (f0.x > f0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    
    // Unskewed grid points in (x,y) space
    vec2 p0 = vec2(i0.x - i0.y * 0.5, i0.y);
    vec2 p1 = vec2(p0.x + i1.x - i1.y * 0.5, p0.y + i1.y);
    vec2 p2 = vec2(p0.x + 0.5, p0.y + 1.0);
    
    // Integer grid point indices in (u,v) space
    i1 = i0 + i1;
    vec2 i2 = i0 + vec2(1.0, 1.0);
    
    // Vectors in unskewed (x,y) coordinates from
    // each of the simplex corners to the evaluation point
    vec2 d0 = pos - p0;
    vec2 d1 = pos - p1;
    vec2 d2 = pos - p2;
    
    // Wrap i0, i1 and i2 to the desired period before gradient hashing:
    // wrap points in (x,y), map to (u,v)
    vec3 xw = mod(vec3(p0.x, p1.x, p2.x), per.x);
    vec3 yw = mod(vec3(p0.y, p1.y, p2.y), per.y);
    vec3 iuw = xw + 0.5 * yw;
    vec3 ivw = yw;
    
    // Create gradients from indices
    vec2 g0 = rgrad2(vec2(iuw.x, ivw.x), rot);
    vec2 g1 = rgrad2(vec2(iuw.y, ivw.y), rot);
    vec2 g2 = rgrad2(vec2(iuw.z, ivw.z), rot);
    
    // Gradients dot vectors to corresponding corners
    // (The derivatives of this are simply the gradients)
    vec3 w = vec3(dot(g0, d0), dot(g1, d1), dot(g2, d2));
    
    // Radial weights from corners
    // 0.8 is the square of 2/sqrt(5), the distance from
    // a grid point to the nearest simplex boundary
    vec3 t = 0.8 - vec3(dot(d0, d0), dot(d1, d1), dot(d2, d2));
    
    // Set influence of each surflet to zero outside radius sqrt(0.8)
    t = max(t, 0.0);
    
    // Fourth power of t
    vec3 t2 = t * t;
    vec3 t4 = t2 * t2;
    
    // Final noise value is:
    // sum of ((radial weights) times (gradient dot vector from corner))
    float n = dot(t4, w);
    
    // Rescale to cover the range [-1,1] reasonably well
    return 11.0*n;
}


//
// 2-D tiling simplex noise with fixed gradients,
// without the analytical derivative.
// This function is implemented as a wrapper to "psrnoise",
// at the minimal cost of three extra additions.
//
float snoise(vec2 pos) {
    return psrnoise(pos * 0.05, vec2(100,100), 0.0);
}

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

float FBMWithRotations(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    
    float lacunarity = 2.0;
    float persistence = 1.8;
    float retval = 0;
    for (int i = 0; i < 12; i++) {
        pos = rotate(pos,seed * pow(12.34,i));
        float noiseval = snoise(pos * pow(lacunarity,i));
        retval += noiseval / pow(persistence,i);
    }
    return retval;
}

float FBMWithRotationsLowPersitance(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    
    float lacunarity = 2.0;
    float persistence = 2.1;
    float retval = 0;
    for (int i = 0; i < 12; i++) {
        pos = rotate(pos,seed * pow(12.34,i));
        float noiseval = snoise(pos * pow(lacunarity,i));
        retval += noiseval / pow(persistence,i);
    }
    return retval;
}

float strataHardnessAt(float terrainHeight, float seed)
{
    float hardness = abs(FBMWithRotationsLowPersitance(vec2(terrainHeight) * 20.0,seed)) * 10.0;
    hardness = floor(hardness);
    hardness *= 0.8;
    return hardness;
}

float hardnessAt(vec2 fragCoord, float terrainHeight, int seed)
{
    float globalFreq = 3.0;
    float globalAmp = 1.0;
    
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,137.137);
    brseedAbs *= 0.01;
    
    float hardness = FBMWithRotations(fragCoord * globalFreq,brseedAbs) * globalAmp;
    
    //hardness = 1.0;
    
    float retval = clamp(hardness * 1.3 + 1.8, 1.5, 7.0); //probably well centered around the + offset
    
    //retval += abs(FBMWithRotations(fragCoord * 30,brseedAbs + 3) * 0.2);
    
    retval += max(0,abs((FBMWithRotations(fragCoord * 10,brseedAbs + 5) * 1.5)) - 1.2) * 2.5;
    retval += max(0,(FBMWithRotations(fragCoord * 50,brseedAbs + 5) * 2.0) - 1.5) * 2.0;
    retval += max(0,(FBMWithRotations(fragCoord * 100,brseedAbs + 10) * 3.5) - 2.0) * 1.5;

    //retval += 1.0;
    //retval /= 2.0;
    
    if(fragCoord.x < 8 || fragCoord.x > 2040 || fragCoord.y < 8 || fragCoord.y > 2040 )
    {
        retval = 10000.0;
    }
    //retval += strataHardnessAt(terrainHeight, brseedAbs + 2);
    //retval = 2.5;
    
    return retval; //probably well centered around the + offset
    //return 3.0;
}

void main(void)
{
    //int seed = 123;
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    float bhmHeight = texelFetch(bhm, fragCoord, 0).r;
    
    terrain.r = bhmHeight * 500.0; //rock
    terrain.r += 1000;
    terrain.g = 0.05; //water
    terrain.g = 0; //water
    terrain.b = 0; //suspended sediments
    
    //terrain.r = min(terrain.r, 1400);
    
    terrain.a = hardnessAt(gl_FragCoord.xy, terrain.r, seed); //hardness
    
    //terrain.r = hardnessAt(gl_FragCoord.xy, seed) * 100.0; //debug to show hardness
    
    waterOutflow = vec4(0);
    velocityField = vec2(0);
}

/*
 
 
 weird bug again
 

 
 
 
 
*/
