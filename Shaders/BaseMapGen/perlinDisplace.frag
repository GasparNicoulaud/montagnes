#version 150 core

out vec4 out_Color;

uniform sampler2D bhm;

uniform int seed;
uniform float amplitude;
uniform float frequency;
uniform float flipHeightMap;

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
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
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

float basicFBM(vec2 pos, float seed)
{
    pos *= 0.1;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    float retval = 0;
    float amp = 1;
    float freq = 1;
    float lac = 2;
    for (int i = 0; i < 5; i++) {
        float noiseval = snoise(pos * freq);
        retval += noiseval * amp;
        amp /= lac + 0.2;
        freq *= lac;
    }
    return retval;
}

void main(void)
{
    //we have an uniform int seed to use
    float seedF = float(seed) * 0.01;
    seedF = sin(seedF) * 100.0;
    
    float mainfreq = frequency;
    float amp = amplitude;
    float displacementX = basicFBM(gl_FragCoord.xy * mainfreq,seedF) * amp;
    float displacementY = basicFBM(gl_FragCoord.xy * mainfreq,-seedF) * amp;

    float invTextureSize = 1.0/2048.0;
    vec4 bhmValue = texture(bhm, gl_FragCoord.xy * invTextureSize + vec2(displacementX,displacementY) * invTextureSize).rgba;
    
    bhmValue.r += (1.0 - bhmValue.r * 2) * flipHeightMap; // reverse (for example to turn mountain into canyons)
    bhmValue.g -= bhmValue.g * flipHeightMap;//if we flip the heightmap, the sediments should be set to 0 because its weird otherwise
    out_Color = bhmValue;
}


