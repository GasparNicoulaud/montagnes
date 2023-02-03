#version 150 core

out vec4 out_Color;

uniform int seed;
uniform float globalFrequency;
uniform float globalAmplification;

vec2 hash( in vec2 x )  // replace this by something better
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

vec3 noised( vec2 p )
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

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

const mat2 m2 = mat2(0.8,-0.6,0.6,0.8);

float FBMQuilezDetail(vec2 p, float seed){
    
    p *= 0.005;
    p += vec2(cos(seed * 2) , sin(seed * -3));
    
    float a = 0.0;
    float b = 1.0;
    vec2  d = vec2(0.0);
    
    for( int i=0; i<16; i++ )
    {
        vec3 n=noised(p);
        d += n.yz;
        a += b * pow(n.x,2.0 - i * 0.1) / (1.0 + dot(d,d));
        b *= 0.55;
        p =  m2 * p * 2.5;
    }
    
    return a * 0.5;
}

float FBMQuilezSqrt(vec2 p, float seed){
    
    p *= 0.001;
    p += vec2(cos(seed * 2) , sin(seed * -3));
    
    float a = 0.0;
    float b = 1.0;
    vec2  d = vec2(0.0);
    
    for( int i=0; i<16; i++ )
    {
        vec3 n=noised(p);
        d += n.yz;
        a += b * pow(pow(n.x,0.3)/ (1.0 + dot(d,d)), 1.5);
        b *= 0.5;
        p =  m2 * p * 2.0;
    }
    
    return a * 2;
}

void main(void)
{
    float globalFreq = globalFrequency;
    float globalAmp = globalAmplification;
    
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,123.456);
    brseedAbs *= 0.01;
    
    float quilez = FBMQuilezSqrt(gl_FragCoord.xy * globalFreq,brseedAbs) * globalAmp;
    
    out_Color = vec4(quilez + 0.7,0,0,1.0);
}




