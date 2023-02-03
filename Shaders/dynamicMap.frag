
#version 410 core

layout(location = 0) out vec4 heightHorizontalDisplacementAO;
layout(location = 1) out vec4 albedoRoughness;

in vec2 worldPos;

uniform sampler2D lightMap;
uniform sampler2D geometryMap;
uniform sampler2D materialMap;
uniform sampler2D rocksMaterialRouhnessAO;
uniform sampler2D rocksMaterialAlbedoHeight;
uniform sampler2D stoneMaterialRouhnessAO;
uniform sampler2D stoneMaterialAlbedoHeight;
uniform sampler2D sandMaterialRouhnessAO;
uniform sampler2D sandMaterialAlbedoHeight;
uniform sampler2D flowMaterialRouhnessAO;
uniform sampler2D flowMaterialAlbedoHeight;

uniform float horizontalDisplacementCoeff;


// 4x4 bicubic filter using 4 bilinear texture lookups
// See GPU Gems 2: "Fast Third-Order Texture Filtering", Sigg & Hadwiger:
// http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter20.html

// w0, w1, w2, and w3 are the four cubic B-spline basis functions
float w0(float a)
{
    return (1.0/6.0)*(a*(a*(-a + 3.0) - 3.0) + 1.0);
}

float w1(float a)
{
    return (1.0/6.0)*(a*a*(3.0*a - 6.0) + 4.0);
}

float w2(float a)
{
    return (1.0/6.0)*(a*(a*(-3.0*a + 3.0) + 3.0) + 1.0);
}

float w3(float a)
{
    return (1.0/6.0)*(a*a*a);
}

// g0 and g1 are the two amplitude functions
float g0(float a)
{
    return w0(a) + w1(a);
}

float g1(float a)
{
    return w2(a) + w3(a);
}

// h0 and h1 are the two offset functions
float h0(float a)
{
    return -1.0 + w1(a) / (w0(a) + w1(a));
}

float h1(float a)
{
    return 1.0 + w3(a) / (w2(a) + w3(a));
}

// by iquilez
vec4 textureGood( sampler2D sam, vec2 uv )
{
    vec2 res = textureSize( sam,0 );

    vec2 st = uv*res - 0.5;

    vec2 iuv = floor( st );
    vec2 fuv = fract( st );

    vec4 a = texture( sam, (iuv+vec2(0.5,0.5))/res );
    vec4 b = texture( sam, (iuv+vec2(1.5,0.5))/res );
    vec4 c = texture( sam, (iuv+vec2(0.5,1.5))/res );
    vec4 d = texture( sam, (iuv+vec2(1.5,1.5))/res );

    return mix( mix( a, b, fuv.x),
                mix( c, d, fuv.x), fuv.y );
}

//by iquilez
vec4 texture2D_bicubic(sampler2D tex, vec2 uv)
{
    vec2 res = textureSize(tex, 0);
    uv = uv*res + 0.5;
    vec2 iuv = floor( uv );
    vec2 fuv = fract( uv );

    float g0x = g0(fuv.x);
    float g1x = g1(fuv.x);
    float h0x = h0(fuv.x);
    float h1x = h1(fuv.x);
    float h0y = h0(fuv.y);
    float h1y = h1(fuv.y);

    vec2 p0 = (vec2(iuv.x + h0x, iuv.y + h0y) - 0.5) / res;
    vec2 p1 = (vec2(iuv.x + h1x, iuv.y + h0y) - 0.5) / res;
    vec2 p2 = (vec2(iuv.x + h0x, iuv.y + h1y) - 0.5) / res;
    vec2 p3 = (vec2(iuv.x + h1x, iuv.y + h1y) - 0.5) / res;
    
    vec4 p0Val = textureGood(tex, p0);
    vec4 p1Val = textureGood(tex, p1);
    vec4 p2Val = textureGood(tex, p2);
    vec4 p3Val = textureGood(tex, p3);

    return g0(fuv.y) * (g0x * p0Val  +
                        g1x * p1Val) +
           g1(fuv.y) * (g0x * p2Val  +
                        g1x * p3Val);
}

vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)),
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); }

vec4 textureNoTile( sampler2D samp, in vec2 uv )
{
    ivec2 iuv = ivec2( floor( uv ) );
     vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + ivec2(0,0) );
    vec4 ofb = hash4( iuv + ivec2(1,0) );
    vec4 ofc = hash4( iuv + ivec2(0,1) );
    vec4 ofd = hash4( iuv + ivec2(1,1) );
    
    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign( ofa.zw-0.5 );
    ofb.zw = sign( ofb.zw-0.5 );
    ofc.zw = sign( ofc.zw-0.5 );
    ofd.zw = sign( ofd.zw-0.5 );
    
    // uv's, and derivatives (for correct mipmapping)
    vec2 uva = uv*ofa.zw + ofa.xy, ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy, ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy, ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy, ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;
        
    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );
    
    return mix( mix( textureGrad( samp, uva, ddxa, ddya ),
                     textureGrad( samp, uvb, ddxb, ddyb ), b.x ),
                mix( textureGrad( samp, uvc, ddxc, ddyc ),
                     textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}


vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, s, -s, c);
    return m * v;
}

void main(void)
{
    vec2 uvLocalToTexture = worldPos;
    
    vec4 geometryMapVal = texture2D_bicubic(geometryMap,uvLocalToTexture);
    
    //geometryMapVal.r = 0;
    
    vec4 materialMapVal = texture2D_bicubic(materialMap,uvLocalToTexture);
    vec4 materialMapDEBUG = materialMapVal;
    vec4 lightMapVal = texture2D_bicubic(lightMap,uvLocalToTexture);
    
    float textureZoomFactor = 50; //used to be 64 but I made weird pattern on high freq textures, maybe bad ratio from zoom * uvtolocal ?
    //tiling
    
    vec2 stoneMaterialRouhnessAOVal;// = texture2D_bicubic(stoneMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 1.0).xy;
    vec4 stoneMaterialAlbedoHeightVal;// = texture2D_bicubic(stoneMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 1.0);
    vec2 rocksMaterialRouhnessAOVal;// = texture2D_bicubic(rocksMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 0.9).xy;
    vec4 rocksMaterialAlbedoHeightVal;// = texture2D_bicubic(rocksMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 0.9);
    vec2 sandMaterialRouhnessAOVal;// = texture2D_bicubic(sandMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 0.8).xy;
    vec4 sandMaterialAlbedoHeightVal;// = texture2D_bicubic(sandMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 0.8);
    vec2 flowMaterialRouhnessAOVal;// = texture2D_bicubic(sandMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 0.8).xy;
    vec4 flowMaterialAlbedoHeightVal;// = texture2D_bicubic(sandMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 0.8);
    
    float stoneZoomFactorModifier = 0.5;
    //intelligent tiling
    stoneMaterialRouhnessAOVal = textureNoTile(stoneMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * stoneZoomFactorModifier).xy;
    stoneMaterialAlbedoHeightVal = textureNoTile(stoneMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * stoneZoomFactorModifier);
    
    //also sample verically I guess ?
    //verticalStretching should be a sahder compile-time parameter, from 0.000001 for no strata to 0.1 for max strata. gen with a pow(rng,5) or something like that
    float verticalStretching = 0.01;
    //stoneMaterialRouhnessAOVal += textureNoTile(stoneMaterialRouhnessAO,geometryMapVal.rr * verticalStretching).xy;
    //dont take height into account, maybe it should count toward horizontal displacement but not vertical height
    vec2 pointToSampleForStrata = vec2(geometryMapVal.r + stoneMaterialAlbedoHeightVal.a * 40.0)
    //term below is added to starta height, at shader compile we should choose if we use stone, rock, sand or flow texture's height, the factor
    + texture2D_bicubic(stoneMaterialAlbedoHeight,uvLocalToTexture).a * 1000.0;
    
    
    stoneMaterialAlbedoHeightVal.rgb = stoneMaterialAlbedoHeightVal.rgb * 0.5 + textureNoTile(stoneMaterialAlbedoHeight,pointToSampleForStrata * verticalStretching * stoneZoomFactorModifier).rgb * 0.5;
    //or maybe take it into account but very slightly ?
    stoneMaterialAlbedoHeightVal.a += sin(textureNoTile(stoneMaterialAlbedoHeight,pointToSampleForStrata * verticalStretching * stoneZoomFactorModifier).a * 100.0) * 0.001; // maybe the 0.3 should sacle to 1/verticalStretching ?? but it will offset the whole texture so not trivial
    
    rocksMaterialRouhnessAOVal = textureNoTile(rocksMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 1.0).xy;
    rocksMaterialAlbedoHeightVal = textureNoTile(rocksMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 1.0);
    
    sandMaterialRouhnessAOVal = textureNoTile(sandMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 1.0).xy;
    sandMaterialAlbedoHeightVal = textureNoTile(sandMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 1.0);
    
    flowMaterialRouhnessAOVal = textureNoTile(flowMaterialRouhnessAO,uvLocalToTexture * textureZoomFactor * 1.0).xy;
    flowMaterialAlbedoHeightVal = textureNoTile(flowMaterialAlbedoHeight,uvLocalToTexture * textureZoomFactor * 1.0);
    
    float dynamicHeight = max(max(max(stoneMaterialAlbedoHeightVal.a * materialMapVal.r,
                                      rocksMaterialAlbedoHeightVal.a * materialMapVal.g),
                                  sandMaterialAlbedoHeightVal.a * materialMapVal.b),
                                flowMaterialAlbedoHeightVal.a * materialMapVal.a);
    
    materialMapVal.rgba *= 0.1;
    
    float stoneHeight = stoneMaterialAlbedoHeightVal.a + materialMapVal.r;
    float rocksHeight = rocksMaterialAlbedoHeightVal.a + materialMapVal.g;
    float sandHeight = sandMaterialAlbedoHeightVal.a + materialMapVal.b;
    float flowHeight = flowMaterialAlbedoHeightVal.a + materialMapVal.a;
    dynamicHeight = max(max(max(stoneHeight,rocksHeight),sandHeight),flowHeight);
    
    dynamicHeight *= 40.0;
    
    //if stone is higher than sand, then stuff. this is hacky and surely super slow
    if(stoneHeight > rocksHeight && stoneHeight > sandHeight && stoneHeight > flowHeight)
    {
        materialMapVal.r = 1.0;
        materialMapVal.g = 0.0;
        materialMapVal.b = 0.0;
        materialMapVal.a = 0.0;
    }
    else if(rocksHeight > sandHeight && rocksHeight > flowHeight)
    {
        materialMapVal.r = 0.0;
        materialMapVal.g = 1.0;
        materialMapVal.b = 0.0;
        materialMapVal.a = 0.0;
    }
    else if(sandHeight > flowHeight)
    {
        materialMapVal.r = 0.0;
        materialMapVal.g = 0.0;
        materialMapVal.b = 1.0;
        materialMapVal.a = 0.0;
    }
    else{
        materialMapVal.r = 0.0;
        materialMapVal.g = 0.0;
        materialMapVal.b = 0.0;
        materialMapVal.a = 1.0;
    }
    
    
    vec3 dynamicAlbedo = stoneMaterialAlbedoHeightVal.rgb * materialMapVal.r
                       + rocksMaterialAlbedoHeightVal.rgb * materialMapVal.g
                       + sandMaterialAlbedoHeightVal.rgb * materialMapVal.b
                       + flowMaterialAlbedoHeightVal.rgb * materialMapVal.a;
    
    float dynamicRoughness = stoneMaterialRouhnessAOVal.r * materialMapVal.r
                           + rocksMaterialRouhnessAOVal.r * materialMapVal.g
                           + sandMaterialRouhnessAOVal.r * materialMapVal.b
                           + flowMaterialRouhnessAOVal.r * materialMapVal.a;
                           
    float dynamicAO = stoneMaterialRouhnessAOVal.g * materialMapVal.r
                    + rocksMaterialRouhnessAOVal.g * materialMapVal.g
                    + sandMaterialRouhnessAOVal.g * materialMapVal.b
                    + flowMaterialRouhnessAOVal.g * materialMapVal.a;
    
    dynamicAO = min(dynamicAO,lightMapVal.x); //adding global AO

    //float surfaceOrientationAngle = acos(dot(normalize(geometryMapVal.gb), normalize(vec2(1.0,0.0))));
    //surfaceOrientationAngle += 0.7853981634; // + pi/4 because for some reason angle is not good?
    
    //surfaceOrientationAngle = 0;
    
    vec2 horizontalDisplacement = vec2(clamp(dynamicHeight - 94.50,-10,10)) * 7.0 * horizontalDisplacementCoeff;
    
    horizontalDisplacement.x *= geometryMapVal.g;
    horizontalDisplacement.y *= geometryMapVal.b;
    
    heightHorizontalDisplacementAO = vec4(geometryMapVal.r + dynamicHeight,horizontalDisplacement,dynamicAO);

    //heightHorizontalDisplacementAO = vec4(geometryMapVal.r + dynamicHeight,0,0,dynamicAO); //the two 0.0 are for horizontal displacement
    
    albedoRoughness = vec4(dynamicAlbedo.r,dynamicAlbedo.g,dynamicAlbedo.b,dynamicRoughness);
    
    albedoRoughness = vec4(vec3(0.015) + dynamicAlbedo.rgb * 0.4,dynamicRoughness); // show reduced colors

    //albedoRoughness = vec4(geometryMapVal.g,geometryMapVal.b,0.0,dynamicRoughness); //debug to show what is in geometryMap
    //heightHorizontalDisplacementAO = vec4(geometryMapVal.r + dynamicHeight * 0.5,horizontalDisplacement * 0.5,dynamicAO);//debug to show reduced height from materials
    //heightHorizontalDisplacementAO = vec4(geometryMapVal.r + dynamicHeight * 1.5,horizontalDisplacement * 1.5,dynamicAO);//debug to show INCREASDE height from materials
    //albedoRoughness = vec4(materialMapDEBUG.rgb + materialMapDEBUG.aaa,dynamicRoughness); //show raw materialmap
    //albedoRoughness = vec4(vec3(0.5,0.5,mod(geometryMapVal.r,100.0)/200.0).bbr,0.3); // show isolines
    
    //albedoRoughness = vec4(vec3(0.5),dynamicRoughness); // show all white
    //heightHorizontalDisplacementAO = vec4(geometryMapVal.r,0.0,0.0,dynamicAO); //debug to remove height from materials
    //heightHorizontalDisplacementAO = vec4(texelFetch(geometryMap, ivec2(uvLocalToTexture * 2048), 0).r,0.0,0.0,dynamicAO); //debug to show basic voxels
}

