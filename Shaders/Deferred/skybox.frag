#version 410

out vec4 out_Color;

uniform vec3 campos;
uniform vec3 sunlightDirection;

in vec3 inoutpos;
in vec3 inoutposMinusCampos;
in float inoutposMinusCamposLENGTH; //actually constant i beleive ?

layout(location = 0) out vec3 position;
layout(location = 1) out vec4 albedoRoughness;
layout(location = 2) out vec3 normal;

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
    
    return starValue * 0.2;
}

void main()
{
    vec3 backgroundColor = vec3(0.0,0.0,0.005) + vec3(starField(normalize(inoutposMinusCampos)));
    position = inoutpos;
    albedoRoughness = vec4(backgroundColor,1.0);
    normal = vec3(0);
}
