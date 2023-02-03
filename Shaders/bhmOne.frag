
#version 150 core

out vec4 out_Color;

uniform int seed;
uniform float noiseFactor;

precision highp float;
precision highp int;

// Simplex 2D noise
//
// Modulo 289, optimizes to code without divisions
vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// Permutation polynomial (ring size 289 = 17*17)
vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

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
// 2-D tiling simplex noise with rotating gradients and analytical derivative.
// The first component of the 3-element return vector is the noise value,
// and the second and third components are the x and y partial derivatives.
//
vec3 psrdnoise(vec2 pos, vec2 per, float rot) {
    // Hack: offset y slightly to hide some rare artifacts
    pos.y += 0.01;
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
    
    // Partial derivatives for analytical gradient computation
    vec3 dtdx = -2.0 * vec3(d0.x, d1.x, d2.x);
    vec3 dtdy = -2.0 * vec3(d0.y, d1.y, d2.y);
    
    // Set influence of each surflet to zero outside radius sqrt(0.8)
    if (t.x < 0.0) {
        dtdx.x = 0.0;
        dtdy.x = 0.0;
        t.x = 0.0;
    }
    if (t.y < 0.0) {
        dtdx.y = 0.0;
        dtdy.y = 0.0;
        t.y = 0.0;
    }
    if (t.z < 0.0) {
        dtdx.z = 0.0;
        dtdy.z = 0.0;
        t.z = 0.0;
    }
    
    // Fourth power of t (and third power for derivative)
    vec3 t2 = t * t;
    vec3 t4 = t2 * t2;
    vec3 t3 = t2 * t;
    
    // Final noise value is:
    // sum of ((radial weights) times (gradient dot vector from corner))
    float n = dot(t4, w);
    
    // Final analytical derivative (gradient of a sum of scalar products)
    vec2 dt0 = vec2(dtdx.x, dtdy.x) * 4.0 * t3.x;
    vec2 dn0 = t4.x * g0 + dt0 * w.x;
    vec2 dt1 = vec2(dtdx.y, dtdy.y) * 4.0 * t3.y;
    vec2 dn1 = t4.y * g1 + dt1 * w.y;
    vec2 dt2 = vec2(dtdx.z, dtdy.z) * 4.0 * t3.z;
    vec2 dn2 = t4.z * g2 + dt2 * w.z;
    
    return 11.0*vec3(n, dn0 + dn1 + dn2);
}

//
// 2-D non-tiling simplex noise with rotating gradients and analytical derivative.
// The first component of the 3-element return vector is the noise value,
// and the second and third components are the x and y partial derivatives.
//
vec3 srdnoise(vec2 pos, float rot) {
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
    
    vec3 x = vec3(p0.x, p1.x, p2.x);
    vec3 y = vec3(p0.y, p1.y, p2.y);
    vec3 iuw = x + 0.5 * y;
    vec3 ivw = y;
    
    // Avoid precision issues in permutation
    iuw = mod289(iuw);
    ivw = mod289(ivw);
    
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
    
    // Partial derivatives for analytical gradient computation
    vec3 dtdx = -2.0 * vec3(d0.x, d1.x, d2.x);
    vec3 dtdy = -2.0 * vec3(d0.y, d1.y, d2.y);
    
    // Set influence of each surflet to zero outside radius sqrt(0.8)
    if (t.x < 0.0) {
        dtdx.x = 0.0;
        dtdy.x = 0.0;
        t.x = 0.0;
    }
    if (t.y < 0.0) {
        dtdx.y = 0.0;
        dtdy.y = 0.0;
        t.y = 0.0;
    }
    if (t.z < 0.0) {
        dtdx.z = 0.0;
        dtdy.z = 0.0;
        t.z = 0.0;
    }
    
    // Fourth power of t (and third power for derivative)
    vec3 t2 = t * t;
    vec3 t4 = t2 * t2;
    vec3 t3 = t2 * t;
    
    // Final noise value is:
    // sum of ((radial weights) times (gradient dot vector from corner))
    float n = dot(t4, w);
    
    // Final analytical derivative (gradient of a sum of scalar products)
    vec2 dt0 = vec2(dtdx.x, dtdy.x) * 4.0 * t3.x;
    vec2 dn0 = t4.x * g0 + dt0 * w.x;
    vec2 dt1 = vec2(dtdx.y, dtdy.y) * 4.0 * t3.y;
    vec2 dn1 = t4.y * g1 + dt1 * w.y;
    vec2 dt2 = vec2(dtdx.z, dtdy.z) * 4.0 * t3.z;
    vec2 dn2 = t4.z * g2 + dt2 * w.z;
    
    return 11.0*vec3(n, dn0 + dn1 + dn2);
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

float simplefbmNoise(vec2 pos, float seed)
{
    float retval = 0;
    pos *= 0.0005;
    for (int i = 0; i < 15; i++) {
        float noiseval = psrnoise((pos + vec2(seed*0.001f + 1)) * (i + 1) * pow(2,i),vec2(100,100),seed * 0.0000001f * i);
        retval += (1.0 - abs(noiseval)) / pow(2,i);
    }
    return retval * 0.8;
}

float fbm(vec2 pos, float seed)
{
    pos += vec2(sin(seed * 5),cos(seed * -7)) * 100;
    float retval = 0;
    for (int i = 0; i < 15; i++) {
        float noiseval = psrnoise((pos + vec2(seed*0.001f + 1)) * (i + 1) * pow(2,i),vec2(100,100),seed * 0.0000001f * i);
        retval += noiseval / pow(2,i);
    }
    return retval + 1;
}

vec3 fbmd(vec2 pos, float seed)
{
    vec3 retval = vec3(0);
    for (int i = 0; i < 15; i++) {
        vec3 noiseval = psrdnoise((pos + vec2(seed*0.002f * 3)) * (i - 1) * pow(2,i),vec2(100,100),seed * 0.0000001f * i);
        retval += noiseval / pow(2,i);
    }
    return retval;
}

float noiseErosion(vec2 pos, float seed)
{
    float warpfactor = 0.1;
    float retval = 0;
    vec2 deriv;
    pos *= 0.005;
    
    //deriv = fbmd(pos,seed).yz;
    
    retval = sqrt(fbm(pos + fbmd(pos + fbmd(pos,seed * 1.5).yz * warpfactor,sqrt(seed)).yz * warpfactor,seed * 3));
    
    return sqrt(pow(retval,fbm(pos, 35.3 + seed))) * 0.8;
}

float basicFBM(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    float retval = 0;
    for (int i = 0; i < 15; i++) {
        float noiseval = snoise(pos * pow(2,i));
        retval += noiseval / pow(2,i);
    }
    return retval;
}

float billowFBM(vec2 pos, float seed)
{
    pos *= 0.05;
    pos += vec2(cos(seed * 2 + 1)- 13,sin(seed * -3) + 3) * 13;
    float retval = 0;
    for (int i = 0; i < 15; i++) {
        float noiseval = abs(snoise(pos * pow(2,i)));
        retval += noiseval / pow(2,i);
    }
    return retval;
}

vec3 perlinNoiseDeriv(vec2 pos, float seed)
{
    seed *= cos(seed) * 5;
    seed = fract(seed) * 4;
    pos += vec2(cos(seed),sin(seed));
    float theta = seed * 123;
    vec2 rotPos = vec2(pos.x * cos(theta) - pos.y * sin(theta),pos.y * cos(theta) + pos.x * sin(theta));// + vec2(cos(seed) * 12, sin(seed) + 5);
    rotPos -= vec2(sin(seed),cos(seed));
    rotPos.x *= 1.6; //corective stretch
    return srdnoise(rotPos,seed);
}

float swissTurbulence(vec2 p, float seed, float lac)
{
    seed = sin(seed) * 3;
    p += vec2(cos(seed),sin(seed));
    int octaves = 15;
    float lacunarity = lac;
    float gain = 1.0f/lacunarity;
    
    vec2 warp = vec2(0,0);
    
    float sum = 0;
    
    float freq = 1.0, amp = 1; // amp and freq are only iterators
    
    vec2 randomWarp = vec2(basicFBM(p* freq,seed + 3* pow(1,4)),basicFBM(p* freq,seed * 2* pow(1,5))) * 20;
    
    for(int i=0; i < octaves; i++)
    {
        vec3 n = perlinNoiseDeriv(p * freq + warp * freq + randomWarp * freq, seed * pow(i,3));
        sum += amp * (1 - abs(n.z));
        freq *= lacunarity;
        amp *= gain;
        
        warp += n.xy * 0.02 * amp;
    }
    
    return pow(sum + 5,2);
}

float noiseVariabe(vec2 p, float seed, float param)
{
    // seed integration
    seed = sin(seed) * 3;
    p += vec2(cos(seed),sin(seed));
    
    int octaves = 15;
    float lacunarity = param;
    float gain = 1.0f/lacunarity;
    
    vec2 warp = vec2(0,0);
    
    float sum = 0;
    
    float freq = 1.0, amp = 1; // amp and freq are only iterators
    
    vec2 randomWarp = warp;//vec2(basicFBM(p* freq,seed + 3* pow(1,4)),basicFBM(p* freq,seed * 2* pow(1,5))) * 20;
    
    for(int i=0; i < octaves; i++)
    {
        vec3 n = perlinNoiseDeriv((p + warp + randomWarp) * freq, seed * pow(i,3));
        sum += amp * (1 - abs(n.z));
        freq *= lacunarity;
        amp *= gain;
        
        //warp += n.xy * 0.02 * amp;
    }
    
    return pow(sum + 5,2);
}

//returns -1.0 if x < 0, and 1.0 if x >= 0
float signGreaterEqualZero(float x)
{
    return step(0, x)*2 - 1;
}

float noiseDistoMountain(vec2 p, float seed)
{
    //seed integration
    seed = cos(seed + 1) * 5;
    p += vec2(-cos(seed),+sin(seed));
    
    int octaves = 1;
    float lacunarity = 2.0f;
    float gain = 1.0f/lacunarity;
    
    vec2 warp = vec2(0,0);
    
    float sum = 0;
    
    float freq = 1.0, amp = 1; // amp and freq are only iterators
    
    vec2 randomWarp = warp;//vec2(basicFBM(p* freq,seed + 3* pow(1,4)),basicFBM(p* freq,seed * 2* pow(1,5))) * 20;
    
    for(int i=0; i < octaves; i++)
    {
        vec3 n = perlinNoiseDeriv(p * freq,seed * pow(i,3));
        float projectedadd = clamp((1 - abs(n.z)),0,1);
        n.xy *= -signGreaterEqualZero(n.z);
        //float oldz = n.z;
        warp = n.xy * 0.04 * amp * max(sin(projectedadd * 3.1415),0);
        vec3 o = perlinNoiseDeriv((p + warp) * freq,seed * pow(i,3));
        sum += amp * (1 - abs(o.z));
        freq *= lacunarity;
        amp *= gain;
    }
    
    return sum + 100;
}

const mat2 m = mat2(0.8,-0.6,0.6,0.8);

float iqnoise(vec2 p)
{
    float a = 0.0;
    float b = 1.0;
    vec2  d = vec2(0.0);
    for( int i=0; i<15; i++ )
    {
        vec3 n = srdnoise(p,10.f);
        d += n.yz;
        a += b*n.x/(1.0+dot(d,d));
        b *= 0.5;
        p = m*p*2.0;
    }
    return abs(a) + 1;
}

float normalBoosted(vec2 p, float seed)
{
    //seed integration
    seed = cos(seed - 1) * 2;
    p += vec2(cos(seed) * 3,sin(seed) * 4);
    
    float retval = 0;
    float freq = 10.0;
    float gain = 1.0;
    float prevLayerNormMag = 0;
    for( int i=0; i<15; i++ )
    {
        vec3 n = srdnoise(p * freq,10.f);
        //prevLayerNormMag += length(n.yz) * gain;
        retval += n.x * gain;// * prevLayerNormMag;
        freq *= 2;
        gain *= length(n.yz) * 0.2;
        
    }
    return retval + 1;
}

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

float normalBoostedv2(vec2 p, float seed)
{
    //seed integration
    seed = cos(seed - 1) * 2;
    
    //p += vec2(basicFBM(p, seed + 3),basicFBM(p, seed * 3)) * 10;
    
    float retval = 0;
    float freq = 10.0;
    float gain = 1.0;
    float prevLayerNormMag = 0;
    for( int i=0; i<15; i++ )
    {
        
        p = rotate(p,seed + i);
        p += vec2(cos(seed + i) * 3,sin(seed - i) * 4);
        
        vec3 n = srdnoise(p * freq,10.f * i);
        //prevLayerNormMag += length(n.yz) * gain;
        retval += (1 - abs(n.x)) * gain;// * prevLayerNormMag;
        freq *= 2.2;
        gain *= pow(length(n.yz),2) * 0.1;
        gain = min(gain,15.0f/freq);
    }
    return retval + -0.5;
}

float julia(vec2 p)
{
    vec2 c = vec2(0.001,0.76); //vec2(-0.12,0.89);
    float seuil = 5;
    int imax = 50;
    vec2 tmp = p;
    
    for( int i=0; i<imax; i++ )
    {
        tmp = vec2(tmp.x * tmp.x - tmp.y * tmp.y + c.x, 2 * tmp.x * tmp.y + c.y);
        
        if(length(tmp) > seuil)
        {
            return i;
        }
    }
    
    return imax;
}

#define PI 3.14159265358979


vec2 hash( in vec2 x )
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

// from https://www.shadertoy.com/view/XdXBRH
//name:Noise - Gradient - 2D - Deriv
//Author: iq
// return gradient noise (in x) and its derivatives (in yz)
vec3 noised( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );
    
    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    vec2 du = 30.0*f*f*(f*(f-2.0)+1.0);
    
    vec2 ga = hash( i + vec2(0.0,0.0) );
    vec2 gb = hash( i + vec2(1.0,0.0) );
    vec2 gc = hash( i + vec2(0.0,1.0) );
    vec2 gd = hash( i + vec2(1.0,1.0) );
    
    float va = dot( ga, f - vec2(0.0,0.0) );
    float vb = dot( gb, f - vec2(1.0,0.0) );
    float vc = dot( gc, f - vec2(0.0,1.0) );
    float vd = dot( gd, f - vec2(1.0,1.0) );
    
    return vec3( va + u.x*(vb-va) + u.y*(vc-va) + u.x*u.y*(va-vb-vc+vd),   // value
                ga + u.x*(gb-ga) + u.y*(gc-ga) + u.x*u.y*(ga-gb-gc+gd) +  // derivatives
                du * (u.yx*(va-vb-vc+vd) + vec2(vb,vc) - va));
}

// code adapted from https://www.shadertoy.com/view/llsGWl
// name: Gavoronoise
// author: guil
//Code has been modified to return analytic derivatives and to favour
//direction quite a bit.
vec3 erosion(in vec2 p, vec2 dir) {
    vec2 ip = floor(p);
    vec2 fp = fract(p);
    float f = 2.*PI;
    vec3 va = vec3(0.0);
    float wt = 0.0;
    for (int i=-2; i<=1; i++) {
        for (int j=-2; j<=1; j++) {
            vec2 o = vec2(i, j);
            vec2 h = hash(ip - o)*0.5;
            vec2 pp = fp +o - h;
            float d = dot(pp, pp);
            float w = exp(-d*2.0);
            wt +=w;
            float mag = dot(pp,dir);
            va += vec3(cos(mag*f), -sin(mag*f)*(pp+dir))*w;
        }
    }
    return va/wt;
}

//This is where the magic happens
vec3 mountain(vec2 p, float s) {
    //First generate a base heightmap
    //it can be based on any type of noise
    //so long as you also generate normals
    //Im just doing basic FBM based terrain using
    //iq's analytic derivative gradient noise
    vec3 n = vec3(0.0);
    float nf = 1.0;
    float na = 0.6;
    for (int i=0;i<3;i++) {
        n+= noised(p*s*nf)*na*vec3(1.0, nf, nf);
        na *= 0.5;
        nf *= 2.0;
    }
    
    //take the curl of the normal to get the gradient facing down the slope
    vec2 dir = n.zy*vec2(1.0, -1.0);
    
    //Now we compute another fbm type noise
    // erosion is a type of noise with a strong directionality
    //we pass in the direction based on the slope of the terrain
    //erosion also returns the slope. we add that to a running total
    //so that the direction of successive layers are based on the
    //past layers
    vec3 h = vec3(0.0);
    float a = 0.7*(smoothstep(0.3, 0.5,n.x*0.5+0.5)); //smooth the valleys
    float f = 1.0;
    for (int i=0;i<5;i++) {
        h+= erosion(p*f, dir+h.zy*vec2(1.0, -1.0))*a*vec3(1.0, f, f);
        a*=0.4;
        f*=2.0;
    }
    //remap height to [0,1] and add erosion
    //looks best when erosion amount is small
    //not sure about adding the normals together, but it looks okay
    return vec3(smoothstep(-1.0, 1.0, n.x)+h.x*0.05, (n.yz+h.yz)*0.5+0.5);
}

//This is where the magic happens
vec3 mountainClayGaspar(vec2 p, float s) {
    //First generate a base heightmap
    //it can be based on any type of noise
    //so long as you also generate normals
    //Im just doing basic FBM based terrain using
    //iq's analytic derivative gradient noise
    vec3 n = vec3(0.0);
    float nf = 1.3;
    float na = 1.0;
    for (int i=0;i<3;i++) {
        n+= noised(p*s*nf)*na*vec3(1.0, nf, nf);
        na *= 0.5;
        nf *= 2.0;
    }
    n.x += 0.2;
    
    //(ff)' =  f f’ + f’ f
    n.yz = n.x * n.yz * 1.5;
    n.x = pow( n.x ,1.5);
    
    n *= 1.0;
    
    //take the curl of the normal to get the gradient facing down the slope
    vec2 dir = n.zy*vec2(1.0, -1.0);
    
    //Now we compute another fbm type noise
    // erosion is a type of noise with a strong directionality
    //we pass in the direction based on the slope of the terrain
    //erosion also returns the slope. we add that to a running total
    //so that the direction of successive layers are based on the
    //past layers
    vec3 h = vec3(0.0);
    float a = 0.7*(smoothstep(0.3, 0.5,n.x*0.5+0.35)); //smooth the valleys , second term after + determine height of valleys
    float f = 1.0;
    for (int i=0;i<15;i++) {
        h+= erosion(p*f, dir+h.zy*vec2(1.0, -1.0))*a*vec3(1.0, f, f);
        a*=0.45;
        f*=2.0;
    }
    //remap height to [0,1] and add erosion
    //looks best when erosion amount is small
    //not sure about adding the normals together, but it looks okay
    return vec3(smoothstep(-1.0, 1.0, n.x)+h.x*0.05, (n.yz+h.yz)*0.5+0.5);
}

vec3 aioBaseNoise(vec2 p, float seed, int iter)
{
    p += vec2(seed * 1.23, 4.0 - seed);
    
    float s = 0.1;
    vec3 n = vec3(0.0);
    float nf = 2.0;
    float na = 1.0;
    for (int i=0;i<iter;i++) {
        n+= noised(p*s*nf)*na*vec3(1.0, nf, nf);
        na *= 0.5;
        nf *= 2.0;
    }
    
    //n.yz = n.yz * n.x * 2.0;
    //n.x = n.x * n.x;
    
    p+= vec2(seed,-seed);
    
    vec3 n2 = vec3(0.0);
    nf = 0.5;
    na = 0.5;
    for (int i=0;i<iter;i++) {
        n2+= noised(p*s*nf)*na*vec3(1.0, nf, nf);
        na *= 0.5;
        nf *= 2.0;
    }
    
    n.x += n2.x;
    
    n.yz += n2.yz;
    

    //n.x += baseFactor;
    //n.yz *= 0;
    
    return n * 0.4;
}

float aioSprinklenoise(vec2 p, float seed)
{
    p += vec2(seed * 0.3, 2.0 - seed);
    
    float s = 0.1;
    vec3 n = vec3(0.0);
    float nf = 10.0;
    float na = 1.0;
    for (int i=0;i<20;i++) {
        n+= noised(p*s*nf)*na*vec3(1.0, nf, nf);
        na *= 0.9;
        nf *= 1.2;
    }
    
    n.x = n.x * n.x * n.x * n.x;
    
    n *= 0.1;
    
    n.x  = smoothstep(-2.0,0.4,n.x);
    
    n.x -= 1.0;
    
    return n.x;
}

float allinoneTerrain(vec2 p, float seed)
{
    p += vec2(seed * 1.23, seed + 3.21);
    p *= 0.01;
    
    vec3 basenoise = aioBaseNoise(p,seed,3);
    
    //take the curl of the normal to get the gradient facing down the slope
    vec2 dir = basenoise.zy*vec2(1.0, -1.0);
    
    vec3 h = vec3(0.0);
    float a = 0.7*(smoothstep(0.3, 0.5,basenoise.x*0.5+0.5)); //smooth the valleys
    float f = 1.0;
    for (int i=0;i<5;i++) {
        h+= erosion(p*f, dir+h.zy*vec2(1.0, -1.0))*a*vec3(1.0, f, f);
        a*=0.4;
        f*=2.0;
    }
    
    vec3 retval = vec3(basenoise.x+ abs(h.x) *0.05, (basenoise.yz+h.yz)*0.5+0.5);
    
    retval += aioBaseNoise(p,seed,15);
    
    retval.x = smoothstep(-0.2, 10.0,retval.x) * 30.0;
    
    retval += aioBaseNoise(p,seed,15) * 0.2;
    
    retval.x += aioSprinklenoise(p,seed) * max(1.0 - retval.x * 10.0,0) * 0.3;
    
    //attempt at a base ground wide noise, n2 should do it but doesnt...
    float baseFactor = abs(basicFBM(p * 100,seed)) + abs(basicFBM(p * 100,seed + 10.0)) + abs(basicFBM(p * 100,seed - 10.0));
    baseFactor = sqrt(sqrt(sqrt(baseFactor)))* 0.7;
    retval.x *= 0.4;
    retval.x += baseFactor - 0.3;
    
    return retval.x;
}

float smax(float x, float y, float k) //softmax
{
    return log( exp(x*k) + exp(y*k) )/k;
}

void main(void)
{
    //out_Color = vec4(noiseDistoMountain(gl_FragCoord.xy * 0.0005,seed)) * 0.03;
    
    //noise ok for erosion but really not much elese
    /*float noise = inoutNF * noiseErosion(vec2(gl_FragCoord.x,gl_FragCoord.y),seed);
    float factor = pow(fbm(vec2(gl_FragCoord.x,gl_FragCoord.y) * 0.0001,seed + 13) * 0.5f,2.0f);
    out_Color = vec4(noise * factor + pow(factor,2));*/
    
    
     // noise billow when low, ridged when high
    /*float baseNoise = basicFBM(vec2(gl_FragCoord.x,gl_FragCoord.y), seed); // [-1 , 1]
    float billow = billowFBM(vec2(gl_FragCoord.x,gl_FragCoord.y), seed * 7 - 15); //  [0 , 1]
    float ridged = 1 - abs(basicFBM(vec2(gl_FragCoord.x,gl_FragCoord.y), seed * 3 - 5)); // [0 , 1]
    out_Color = vec4((baseNoise * 0.2 + (baseNoise * billow * -0.05)) + 3); // ajoute du  ridged si basenois > 0 et du billow si basenois < 0*/

    
    //out_Color = vec4(basicFBM(gl_FragCoord.xy * 2,seed));
    
    
    // the goood noise nice disto ridges
    //out_Color = vec4(swissTurbulence(gl_FragCoord.xy * 0.0005,seed,2.1)) * 0.03;
    
    
    // the mountaneous large disto ridges       [Current default]
    //out_Color = vec4(swissTurbulence(gl_FragCoord.xy * 0.00005,seed,2.1) + swissTurbulence(gl_FragCoord.xy * 0.00005,seed * 3,2.1) * 0.1 - 10) * 0.2;
    
    
    // the same with lower lacunarity noise nice disto ridges
    //out_Color = vec4(swissTurbulence(gl_FragCoord.xy * 0.0005,seed,2.1) + swissTurbulence(gl_FragCoord.yx * 0.0005,seed * 2,2.1) + swissTurbulence(gl_FragCoord.yx * 0.0005,seed * 3,2.1)) * 0.01;
    
    //another good noise but weird
    //float param = pow(basicFBM(gl_FragCoord.xy,seed * 3 + 1),2) * 5;
    //param += 7;
    //out_Color = vec4(noiseVariabe(gl_FragCoord.xy * 0.0005,seed,param)) * 0.03; //*/
    
    //out_Color = vec4(param * 0.2);
    //out_Color = vec4(inoutNF * fbm(vec2(gl_FragCoord.x,gl_FragCoord.y) * 0.0001,seed + 13)) * 0.3;
    
    //iqnoise
    //out_Color = vec4(iqnoise(gl_FragCoord.xy * 0.0005) * 3);
    
    //normal boosted noise
    //out_Color = vec4(normalBoosted(gl_FragCoord.xy * 0.00004,seed - 5)) * 1;
    
    //normal boosted noise v2, interesting without any erosion, large mountain chains
    //vec2 warp =  0.0005 * vec2(basicFBM(gl_FragCoord.xy * 2,seed * 3),basicFBM(gl_FragCoord.xy * 2,seed + 2));
    //out_Color = vec4(normalBoostedv2(gl_FragCoord.xy * 0.00007 + warp,seed - 5)) * 1.3;
    
    
   // float expNoise = basicFBM(gl_FragCoord.xy * 2,seed * 3) * basicFBM(gl_FragCoord.xy * 2,seed - 13) * basicFBM(gl_FragCoord.xy * 2,seed + 13) * basicFBM(gl_FragCoord.xy * 2,seed * -100);
    //expNoise = abs(expNoise);
    
    /*//clyajohn noise !!
    float seedf = float(seed) * 0.01;
    vec2 seedVec2 = vec2(fract(seedf),fract(sin(seedf)));
    float expNoise = mountainClayGaspar((gl_FragCoord.xy * 0.005) + seedVec2, 0.1).x * 4.0;
    
    expNoise -= 2.8;
    
    expNoise *= 0.7;
    
    //expNoise = max(-0.7,expNoise);
    
    expNoise += 0.7;*/
    
    /*//Gaspar clyajohn Remix noise
    float seedf = float(seed) * 0.001;
    vec2 seedVec2 = vec2(fract(seedf),fract(sin(seedf))) * 100.0;
    float expNoise = mountainClayGaspar((gl_FragCoord.xy * 0.007) + seedVec2, 0.1).x * 4.0;
    
    expNoise = min(expNoise,2.6);
    
    //expNoise =2.0;
    
    expNoise -= 2.8;
    
    expNoise *= 0.7;
    
    //expNoise = max(-0.7,expNoise);
    
    expNoise += 0.7;*/
    
    /*// bedrockNoise
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,1234.5);
    brseedAbs *= 0.01;
    float brexpNoise = sqrt(abs(basicFBM(gl_FragCoord.xy * 15 + basicFBM(gl_FragCoord.xy * 15,brseedAbs - 3),brseedAbs * 3)*0.5))* 0.05;
    float expNoise = 0;
    expNoise += brexpNoise;
     */
     //expNoise -= 2.0;
     
     //expNoise *= 0.7;
     
     //expNoise = max(-0.7,expNoise);
    
    //plateaus
    /*
    float seedf = seed * 0.01;
    float seedAbs = abs(seedf);
    seedAbs = mod(seedAbs,1234.5);
    seedAbs *= 0.01;
    float expNoise = mountain((gl_FragCoord.xy * 0.02)  + vec2(seedAbs * 2.0, seedAbs + 3.0), 0.1).x * 6.0;
    expNoise -= 1.0;
    expNoise *= 0.2;
    expNoise = min(expNoise + basicFBM(gl_FragCoord.xy * 2,seed + 12) * 0.02,0.5) + basicFBM(gl_FragCoord.xy * 20,seed * 3) * 0.02 ;
    expNoise = sqrt(expNoise);
    */
    
    /*//simplish perlin noise
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,1234.5);
    brseedAbs *= 0.01;
    float expNoise = abs(basicFBM(gl_FragCoord.xy * 2 + basicFBM(gl_FragCoord.xy * 15,brseedAbs - 3),brseedAbs * 3)*0.5)* 1.0;
    //*/
    
    /*//aoi noise
    float seedf = seed * 0.01;
    float seedAbs = abs(seedf);
    seedAbs = mod(seedAbs,1234.5);
    seedAbs *= 0.01;
    float expNoise = allinoneTerrain(gl_FragCoord.xy * 0.5,seedAbs) * 2.0;
    expNoise -= 1.0;
     */
    
    //*//clawmark noise
    float globalFreq = 1.5;
    float globalAmp = 0.3;
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
    //seed2fZeroToTwoPi = seedfZeroToTwoPi + 1.6;
    float noiseFrequency2 = (((seed * seed) % 1000) + 500) * 0.001 * globalFreq * 0.2; // should be a parameter between 0.1 and 1.5
    float stretchFactor2 = 0.5; // should be a parameter between 0.5 and 2.5
    vec2 clawMarkDir2 = vec2(10.0 * stretchFactor2,1.0) * noiseFrequency2;

    float secondaryClaw = abs(basicFBM(rotate(gl_FragCoord.xy,seed2fZeroToTwoPi) * clawMarkDir2,brseedAbs * -3 + 12));
    
    secondaryClaw = sqrt(secondaryClaw);
    
    expNoise = sqrt(expNoise);
    expNoise = min(secondaryClaw,expNoise) * globalAmp;
    //*/
    
    
    //debug line :
    //expNoise = length(gl_FragCoord.xy) * 0.004;
    
    
    out_Color = vec4(expNoise,expNoise,expNoise,1.0);
    out_Color.g = 0; //set sediments channel to zero
 
    //out_Color = vec4(basicFBM(gl_FragCoord.xy * 5,seed * 3)) * 0.5;
    //out_Color = vec4(julia((gl_FragCoord.xy - vec2(1024)) / 500.0)) * 0.005; // julia set
    //out_Color.a = 140.0f;//length(warp) * 10000000;//param * 0.2;
}















