#version 150 core

out vec4 out_Color;

uniform vec3 campos;
uniform vec3 sunlightDirection;

in vec3 inoutposMinusCampos;
in float inoutposMinusCamposLENGTH; //actually constant i beleive ?

const vec3 atmosphereColor = vec3(0.585,0.650,0.987);

vec3 applyFogWithBackground
             (in vec3   rgb,      // original color of the pixel
              in float  pointdistance, // camera to point distance
              in vec3   rayDir,   // camera to point vector
              in vec3   camPos, // camera position
              in vec3   sunDir )  // sun light direction
{
    //density seems to actually control how much packed near the ground the fog is so maybe c is the real density ?
    // heavy fog
    float density = 0.003;
    // ultrathin fog
    density = 0.0001;
    // medium fog
    //density = 0.001;
    // light-medium fog
    density = 0.0005;
    //ultra heavy fog
    density = 0.0005;
    density = 0.001;
    // like nearly no fog fog
    //density = 0.000005;
    
    

    //gamma non corrected (linear space) atmosphere color : vec3(0.535,0.800,0.927)
    
    /* // rings iin the sky
    float bandwidth = 0.1;
    float invBandwidth = 1.0/bandwidth;
    float sunheight = 0.3;;
    
    if(rayDir.z > sunheight && rayDir.z < sunheight + bandwidth)
    {
        rayDir = mix(sunDir,rayDir,(rayDir.z - sunheight) * invBandwidth);
    }
    else if(rayDir.z > (sunheight - bandwidth) && rayDir.z < sunheight)
    {
        rayDir = mix(rayDir,sunDir,(rayDir.z - (sunheight - bandwidth)) * invBandwidth);
    }
    
     sunheight = 0.4;
    
    if(rayDir.z > sunheight && rayDir.z < sunheight + bandwidth)
    {
        rayDir = mix(sunDir,rayDir,(rayDir.z - sunheight) * invBandwidth);
    }
    else if(rayDir.z > (sunheight - bandwidth) && rayDir.z < sunheight)
    {
        rayDir = mix(rayDir,sunDir,(rayDir.z - (sunheight - bandwidth)) * invBandwidth);
    }
    
    sunheight = 0.46;
    
    if(rayDir.z > sunheight && rayDir.z < sunheight + bandwidth)
    {
        rayDir = mix(sunDir,rayDir,(rayDir.z - sunheight) * invBandwidth);
    }
    else if(rayDir.z > (sunheight - bandwidth) && rayDir.z < sunheight)
    {
        rayDir = mix(rayDir,sunDir,(rayDir.z - (sunheight - bandwidth)) * invBandwidth);
    }
    
    sunheight = 0.5;
    
    if(rayDir.z > sunheight && rayDir.z < sunheight + bandwidth)
    {
        rayDir = mix(sunDir,rayDir,(rayDir.z - sunheight) * invBandwidth);
    }
    else if(rayDir.z > (sunheight - bandwidth) && rayDir.z < sunheight)
    {
        rayDir = mix(rayDir,sunDir,(rayDir.z - (sunheight - bandwidth)) * invBandwidth);
    }
     */
    
    
    
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    //bluish fog color vec3(0.535,0.650,0.927)
    //bluer fog color vec3(0.585,0.650,0.987)
    //reddish fog color vec3(0.75,0.4,0.3)
    vec3  fogColor  = mix( atmosphereColor, // background color
                          vec3(1.0,0.95,0.90) * 4, // sun color
                          pow(sunAmount,8.0) );
    
    //                          pow(sunAmount,8.0) );

    
    //actually seems to be a good globa fog amout control
    float c = 0.15; //????? seems to only affect altitude fog computation or maybe fog fallof??
    //c = 0.4;
    //c = 0.2;
    //c = 0;
    //c = 1.5;

    //optimisation note, exp2 is quite faster than exp but gives different result
    float fogAmount = c * exp(-camPos.z*density) * (1.0-exp( -pointdistance*rayDir.z*density ))/rayDir.z;
    
    fogAmount = clamp(fogAmount + pow(sunAmount,4096),0,1); //sun exponent 4096
    
    return mix( rgb, fogColor, fogAmount );
}

float oneLinerHash(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898,78.233)))* 43758.5453123);
}

float starField(vec3 rayDir)
{
    rayDir = floor(rayDir * 800);
    float starValue = oneLinerHash(rayDir.xy + vec2(oneLinerHash(rayDir.zz)));
    
    float threshold = 0.005;
    starValue -= 1.0 - threshold;
    
    starValue = max(starValue,0);
    
    starValue *= 1.0/threshold;
    
    return starValue * 0.1;
}

void main()
{
    vec3 backgroundColor = vec3(0.0,0.0,0.005);
    
    //backgroundColor = vec3(0.2,0.2,0.21);
    
    backgroundColor += vec3(starField(normalize(inoutposMinusCampos)));
    
    //applyflog takes 0.48ms maybe ?
    out_Color.rgb = applyFogWithBackground(backgroundColor,inoutposMinusCamposLENGTH,normalize(inoutposMinusCampos),campos,sunlightDirection);
    out_Color.a = 1.0;
    
    //contrast
    //out_Color.rgb = ((out_Color.rgb - 0.5f) * 1.03) + 0.5f;
    
    out_Color.rgb *= 2.0; //exposure
    
    //hdr tone mapping ?
    out_Color.rgb = out_Color.rgb / (out_Color.rgb + vec3(1.0));
    // tone mapping
    //out_Color.rgb = vec3(1.0) - exp(-out_Color.rgb * 2.0);
    
    //out_Color.rgb = vec3(0.5) - out_Color.rgb;
}

