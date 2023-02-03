#version 150

uniform usampler2D blurMap;
out uvec4 out_Color;


void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    float outc = 0;
    
    //outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 4;
    /*
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,1))/2048.0f).r * 2;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,0))/2048.0f).r * 2;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).r * 2;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).r * 2;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,1))/2048.0f).r * 1;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).r * 1;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).r * 1;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).r * 1;
     
     outc /= 16.0;
     */
    
    /*
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 41;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,1))/2048.0f).r * 26;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,0))/2048.0f).r * 26;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).r * 26;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).r * 26;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,1))/2048.0f).r * 16;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).r * 16;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).r * 16;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).r * 16;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,2))/2048.0f).r * 7;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 7;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,-2))/2048.0f).r * 7;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 7;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,2))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,2))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,-2))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,-2))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,1))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,-1))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,1))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,-1))/2048.0f).r * 4;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,2))/2048.0f).r * 1;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,-2))/2048.0f).r * 1;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,2))/2048.0f).r * 1;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,-2))/2048.0f).r * 1;
     
     outc /= 273.0;
     */
    /*
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 159.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,1))/2048.0f).r * 97.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,0))/2048.0f).r * 97.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).r * 97.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).r * 97.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,1))/2048.0f).r * 59.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).r * 59.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).r * 59.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).r * 59.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,2))/2048.0f).r * 22.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 22.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,-2))/2048.0f).r * 22.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,0))/2048.0f).r * 22.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,2))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,2))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,-2))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,-2))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,1))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,-1))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,1))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,-1))/2048.0f).r * 13.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,2))/2048.0f).r * 3.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(2,-2))/2048.0f).r * 3.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,2))/2048.0f).r * 3.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-2,-2))/2048.0f).r * 3.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,3))/2048.0f).r * 2.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(0,-3))/2048.0f).r * 2.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(3,0))/2048.0f).r * 2.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-3,0))/2048.0f).r * 2.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,3))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,3))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(1,-3))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-1,-3))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(3,1))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(3,-1))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-3,1))/2048.0f).r * 1.0;
     outc += texture(blurMap, (gl_FragCoord.xy + vec2(-3,-1))/2048.0f).r * 1.0;
     
     outc /= 1003.0;
     */
    
    //boxblur
    const int blurrad = 5;
    int divisor = (blurrad * 2 + 1) * (blurrad * 2 + 1);
    divisor = 0;
    
    for(int x = blurrad * -1; x <= blurrad; x+=5)
    {
        for(int y = blurrad * -1; y <= blurrad; y+=5)
        {
            outc += texture(blurMap, (gl_FragCoord.xy + vec2(x,y))/2048.0f).r;
            divisor++;
        }
    }
    
    outc /= divisor;
    
    
    //outc = texture(blurMap, gl_FragCoord.xy/2048.0f).r;
    
    //outc += 1.0;
    
    //outc = sin(gl_FragCoord.x*0.01);
    
    //outc *= 30000;
    
    outc = abs(outc);
    
    out_Color = uvec4(int(outc),0,0,1);
}





