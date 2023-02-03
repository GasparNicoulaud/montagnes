
#version 150 core

out vec4 out_Color;

uniform sampler2D bhmCopy;
uniform sampler2D normConvMap;

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
    for (int i = 0; i < 10; i++) {
        float noiseval = snoise(pos * freq);
        retval += noiseval * amp;
        amp /= lac + 0.1;
        freq *= lac;
    }
    return retval;
}

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
    float f = 2.*3.14159265358979;
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
float clayJohnNoise(vec2 pos, vec2 norm)
{
    //vec3 n = vec3(0.0,sin(pos));
    
    
    //take the curl of the normal to get the gradient facing down the slope
    vec2 dir = norm*vec2(-1.0, 1.0) * 5.0;
    
    //Now we compute another fbm type noise
    // erosion is a type of noise with a strong directionality
    //we pass in the direction based on the slope of the terrain
    //erosion also returns the slope. we add that to a running total
    //so that the direction of successive layers are based on the
    //past layers
    vec3 h = vec3(0.0);
    float a = 0.7;
    float f = 1.0;
    for (int i=0;i<2;i++) {
        h+= erosion(pos*f, dir+h.zy*vec2(-1.0, 1.0))*a*vec3(1.0, f, f);
        a*=0.5;
        f*=2.0;
    }
    //remap height to [0,1] and add erosion
    //looks best when erosion amount is small
    return h.x;
}

float mountain(vec2 p, float s, vec2 norm, float baseNoiseHeight) {
    
    p += vec2(100,100);
    //First generate a base heightmap
    //it can be based on any type of noise
    //so long as you also generate normals
    //Im just doing basic FBM based terrain using
    //iq's analytic derivative gradient noise
    vec3 n = vec3(0.0);
    float nf = 2.0;
    float na = 0.9;
    for (int i=0;i<2;i++) {
        n+= noised(p*s*nf)*na*vec3(1.0, nf, nf);
        na *= 0.5;
        nf *= 2.0;
    }
    
    
    //take the curl of the normal to get the gradient facing down the slope
    vec2 dir = n.zy*vec2(1.0, -1.0);
    dir = norm.yx*vec2(1.0, -1.0) * 300.0;

    
    //Now we compute another fbm type noise
    // erosion is a type of noise with a strong directionality
    //we pass in the direction based on the slope of the terrain
    //erosion also returns the slope. we add that to a running total
    //so that the direction of successive layers are based on the
    //past layers
    vec3 h = vec3(0.0);
    float a = 1.5*(smoothstep(0.3, 0.5,n.x*0.5+0.5)); //smooth the valleys
    float f = 0.5;
    for (int i=0;i<5;i++) {
        h+= erosion(p*f, dir+h.zy*vec2(1.0, -1.0))*a*vec3(1.0, f, f);
        a*=0.4;
        f*=2.0;
    }
    //remap height to [0,1] and add erosion
    //looks best when erosion amount is small
    //not sure about adding the normals together, but it looks okay
    return h.x*0.15 * (min(length(norm),0.005) * 100.0) * (min(length(norm),0.005) * 100.0);
}

void main(void)
{
    float invTextureSize = 1.0/2048.0;
    vec2 normal = texture(normConvMap, gl_FragCoord.xy * invTextureSize).rg;
    float conv = texture(normConvMap, gl_FragCoord.xy * invTextureSize).b;

    //normal *= max(0,1.0 - max(0,conv * 2000.0));
    float clayNoise = clayJohnNoise(gl_FragCoord.xy * 0.05,normal * 1.0);
    
    vec3 bhmBaseValue = texture(bhmCopy, gl_FragCoord.xy * invTextureSize).rgb;
    //bhmBaseValue.r = clayNoise;
    //bhmBaseValue.r += clayNoise * 0.1;
    
    //normal *= 1.0 - clamp(length(normal) * 100,0,1);

    //bhmBaseValue.r = normal.x * 10.0 + normal.y * 10.0;
    //normal *= 2.0;
    
    //bhmBaseValue.r += basicFBM(gl_FragCoord.xy * 0.05 + normal * vec2(1.0,-1.0) * 200, 1) * 0.005 * length(normal) * 100;
    
    //clayNoise -= 0.7;
    
    //bhmBaseValue.r -= clayNoise * 2.0;
    //bhmBaseValue.r = clayNoise * 5.0 + conv;
    
    bhmBaseValue.r += mountain(gl_FragCoord.xy * 0.01, 0.05, normal, bhmBaseValue.r);
    
    //bhmBaseValue.r = normal.x * 10.0 + normal.y * 10.0;
    
    //bhmBaseValue.r = conv * 1.0;
    
    bhmBaseValue.g = 0.00;

    out_Color = vec4(bhmBaseValue.r,bhmBaseValue.g,bhmBaseValue.b,1.0);
}


