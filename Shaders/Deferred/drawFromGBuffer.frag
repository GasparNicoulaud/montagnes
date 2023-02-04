#version 410

const float PI = 3.141592;
const float Epsilon = 0.00001;

//Shader Compile-time Constants
const vec3 sunColor = vec3(1.3,1.1,0.85) * 10;
const vec3 ambientIrradiance = vec3(0.585,0.650,0.987) * 0.3 * 1.3; // sky light
const vec3 abmientLightFactor = vec3(1,1,1) * 0.02; //true ambient light
const float fogIntensity = 0.08; //0.15 actually i think this controlls fog height

//bluish fog color vec3(0.535,0.650,0.927)
//bluer fog color vec3(0.585,0.650,0.987)
//reddish fog color vec3(0.75,0.4,0.3)
const vec3 atmosphereColor = vec3(0.585,0.650,0.987); //color of the fog

//Uniforms
uniform vec3 sunlightDirection;
uniform vec3 campos;

//textures from g buffer
uniform sampler2D positionSampler;
uniform sampler2D albedoRoughnessSampler;
uniform sampler2D normalSampler;

// other textures
uniform sampler2D globalLightMap;


/*possible new layout

pour le frag shader
uniform sampler2D albedoTexture + Roughness RGBA @ 8 bit per channel ?
uniform sampler2D normalTexture RGB @ 8 bit per channel for space maybe ?

pour le vtx shader
32 bits d'altitude de terrain
16 et 16 de horizontal displacement XY ?

*/

// output
out vec4 out_Color;

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

//// g0 and g1 are the two amplitude functions
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


// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha   = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(float cosTheta)
{
    return vec3(0.04) + (vec3(0.96) * pow(1.0 - cosTheta, 5.0));
}


vec3 applyFog(in vec3   rgb,      // original color of the pixel
              in float  pointdistance, // camera to point distance
              in vec3   rayDir,   // camera to point vector
              in vec3   camPos, // camera position
              in vec3   sunDir )  // sun light direction
{
    // heavy fog
    float density = 0.003;
    // ultrathin fog
    //density = 0.0001;
    // light-medium fog
    density = 0.0005;
    //ultra heavy fog
    density = 0.0015;

    const float c = fogIntensity;
    float fogAmount = c * exp(-camPos.z*density) * (1.0-exp(-pointdistance*rayDir.z*density ))/rayDir.z;

    fogAmount = clamp(fogAmount,0,1);

    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );

    vec3  fogColor  = mix(atmosphereColor, // base color
                          sunColor, // sun color
                          pow(sunAmount,8.0) * 0.5);

    return mix( rgb, fogColor, fogAmount );
}

// compute the near and far intersections of the cube (stored in the x and y components) using the slab method
// no intersection means vec.x > vec.y (really tNear > tFar)
vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir) {
    vec3 boxMin = vec3(-5000,-5000,-500);
    vec3 boxMax = vec3( 5000, 5000,1500);
    vec3 tMin = (boxMin - rayOrigin) / rayDir;
    vec3 tMax = (boxMax - rayOrigin) / rayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}

vec3 applyFogVolumetric(in vec3   rgb,      // original color of the pixel
                        in float  pointdistance, // camera to point distance
                        in vec3   rayDir,   // camera to point vector
                        in vec3   camPos, // camera position
                        in vec3   sunDir )  // sun light direction
{
    vec3 finalColor = rgb;

    vec2 intersections = intersectAABB(camPos,rayDir);
    float nearDistance = max(0,intersections.x);
    float farDistance = min(pointdistance,intersections.y);
    
    if(pointdistance > intersections.y){
        finalColor = applyFog(rgb,pointdistance,rayDir,camPos,sunDir);
    }
    
    if(nearDistance > farDistance){
      return applyFog(rgb,pointdistance,rayDir,camPos,sunDir);
    }
    
    vec3 nearPoint = camPos + rayDir * nearDistance;
    vec3 farPoint = camPos + rayDir * farDistance;
    
    #define STEP_COUNT 32.0
    
    float stepSize = (farDistance - nearDistance) / STEP_COUNT;
    
    for(int i=0; i< STEP_COUNT ; i++){
        vec3 curPoint = mix(nearPoint,farPoint,1.0 - i/ STEP_COUNT );
        float shadowHeight = texture2D_bicubic(globalLightMap,(curPoint.xy + vec2(5000.0))/10000.0).g;
//        float lightFactor = clamp((curPoint.z - (shadowHeight * 500 + 90)),0,1);
        float lightFactor = clamp((curPoint.z - (shadowHeight * 500 + 50)) * 0.05,0,1);
        vec3 fogColor = mix(atmosphereColor * 0.1,atmosphereColor,lightFactor);
        float density = exp(curPoint.z * -0.004) * 0.0015;
        
        //density = mix(0.1,0.00001,lightFactor);
        
        finalColor = mix(finalColor,fogColor, min(stepSize * density,1.0));
    }

    return finalColor;
//    return mix(finalColor, vec3(1,0,0), min(0.05 + (farDistance - nearDistance) * 0.0001,1.0));
}

void main(void)
{
    vec2 screenSize = vec2(3456,2234);
    vec3 position = texture(positionSampler,gl_FragCoord.xy/screenSize).xyz;
    vec4 albedoRoughness = texture(albedoRoughnessSampler,gl_FragCoord.xy/screenSize).rgba;
    vec3 normal = texture(normalSampler,gl_FragCoord.xy/screenSize).xyz;
    
    float shadowHeight = texture2D_bicubic(globalLightMap,(position.xy + vec2(5000.0))/10000.0).g;
//    float dropShadowFactor = texture2D_bicubic(globalLightMap,(position.xy + vec2(5000.0))/10000.0).g;
    float dropShadowFactor = clamp((position.z - (shadowHeight * 500 + 50)) * 0.05,0,1);

    vec3 albedo = albedoRoughness.rgb;
    float roughness = albedoRoughness.a;

    // Outgoing light direction (vector from world-space fragment position to the "eye").
    vec3 Lo = normalize(campos - position);
    // Angle between surface normal and outgoing light direction.
    float cosLo = max(0.0, dot(normal, Lo));
    // Half-vector between Li and Lo.
    vec3 Lh = normalize(sunlightDirection + Lo);
    vec3 LhSky = normalize(vec3(0.0,0.0,1.0) + Lo);
    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(normal, sunlightDirection));
    float cosLiSky = max(0.0, dot(normal, vec3(0.0,0.0,1.0)));
    float cosLh = max(0.0, dot(normal, Lh));
    float cosLhSky = max(0.0, dot(normal, LhSky));
    float LhDotLo = max(0.0, dot(Lh, Lo));
    float LhDotLoSky = max(0.0, dot(LhSky, Lo));
    // Calculate Fresnel term for direct lighting.
    vec3 F  = fresnelSchlick(LhDotLo);
    vec3 FSky  = fresnelSchlick(LhDotLoSky);

    // Calculate normal distribution for specular BRDF.
    float D = ndfGGX(cosLh, roughness);
    float DSky = ndfGGX(cosLhSky, roughness);
    // Calculate geometric attenuation for specular BRDF.
    float G = gaSchlickGGX(cosLi, cosLo, roughness);
    float GSky = gaSchlickGGX(cosLiSky, cosLo, roughness);
    vec3 kd = vec3(1.0) - F;
    vec3 kdSky = vec3(1.0) - FSky;
    // Cook-Torrance specular microfacet BRDF.
    vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * LhDotLo);
    vec3 specularBRDFSky = (FSky * DSky * GSky) / max(Epsilon, 4.0 * cosLiSky * LhDotLoSky);
    // Total contribution for directLighting.
    vec3 directLighting = (kd * albedo + specularBRDF) * sunColor * cosLi; //SUNLIGHT

    directLighting *= dropShadowFactor; //shadow from sun

    directLighting += (kdSky * albedo + specularBRDFSky) * ambientIrradiance * cosLiSky; //ambient sky directional

    //float AOfactor = heightHorizontalDisplacementAOVal.a;

    vec3 ambientLighting = albedo * abmientLightFactor;

    // Final fragment color befor fog
    vec3 PBRcolor = directLighting + ambientLighting;
    //PBRcolor *= AOfactor; //this line is incorect buuut it doenst look bad so ???


    // fog
    //PBRcolor = applyFog(PBRcolor,length(position - campos),normalize(position - campos),campos,sunlightDirection);
    PBRcolor = applyFogVolumetric(PBRcolor,length(position - campos),normalize(position - campos),campos,sunlightDirection);

    //hdr tone mapping
    out_Color.rgb = PBRcolor / (PBRcolor + vec3(1.0));
    out_Color.a = 1.0;
}


