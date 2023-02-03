#version 410

uniform sampler2D heightRocksSediments;

out vec4 bhm;

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    vec2 here = texelFetch(heightRocksSediments, fragCoord, 0).rg;
    
    float sediments = max(0,here.g - 3.0);
    sediments *= 2;
    bhm = vec4((here.r + here.g) / 500.0,sediments / 500.0,0,0);
}
/*
 
 this weird bug again
 
 
 */
