#version 410 core
//precision highp float;
// Based on https://learnopengl.com/PBR/Theory
//in vec2 TexCoords;
//in vec3 WorldPos;
//in vec3 Normal;

in vec3 position; //WorldPos
in vec2 fixedSamplingCoord; //TexCoords
in vec2 uvForStaticmapSampling; //TexCoords

// output
out vec4 out_Color;

//layout (location =0) out vec4 fragColour;

// material parameters
//uniform vec3 albedo;
//uniform float metallic;
//uniform float roughness;
//uniform float ao;

// lights
//uniform vec3 lightPositions[4];
//uniform vec3 lightColors[4];

//uniform vec3 cameraPosition;
//uniform float exposure;

//Shader Compile-time Constants
const vec3 sunColor = vec3(1.0,0.95,0.90) *4;
const vec3 ambientIrradiance = vec3(0.585,0.650,0.987) * 0.3;

const vec3 atmosphereColor = vec3(0.585,0.650,0.987);

//Uniforms
uniform vec3 sunlightDirection;
uniform vec3 campos;

//textures
uniform sampler2D localHeightmap;
uniform sampler2D localNormalmap;
uniform sampler2D globalLightMap;
uniform sampler2D heightHorizontalDisplacementAO;
uniform sampler2D albedoRoughness;

const float PI = 3.14159265359;

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

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
    vec3 N = texture(localNormalmap,fixedSamplingCoord).xyz;
    vec3 V = normalize(campos - position);
    vec3 R = reflect(-V, N);
    
    vec3 albedo = texture(albedoRoughness,fixedSamplingCoord).rgb;
    float roughness = texture(albedoRoughness,fixedSamplingCoord).a;
    float lightmapVal = texture2D_bicubic(globalLightMap,uvForStaticmapSampling).r;
    
    //albedo = vec3(0.1);
    //roughness = 1.0;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    
    // calculate per-light radiance
    vec3 L = sunlightDirection; //maybe * -1 ?
    vec3 H = normalize(V + L);
    vec3 radiance = sunColor;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(V, H), 0.0) * max(dot(L, N), 0.0) + 0.001f; // 0.001 to prevent divide by zero.
    vec3 brdf = nominator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    //kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + brdf) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = ambientIrradiance * albedo * lightmapVal;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    
    //gamma correction
    //out_Color.rgb = pow(out_Color.rgb,vec3(1.0/2.2)); //not needed, done by opengl through   glEnable(GL_FRAMEBUFFER_SRGB);

    
    out_Color = vec4(color, 1.0);
}
