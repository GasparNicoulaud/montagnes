#version 150 core

out vec4 out_Color;

uniform int seed;
uniform float globalFrequency;
uniform float globalAmplification;

precision highp float;
precision highp int;

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


float basicFBM(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    float retval = 0;
    for (int i = 0; i < 8; i++) {
        float noiseval = snoise(pos * pow(2,i));
        retval += noiseval / pow(2,i);
    }
    return retval;
}

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

void main(void)
{
    float globalFreq = globalFrequency; //1.5;
    float globalAmp = globalAmplification; //0.3;
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,1234.5);
    brseedAbs *= 0.01;
    float seedfZeroToTwoPi = (seed % 1000) * 0.001 * 6.2831;
    float noiseFrequency = ((seed % 1000) + 500) * 0.001 * globalFreq; // should be a parameter between 0.1 and 1.5
    float stretchFactor = 0.5; // should be a parameter between 0.5 and 2.5
    vec2 clawMarkDir = vec2(10.0 * stretchFactor,1.0) * noiseFrequency;


    float noiseWarping = basicFBM(gl_FragCoord.xy * 10,brseedAbs - 3);
    float expNoise = abs(basicFBM(rotate(gl_FragCoord.xy,seedfZeroToTwoPi) * clawMarkDir + noiseWarping,brseedAbs * 3));

    float seed2fZeroToTwoPi = ((seed + 123) % 200) * 0.005 * 3.1415;
    float noiseFrequency2 = (((seed * seed) % 1000) + 500) * 0.001 * globalFreq * 0.5; // should be a parameter between 0.1 and 1.5
    float stretchFactor2 = 0.5; // should be a parameter between 0.5 and 2.5
    vec2 clawMarkDir2 = vec2(10.0 * stretchFactor2,1.0) * noiseFrequency2;

    float secondaryClaw = abs(basicFBM(rotate(gl_FragCoord.xy,seed2fZeroToTwoPi) * clawMarkDir2,brseedAbs * -3 + 12));

    secondaryClaw = sqrt(secondaryClaw);

    expNoise = sqrt(expNoise);
    expNoise = min(secondaryClaw,expNoise) * globalAmp;

    out_Color = vec4(expNoise,0,0,1.0);
    out_Color.g = 0; //set sediments channel to zero
 
    //out_Color = vec4(basicFBM(gl_FragCoord.xy * 5,seed * 3)) * 0.5;
    //out_Color = vec4(julia((gl_FragCoord.xy - vec2(1024)) / 500.0)) * 0.005; // julia set
    //out_Color.a = 140.0f;//length(warp) * 10000000;//param * 0.2;
}
