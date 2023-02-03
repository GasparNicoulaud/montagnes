#version 150 core

uniform usampler2D terrain;
out vec4 out_Color; //bhm

void main(void)
{
    float outc;
    uvec4 sampleres = texture(terrain,gl_FragCoord.xy/2048.0f);

    
    outc = ((float(sampleres.r))/ 10000.0f) - 2;
    
    out_Color = vec4(outc,0,sampleres.g/255.0,1);
}
