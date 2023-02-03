#version 410

uniform sampler2D bhm;

layout(location = 0) out vec2 heightRocksSedimentsOut;
layout(location = 1) out vec4 waterPipesOut;
layout(location = 2) out vec4 sedimentPipesOut;


void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    vec2 here = texelFetch(bhm, fragCoord, 0).rg;
    
    float heightFromBhm = here.r;
    float sedFromBhm = here.g;
    float rocksHeight = heightFromBhm - sedFromBhm;
    
    heightRocksSedimentsOut.r = rocksHeight * 500.0;
    heightRocksSedimentsOut.g = sedFromBhm * 500.0;
    
    waterPipesOut = vec4(0);
    sedimentPipesOut = vec4(0);
}


