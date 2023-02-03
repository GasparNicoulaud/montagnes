#version 150

uniform sampler2D bhm;
out uvec4 out_Color;


void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    float outc = 0;
    
    //outc += texture(bhm, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 4;
    /*
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,1))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,0))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,1))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).r * 1;

    outc /= 16.0;
    */
    
    /*
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 41;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,1))/2048.0f).r * 26;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,0))/2048.0f).r * 26;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).r * 26;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).r * 26;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,1))/2048.0f).r * 16;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).r * 16;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).r * 16;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).r * 16;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,2))/2048.0f).r * 7;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 7;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,-2))/2048.0f).r * 7;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 7;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,2))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,2))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,-2))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,-2))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,1))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,-1))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,1))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,-1))/2048.0f).r * 4;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,2))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,-2))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,2))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,-2))/2048.0f).r * 1;
    
    outc /= 273.0;
     */
    
    /*outc += texture(bhm, (gl_FragCoord.xy + vec2(0,0))/2048.0f).r * 159;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,1))/2048.0f).r * 97;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,0))/2048.0f).r * 97;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).r * 97;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).r * 97;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,1))/2048.0f).r * 59;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).r * 59;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).r * 59;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).r * 59;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,2))/2048.0f).r * 22;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 22;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,-2))/2048.0f).r * 22;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,0))/2048.0f).r * 22;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,2))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,2))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,-2))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,-2))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,1))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,-1))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,1))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,-1))/2048.0f).r * 13;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,2))/2048.0f).r * 3;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(2,-2))/2048.0f).r * 3;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,2))/2048.0f).r * 3;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-2,-2))/2048.0f).r * 3;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,3))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(0,-3))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(3,0))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-3,0))/2048.0f).r * 2;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,3))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,3))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(1,-3))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-1,-3))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(3,1))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(3,-1))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-3,1))/2048.0f).r * 1;
    outc += texture(bhm, (gl_FragCoord.xy + vec2(-3,-1))/2048.0f).r * 1;
    
    outc /= 1003.0;*/
    
    //boxblur
    const int blurrad = 5;
    int divisor = (blurrad * 2 + 1) * (blurrad * 2 + 1);
        
        for(int x = blurrad * -1; x <= blurrad; x++)
        {
            for(int y = blurrad * -1; y <= blurrad; y++)
            {
                outc += texture(bhm, (gl_FragCoord.xy + vec2(x,y))/2048.0f).r;
                //divisor++;
            }
        }
    
    outc /= divisor;
    
    //outc = texture(bhm, gl_FragCoord.xy/2048.0f).r;
    
    outc += 1.0;
    
    //outc = sin(gl_FragCoord.x*0.01);
    
    outc *= 3000000;
    
    outc = abs(outc);
    
    out_Color = uvec4(int(outc),0,0,1);
}




