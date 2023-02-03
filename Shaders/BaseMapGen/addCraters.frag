#version 150 core

out vec4 out_Color;

uniform sampler2D bhm;

uniform int seed;
uniform float amplitude;
uniform float frequency;

/*
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
*/
 
// Gaspar 2D noise, polka dot pattern-like
float gnoise(vec2 v){ //returns values between 0 and 2

    v += vec2(100,100);

    vec2 fv = floor(v);
    vec2 locv = v-fv; // local v
    vec2 dt; //the dot location

    //dotsz should be between 0.1 and 0.2
    float dotsz = mod(abs(sin(fv.x + 59) * cos(fv.y + 83) * 50 + 6197),0.1) + 0.1;
    float dotsz10 = dotsz * 10;

    //dotx and doty should be between 1 + dotsz and 1 - dotsz
    dt.x = mod(abs(sin(fv.x + 3) * cos(fv.y + 13) * 100 + 5153),1 - dotsz*2) + dotsz;
    dt.y = mod(abs(cos(fv.x + 7) * sin(fv.y + 17) * 300 + 6037),1 - dotsz*2) + dotsz;

    float retval = max(dotsz-distance(locv,dt),0) * (1.0f/dotsz) * 3.14159 - 3.14159; // retval is between -pi and 0

    return (cos(retval) * dotsz10 + dotsz10) * 0.5;
}

float moonnoise(vec2 v)
{
    vec2 ofst = vec2(5,5);
    /*
    float retval = 0.0f
    - abs(0.45 - gnoise(v * 0.7  - ofst*5)/3.7)
    - abs(0.4  - gnoise(v * 0.8  + ofst*6)/3.5)
    - abs(0.35 - gnoise(v * 0.9  - ofst*7)/3.2)
    - abs(0.3  - gnoise(v        + ofst*8)/3.0)
    - abs(0.2  - gnoise(v * 2    - ofst*3)/2.0)/2.0
    - abs(0.1  - gnoise(v * 4    + ofst*2)/1.5)/4.0
    - abs(0.09 - gnoise(v * 8    - ofst  )    )/8.0
    - abs(0.08 - gnoise(v * 16   + ofst*2)    )/16.0
    - abs(0.07 - gnoise(v * 32   - ofst*3)    )/32.0
    - abs(0.06 - gnoise(v * 64   + ofst*4)    )/64.0
    - abs(0.05 - gnoise(v * 128  - ofst*5)    )/128.0
    - abs(0.04 - gnoise(v * 256  + ofst*6)    )/256.0
    - abs(0.03 - gnoise(v * 512  - ofst*7)    )/512.0
    - abs(0.02 - gnoise(v * 1024 + ofst*8)    )/1024.0
    - abs(0.01 - gnoise(v * 2048 - ofst*9)    )/2048.0
    - abs(0.009- gnoise(v * 4096 + ofst*11)   )/4096.0;
    //retval += simplexSumNoise(v) * 0.5;
    */
    ///*
    float retval = 0;
    //retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 0.7  - ofst*5)/3.7));
    //retval = min(retval, -1.0f * abs(0.0  - gnoise(v * 0.8  + ofst*6)/3.5));
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 0.9  - ofst*7)/3.2));
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v        + ofst*8)/3.0));
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 2    - ofst*3)/2.0)/2.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 4    + ofst*2)/1.5)/4.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 8    - ofst  )    )/8.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 16   + ofst*2)    )/16.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 32   - ofst*3)    )/32.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 64   + ofst*4)    )/64.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 128  - ofst*5)    )/128.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 256  + ofst*6)    )/256.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 512  - ofst*7)    )/512.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 1024 + ofst*8)    )/1024.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 2048 - ofst*9)    )/2048.0);
    retval = min(retval, -1.0f * abs(0.0 - gnoise(v * 4096 + ofst*11)   )/4096.0);

    //retval += 1.7;
    return retval;
}

void main(void)
{
    //we have an uniform int seed to use
    float seedF = float(seed) * 0.01;
    seedF = sin(seedF) * 100.0;
    
    float mainfreq = frequency * 0.0001;
    float amp = amplitude * 0.5;
    
    float moonNoise = moonnoise((gl_FragCoord.xy * mainfreq) + vec2(seedF,seedF)) * amp;
    
    //float displacementX = basicFBM(gl_FragCoord.xy * mainfreq,seedF) * amp;
    //float displacementY = basicFBM(gl_FragCoord.xy * mainfreq,-seedF) * amp;

    float invTextureSize = 1.0/2048.0;
    vec4 bhmValue = texture(bhm, gl_FragCoord.xy * invTextureSize).rgba;
    
    bhmValue.r += moonNoise;
    //bhmValue.r += (1.0 - bhmValue.r * 2) * flipHeightMap; // reverse (for example to turn mountain into canyons)
    
    out_Color = bhmValue;
}



