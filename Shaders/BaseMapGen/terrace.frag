#version 150 core

out vec4 out_Color;

uniform sampler2D bhm;

uniform float terraceAmount;
uniform float seed;

vec2 hash( in vec2 x )  // replace this by something better
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
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
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 100;
    
    float lacunarity = 2.0;
    float retval = 0;
    for (int i = 0; i < 8; i++) {
        pos = rotate(pos,seed * pow(12.34,i));
        float noiseval = snoise(pos * pow(lacunarity,i));
        retval += noiseval / pow(lacunarity,i);
    }
    return retval;
}

void main(void)
{
    float invTextureSize = 1.0/2048.0;
    vec4 bhmValue = texture(bhm, gl_FragCoord.xy * invTextureSize).rgba;
    float orgiHeight = bhmValue.r;
    float noiseHeight = abs(FBMWithRotations(gl_FragCoord.xy * 0.2,seed) * 1.5);
    
    float terraceFreq = terraceAmount;
    
    float _floor = -5.0;
    
    vec2 rng = hash(vec2(seed,-seed));
    
    bhmValue.r += noiseHeight;
    for (int i = 0; i < 2048; i++) {
        _floor += rng.x / terraceFreq;
        rng = hash(rng);
        if(bhmValue.r < _floor){
            bhmValue.r = _floor;
            break;
        }
    }
    bhmValue.r -= noiseHeight;
    
//    float top = texture(bhm, (gl_FragCoord.xy + vec2(0,1)) * invTextureSize).r;
//    float bottom = texture(bhm, (gl_FragCoord.xy + vec2(0,-1)) * invTextureSize).r;
//    float right = texture(bhm, (gl_FragCoord.xy + vec2(1,0)) * invTextureSize).r;
//    float left = texture(bhm, (gl_FragCoord.xy + vec2(-1,0)) * invTextureSize).r;
//    float slope = sqrt((top-bottom)*(top-bottom) + (right-left)*(right-left)) * 128.0;
    
    /*
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-3,-1)) * invTextureSize).r) * 1;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-3, 0)) * invTextureSize).r) * 2;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-3, 1)) * invTextureSize).r) * 1;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2,-2)) * invTextureSize).r) * 3;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2,-1)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2, 0)) * invTextureSize).r) * 5;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2, 1)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2, 2)) * invTextureSize).r) * 3;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1,-3)) * invTextureSize).r) * 1;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1,-2)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1,-1)) * invTextureSize).r) * 6;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 0)) * invTextureSize).r) * 7;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 1)) * invTextureSize).r) * 6;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 2)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 3)) * invTextureSize).r) * 1;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0,-3)) * invTextureSize).r) * 2;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0,-2)) * invTextureSize).r) * 5;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0,-1)) * invTextureSize).r) * 7;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0, 1)) * invTextureSize).r) * 7;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0, 2)) * invTextureSize).r) * 5;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0, 3)) * invTextureSize).r) * 2;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1,-3)) * invTextureSize).r) * 1;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1,-2)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1,-1)) * invTextureSize).r) * 6;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 0)) * invTextureSize).r) * 7;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 1)) * invTextureSize).r) * 6;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 2)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 3)) * invTextureSize).r) * 1;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2,-2)) * invTextureSize).r) * 3;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2,-1)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2, 0)) * invTextureSize).r) * 5;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2, 1)) * invTextureSize).r) * 4;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2, 2)) * invTextureSize).r) * 3;
     
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 3,-1)) * invTextureSize).r) * 1;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 3, 0)) * invTextureSize).r) * 2;
     slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 3, 1)) * invTextureSize).r) * 1;
     */
    
    float slope = 0;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-3,-1)) * invTextureSize).r) * 1;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-3, 0)) * invTextureSize).r) * 2;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-3, 1)) * invTextureSize).r) * 1;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2,-2)) * invTextureSize).r) * 3;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2,-1)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2, 0)) * invTextureSize).r) * 5;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2, 1)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-2, 2)) * invTextureSize).r) * 3;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1,-3)) * invTextureSize).r) * 1;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1,-2)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1,-1)) * invTextureSize).r) * 6;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 0)) * invTextureSize).r) * 7;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 1)) * invTextureSize).r) * 6;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 2)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2(-1, 3)) * invTextureSize).r) * 1;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0,-3)) * invTextureSize).r) * 2;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0,-2)) * invTextureSize).r) * 5;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0,-1)) * invTextureSize).r) * 7;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0, 1)) * invTextureSize).r) * 7;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0, 2)) * invTextureSize).r) * 5;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 0, 3)) * invTextureSize).r) * 2;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1,-3)) * invTextureSize).r) * 1;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1,-2)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1,-1)) * invTextureSize).r) * 6;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 0)) * invTextureSize).r) * 7;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 1)) * invTextureSize).r) * 6;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 2)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 1, 3)) * invTextureSize).r) * 1;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2,-2)) * invTextureSize).r) * 3;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2,-1)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2, 0)) * invTextureSize).r) * 5;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2, 1)) * invTextureSize).r) * 4;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 2, 2)) * invTextureSize).r) * 3;
    
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 3,-1)) * invTextureSize).r) * 1;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 3, 0)) * invTextureSize).r) * 2;
    slope += abs(orgiHeight - texture(bhm, (gl_FragCoord.xy + vec2( 3, 1)) * invTextureSize).r) * 1;
    slope *= 0.1;
    slope -= 0.2;
    
    bhmValue.r = mix(orgiHeight,bhmValue.r,clamp(slope,0.2,0.7));
    
    out_Color = bhmValue;
}



