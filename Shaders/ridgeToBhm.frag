#version 150

uniform usampler2D pyramidized;
out vec4 out_Color;

void main(void)
{
    uint uintval = texture(pyramidized, gl_FragCoord.xy/2048.0f).r;
    float outc = float(pow(uintval * 0.0001,5)) * 0.5;
    //outc = sin(gl_FragCoord.x/20.0f) * uintval;
    
    outc = uintval * 0.0002;
    
    outc = pow(outc/6.1,8) * 1.0;
    
    out_Color = vec4(outc,0,0,1);
}



