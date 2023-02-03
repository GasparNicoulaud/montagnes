//  Simplex 3D Noise
//  by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(x*x*34.0+x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
    
    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;
    
    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );
    
    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;
    
    // Permutations
    i = mod(i, 289.0 );
    vec4 p = permute( permute( permute(
                                       i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                              + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
                     + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
    
    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;
    
    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)
    
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)
    
    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);
    
    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );
    
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));
    
    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
    
    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);
    
    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    
    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                 dot(p2,x2), dot(p3,x3) ) );
}

// Simplex 2D noise
//
vec3 permute(in vec3 x) { return mod(x*x*34.0+x, 289.0); }

float snoise(in vec2 v){
    const vec4 C = vec4(0.211324865405187, 0.366025403784439,
                        -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod(i, 289.0);
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
                            dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}



#version 140

in vec2 in_Vertex;

uniform mat4 modproj;
uniform mat4 modelpos;

// Simplex 2D noise
//
vec3 permute(in vec3 x) { return mod(x*x*34.0+x, 289.0); }

float snoise(in vec2 v){
const vec4 C = vec4(0.211324865405187, 0.366025403784439,
-0.577350269189626, 0.024390243902439);
vec2 i  = floor(v + dot(v, C.yy) );
vec2 x0 = v -   i + dot(i, C.xx);
vec2 i1;
i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
vec4 x12 = x0.xyxy + C.xxzz;
x12.xy -= i1;
i = mod(i, 289.0);
vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
+ i.x + vec3(0.0, i1.x, 1.0 ));
vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
dot(x12.zw,x12.zw)), 0.0);

vec3 x = 2.0 * fract(p * C.www) - 1.0;
vec3 h = abs(x) - 0.5;
vec3 ox = floor(x + 0.5);
vec3 a0 = x - ox;
vec3 m4 = m*m*m*m;
m = (m4 * 1.79284291400159) - (m4 * 0.85373472095314 * ( a0*a0 + h*h ));
vec3 g;
g.x  = a0.x  * x0.x  + h.x  * x0.y;
g.yz = a0.yz * x12.xz + h.yz * x12.yw;
return 130.0 * dot(m, g);
}

void main(void) {

vec4 transformedPos = modelpos * vec4(in_Vertex, 0.0, 1.0); //place the vertex in the worldpos

float height = snoise(transformedPos.xy); //height of the vertex
vec3 noisedPos = vec3(transformedPos.xy,height); //actual pos of the vertex

// all the out stuff
gl_Position = modproj * vec4(noisedPos, 1.0); //transform to screen space and send out
}






///////////////////




#version 140

in vec2 in_Vertex;

out vec3 pos; //the position for the fragment shader
out vec3 normal;

uniform mat4 modproj;
uniform mat4 modelpos;

// Simplex 2D noise
//
vec3 permute(in vec3 x) { return mod(x*x*34.0+x, 289.0); }

float snoise(in vec2 v){
const vec4 C = vec4(0.211324865405187, 0.366025403784439,
-0.577350269189626, 0.024390243902439);
vec2 i  = floor(v + dot(v, C.yy) );
vec2 x0 = v -   i + dot(i, C.xx);
vec2 i1;
i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
vec4 x12 = x0.xyxy + C.xxzz;
x12.xy -= i1;
i = mod(i, 289.0);
vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
+ i.x + vec3(0.0, i1.x, 1.0 ));
vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
dot(x12.zw,x12.zw)), 0.0);

vec3 x = 2.0 * fract(p * C.www) - 1.0;
vec3 h = abs(x) - 0.5;
vec3 ox = floor(x + 0.5);
vec3 a0 = x - ox;
vec3 m4 = m*m*m*m;
m = (m4 * 1.79284291400159) - (m4 * 0.85373472095314 * ( a0*a0 + h*h ));
vec3 g;
g.x  = a0.x  * x0.x  + h.x  * x0.y;
g.yz = a0.yz * x12.xz + h.yz * x12.yw;
return 130.0 * dot(m, g);
}

float alpNoise(in vec2 v) //attempts to do nice mountains
{
float baseTerrain = snoise(v * 0.1);

float mntCoef = max(snoise(v * 0.5) + snoise((v + vec2(123,456)))*0.5,0.0f);
float mntn = 1.5-abs(snoise(v) + snoise(v+ vec2(-321,111)*2)/2  + snoise(v+ vec2(321,-111)*5)/5);

float rblCoef = (max(snoise(v * 2) + 0.5,0) + 0.1) * 0.1 ;
float rubble = abs(snoise(v*15)*0.1) + (1 - abs(snoise(v*30)*0.05)) + snoise(v*60)*0.03 + snoise(v*150)*0.01;

float retval = baseTerrain + (mntCoef * mntn * 0.5) + (rubble * rblCoef);

return retval;
}

void main(void) {

//here we should skew one out of two vertices in function of the dst to cam

float grittyness = 0.0008;
float peakHeight = 500;

vec4 transformedPos = modelpos * vec4(in_Vertex, 0.0, 1.0); //place the vertex in the worldpos

float height = alpNoise(transformedPos.xy * grittyness) * peakHeight; //height of the vertex
vec3 noisedPos = vec3(transformedPos.xy,height); //actual pos of the vertex

float nd = 1; //area sampled to get the normal. artifacts appear under 0.005

//normal buisness //also I guess if this was moved to fragment shader, it would look muuuuuch better
float hb = alpNoise((transformedPos.xy + vec2(nd,0))*grittyness) * peakHeight;
float hc = alpNoise((transformedPos.xy + vec2(0,nd))*grittyness) * peakHeight;
vec3 b = vec3(transformedPos.xy,0) + vec3(nd,0.0,hb);
vec3 c = vec3(transformedPos.xy,0) + vec3(0.0,nd,hc);

// all the out stuff
gl_Position = modproj * vec4(noisedPos, 1.0); //transform to screen space and send out
normal = normalize(cross(b - noisedPos, c - noisedPos));
pos = noisedPos;
}








//////EROSIONSIOSNISNOSINSOISNSOINSOINSINSOISN




/*
 :copyright: 2010 by Florian Boesch <pyalot@gmail.com>.
 :license: GNU AGPL v3 or later, see LICENSE for more details.
 */

#version 120 core

uniform sampler2D texture;
uniform sampler2D filter_weight;
uniform vec2 offsets;
const float pih = 1.0/(3.14159265358979323846264*0.5);
uniform bool invert, shallow, rough, slope;

vec3 get(vec2 uv){
    float height = texture2D(texture, vec2(uv)).r;
    return vec3(uv.s, height, uv.t);
}

void main(){
    vec2 uv = gl_TexCoord[0].st;
    float s = offsets.x;
    float t = offsets.y;
    
    float weight = texture2D(filter_weight, uv).r;
    vec3 pos = get(uv);
    vec3 left = get(uv+vec2(-s, 0.0));
    vec3 right = get(uv+vec2(s, 0.0));
    vec3 top = get(uv+vec2(0.0, t));
    vec3 bottom = get(uv+vec2(0.0, -t));
    vec3 left_top = get(uv+vec2(-s, t));
    vec3 right_top = get(uv+vec2(s, t));
    vec3 left_bottom = get(uv+vec2(-s, -t));
    vec3 right_bottom = get(uv+vec2(s, -t));
    
    vec4 a = vec4(left.y, right.y, top.y, bottom.y);
    vec4 b = vec4(left_top.y, right_top.y, left_bottom.y, right_bottom.y);
    
    vec4 comparision;
    float count = 1.0;
    float sum = pos.y;
    float result;
    
    if(invert){
        comparision = vec4(greaterThan(a, vec4(pos.y)));
        count += dot(comparision, comparision);
        sum += dot(comparision, a);
        
        if(!rough){
            comparision = vec4(greaterThan(b, vec4(pos.y)));
            count += dot(comparision, comparision);
            sum += dot(comparision, b);
        }
    }
    else{
        comparision = vec4(lessThan(a, vec4(pos.y)));
        count += dot(comparision, comparision);
        sum += dot(comparision, a);
        
        if(!rough){
            comparision = vec4(lessThan(b, vec4(pos.y)));
            count += dot(comparision, comparision);
            sum += dot(comparision, b);
        }
    }
    
    if(slope){
        vec3 normal = normalize(vec3(
                                     left.y - right.y,
                                     s+t,
                                     bottom.y - top.y
                                     ));
        float factor = dot(normal, vec3(0.0, 1.0, 0.0));
        if(shallow){
            factor = 1.0-factor;
        }
        else{
            factor = factor-0.05*count;
        }
        result = mix(sum/count, pos.y, factor);
    }
    else{
        result = sum/count;
    }
    
    result = mix(result, pos.y, clamp(weight, 0.0, 1.0));
    
    gl_FragColor = vec4(result);
}











