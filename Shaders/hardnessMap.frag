
#version 150 core

out vec4 out_Color;

uniform int seed;

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
        amp /= lac;
        freq *= lac;
    }
    return retval;
}

void main(void)
{
    /*
    float hardnessIndex[10];
    hardnessIndex[0] = 0.1;
    hardnessIndex[1] = 0.9;
    hardnessIndex[2] = 0.1;
    hardnessIndex[3] = 0.3;
    hardnessIndex[4] = 0.8;
    hardnessIndex[5] = 0.1;
    hardnessIndex[6] = 0.9;
    hardnessIndex[7] = 0.9;
    hardnessIndex[8] = 0.1;
    hardnessIndex[9] = 0.5;
     */

    //we have an uniform int seed to use 
    float mainfreq = 0.5;
    float roughness = basicFBM(gl_FragCoord.xy * 2 * mainfreq,12) * 0.2;
    
    //roughness
    //roughness = 0;
    mainfreq = 1.0;
    //roughness += basicFBM(gl_FragCoord.xy * 2 * mainfreq,5) * 0.2;
    mainfreq = 2.0;
    //roughness += basicFBM(gl_FragCoord.xy * 2 * mainfreq,9) * 0.4;
    mainfreq = 0.005;
    //roughness += basicFBM(gl_FragCoord.xy * 2 * mainfreq,9) * 0.8;
    mainfreq = 0.1;
    float bias1 = basicFBM(gl_FragCoord.xy * 2 * mainfreq,9);
    float bias2 = basicFBM(gl_FragCoord.xy * 2 * mainfreq,-12);
    vec2 bias = vec2(bias1,bias2) * 15.0; //warp frequency, good results between 10 and 50
    roughness += basicFBM((gl_FragCoord.xy + bias) * 2 * mainfreq,19) * 0.8;
    mainfreq = 0.00005;
    //roughness += basicFBM(gl_FragCoord.xy * 2 * mainfreq,93) * 0.8;
    //roughness = max(basicFBM(gl_FragCoord.xy * 0.1 * mainfreq,12) * 0.2,roughness);
    //introduce a height value
    
    roughness *= roughness;
    
    roughness = clamp(abs(roughness),0.1,0.7);
    
    out_Color = vec4(roughness * 1,0,0,1);
}

