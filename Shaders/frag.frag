
#version 150 core

in vec3 pos;
in vec3 normal;
in vec3 origVertex;
in float inoutscale;
in vec3 inoutcampos;
in float inouttime;
in float inoutUD; //inout upside down
//in float error;

out vec4 out_Color;

uniform sampler2D matexture;

//
// Description : Array and textureless GLSL 2D/3D/4D simplex
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
//

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
{
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
    
    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y
    
    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
                                       i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                              + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
                     + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
    
    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;
    
    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)
    
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)
    
    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);
    
    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );
    
    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
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

float colnoise(in vec3 v) // returns a noise but between 1 and 0 not 1 and -1
{
    return (snoise(v.xy) * 0.5) + 0.5;
}

float colsumnoise(in vec3 v)
{
    float str = 0.0005; //stretching of the texture
    //return (colnoise(v * str) + colnoise(v * str*2)/2.0 + colnoise(v * str*4)/3.0 + colnoise(v * str*8)/4.0 + colnoise(v * str*16)/6.0 + colnoise(v * str*32)/12.0);
    return 1;
}

float fogD(float a, float b)
{
    float top = (max(a,b) - 4000)/500.0;
    float bot = (min(a,b) - 4000)/500.0;
    // top and bot are between 0 and 1
    float toppart = (top - 4250)/2.0;
    float mid = (top + bot)/2.0;
    float avgDensity = (0.5 - abs(mid - 0.5));// * (1.0/(top-bot));
    avgDensity = (top - bot) * (0.5 - abs(mid - 0.5)) + (0.5 - abs(mid - 0.5));
    
    
    return avgDensity;
}

void main(void)
{
    //vec3 sunlightDirection = normalize(vec3(sin(inouttime * 0.2f),cos(inouttime * 0.2f),0.75));
    vec3 sunlightDirection = normalize(vec3(0,cos(inouttime * 0.5f),sin(inouttime * 0.5f)));

    if(inoutUD >= 0.5){
        sunlightDirection.xy *= -1.0f;
    }
    
    vec3 rofst = vec3(150,250,-100);
    vec3 gofst = vec3(750,0,100);
    vec3 bofst = vec3(50,-250,-500);
    
    /*float matr = colsumnoise(pos + rofst) * 0.25 + 0.3;
    float matg = matr - colsumnoise(pos + gofst) * 0.032; // because fuck green and blue right ?
    float matb = matr - colsumnoise(pos + bofst) * 0.03;
    vec3 materialColor = vec3(matr,matg,matb);*/
    //materialColor = vec3(0.5);
    vec3 materialColor = vec3(0.35,0.2,0.2);
    //materialColor += min(texture(matexture,origVertex.xy).g,500) * .01f * vec3(0.26,0.08,0.03); // red sand
    materialColor += min(texture(matexture,origVertex.xy).g,500) * .02f * vec3(0.55,0.55,0.55); //snow

    //materialColor -= texture(matexture,origVertex.xy).b * vec3(0.31,0.30,0.29);

    float zStretch = 1 * (1.0f/inoutscale);
    float nd = 0.002;// * (1.0f/inoutscale); //area sampled to get the normal. artifacts appear under 0.005
    
    float sampleScaling = 0.000005;
    
    vec2 aorig = origVertex.xy + vec2(-nd,0);
    vec2 borig = origVertex.xy + vec2(nd,0);
    vec2 corig = origVertex.xy + vec2(0,-nd);
    vec2 dorig = origVertex.xy + vec2(0,nd);
    vec3 a = vec3(aorig,texture(matexture, aorig).x * (sampleScaling * zStretch));
    vec3 b = vec3(borig,texture(matexture, borig).x * (sampleScaling * zStretch));
    vec3 c = vec3(corig,texture(matexture, corig).x * (sampleScaling * zStretch));
    vec3 d = vec3(dorig,texture(matexture, dorig).x * (sampleScaling * zStretch));
    float light = normalize(cross(b - a, d - c)).z;
    
    if(abs(light) > 0.8) // snow threshold
    {
        //materialColor = vec3(0.97,0.98,1); //uncoment to get snow
        //materialColor = vec3(fract(pos.z/10.0),0.98,1);
    }
    
    if(abs(light) < 0.05)
    {
        light = 0.99;
    }
    //float light = 1;
    
    vec3 fogColor = vec3(0.97,0.97,1.0);// * clamp(2 - (pos.z / 300.0f - 13),1,2);
    /*
    float fogtop = clamp(inoutcampos.z,4000.0,4500.0);
    float fogbot = clamp(pos.z,4000.0,4500.0);
    float fogmid = (fogtop + fogbot)/2.0;
    float fracOfTripInFog = abs(fogtop - fogbot)/abs(pos.z-inoutcampos.z);
    float distanceInFog = fracOfTripInFog * distance(inoutcampos,pos);
    vec3 fogtoppoint = mix(inoutcampos,pos,abs((inoutcampos.z-fogtop))/abs(pos.z-inoutcampos.z));
    vec3 fogbotpoint = mix(inoutcampos,pos,abs((inoutcampos.z-fogbot))/abs(pos.z-inoutcampos.z));
    vec3 midpoint = (fogtoppoint + fogbotpoint)/2.0;
    
    
    float integA = -cos((abs(4200.0-fogtop)/100.0) * 3.1415);
    float integB = -cos((abs(4200.0-fogbot)/100.0) * 3.1415);
    float integ = abs(integA-integB);
    
    integ = fogD(fogtop,fogbot);
    
    integ = 1 - (abs(4150.0 - midpoint.z)/50.0);
    if((fogtop > 4150 && fogbot > 4150) || (fogtop < 4150 && fogbot < 4150))
    {
        integ = 1 - (abs(4150.0 - fogmid)/50.0);
    }
    else if(fogtop > fogbot)
    {
        
        float part1 = ((fogtop + 4150)/2.0 - 4150)/50.0 * abs(fogtop - 4150)/abs(fogtop - fogbot);
        float part2 = ((fogbot + 4150)/2.0 - 4100)/50.0 * abs(fogbot - 4150)/abs(fogtop - fogbot);
        integ = part1 + part2;
    }
    else
    {
        
        float part1 = ((fogbot + 4150)/2.0 - 4150)/50.0 * abs(fogbot - 4150)/abs(fogtop - fogbot);
        float part2 = ((fogtop + 4150)/2.0 - 4100)/50.0 * abs(fogtop - 4150)/abs(fogtop - fogbot);
        integ = part1 + part2;
    }*/
    
    //float part1 = max(abs(fogtop - 4150),0)/abs(fogtop - fogbot) * (1 - abs(fogtop - 4150)/50.0);
    //float part2 = max(abs(fogbot - 4150),0)/abs(fogtop - fogbot) * (1 - abs(fogbot - 4150)/50.0);
    //integ = part1 + part2;
    
    
    
    float fogamount = gl_FragCoord.z / (gl_FragCoord.w * 10000.0f)  - ((pos.z - 100) * 0.0002);
    fogamount = clamp(fogamount,0,1);
    //vec3 fogpos = vec3(midpoint.xy,inouttime*100) / 2.0;
    
    //fogamount += distanceInFog * 0.001 * integ * (((snoise(fogpos*0.002))+snoise(fogpos*0.004)/2.0+snoise(fogpos*0.008)/4.0+snoise(fogpos*0.016)/8.0)+1);
    //fogamount += max(1 - (pos.z / 200.0f - 17),0);
    //fogamount = 0;
    //fogamount = clamp(fogamount,0,1);
    
    //out_Color = vec4(materialColor * light * light * light * light * (1-fogamount) + fogamount * fogColor,1.0);
    //out_Color = vec4((gl_FragCoord.z / gl_FragCoord.w) / 5000.0f );
    //out_Color = texture(matexture,pos.xy);
    //out_Color.g = 1 - (pos.z / 300.0f - 13);
    
    /*
    if(abs(pos.x) > 2000 || abs(pos.y) > 2000 ) out_Color.r = 0.8; // unplayable area
    if(abs(pos.x) < 1000 && abs(pos.y) < 1000 ) out_Color.g = 0.8;
    if(abs(pos.x) < 500 && abs(pos.y) < 500 ) out_Color.g = 1; // area of defense
     */
    
    //float cosTheta =
    
    zStretch = 1.0f;///inoutscale;
    
    vec2 size = vec2(nd*2,0.0);
    vec3 va = normalize(vec3(size.xy,(b.z-a.z)*zStretch));
    vec3 vb = normalize(vec3(size.yx,(d.z-c.z)*zStretch));
    vec3 normal = cross(va,vb);// + vec3(0.5,0.5,0);
    
    out_Color = vec4( materialColor * clamp(dot(sunlightDirection,normal),0,1), 1);//vec3(clamp(dot(sunlightDirection,normal),0,1));
    
    out_Color.rgb = out_Color.rgb * (1-fogamount) + fogamount * fogColor;
    
    out_Color.rgb = pow(out_Color.rgb,vec3(1.0/2.2)); //gamma correction
    
    /*if(texture(matexture,origVertex.xy).g > 0.0001)
    {
        out_Color.g = 1;
    }
    else
    {
        out_Color.g = 0;
    }*/
    
    //out_Color.g = texture(matexture,origVertex.xy).g;
    
    float gridSize = 488.28125;
    // red grid gridSize x gridSize meter grid
    //if(abs(mod(pos.x,gridSize)) < gridSize*0.1)out_Color.r += 0.8;
    //if(abs(mod(pos.y,gridSize)) < gridSize*0.1)out_Color.r += 0.8;
}

