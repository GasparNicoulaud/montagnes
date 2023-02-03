
#version 150

uniform sampler2D materialMap;
out vec4 blurredMaterialMap;


void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    vec4 outc = vec4(0);
    
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
    
    // 7 x 7 gaussian kernel
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f) * 159;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,1))/2048.0f) * 97;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,0))/2048.0f) * 97;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,-1))/2048.0f) * 97;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,0))/2048.0f) * 97;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,1))/2048.0f) * 59;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,1))/2048.0f) * 59;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,-1))/2048.0f) * 59;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f) * 59;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,2))/2048.0f) * 22;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-2,0))/2048.0f) * 22;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,-2))/2048.0f) * 22;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-2,0))/2048.0f) * 22;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,2))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,2))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,-2))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,-2))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(2,1))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(2,-1))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-2,1))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-2,-1))/2048.0f) * 13;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(2,2))/2048.0f) * 3;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(2,-2))/2048.0f) * 3;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-2,2))/2048.0f) * 3;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-2,-2))/2048.0f) * 3;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,3))/2048.0f) * 2;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(0,-3))/2048.0f) * 2;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(3,0))/2048.0f) * 2;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-3,0))/2048.0f) * 2;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,3))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,3))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(1,-3))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-1,-3))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(3,1))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(3,-1))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-3,1))/2048.0f) * 1;
    outc += texture(materialMap, (gl_FragCoord.xy + vec2(-3,-1))/2048.0f) * 1;
    
    outc /= 1003.0;
    
    /*
    outc = vec4(0);
    //boxblur
    const int blurrad = 10;
    int divisor = (blurrad * 2 + 1) * (blurrad * 2 + 1);
        
    for(int x = blurrad * -1; x <= blurrad; x++)
    {
        for(int y = blurrad * -1; y <= blurrad; y++)
        {
            outc += texture(materialMap, (gl_FragCoord.xy + vec2(x,y))/2048.0f);
        }
    }
    
    outc /= divisor;
     */
    
    // lighter blur for flowmap, to retain more shapes
    outc.a = 0;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f).a * 4;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(0,1))/2048.0f).a * 2;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(1,0))/2048.0f).a * 2;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(0,-1))/2048.0f).a * 2;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(-1,0))/2048.0f).a * 2;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(1,1))/2048.0f).a * 1;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(-1,1))/2048.0f).a * 1;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(1,-1))/2048.0f).a * 1;
    outc.a += texture(materialMap, (gl_FragCoord.xy + vec2(-1,-1))/2048.0f).a * 1;
    outc.a /= 16.0;
    
    
    // don't blur flow
    //outc.a = texture(materialMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f).a;
    
    //outc = texture(bhm, gl_FragCoord.xy/2048.0f).r;
    
    //outc += 1.0;
    
    //outc = sin(gl_FragCoord.x*0.01);
    
    //outc *= 3000000;
    
    //outc = abs(outc);
    
    //don't blur anything
    //outc = texture(materialMap, (gl_FragCoord.xy + vec2(0,0))/2048.0f);
    
    blurredMaterialMap = outc;
}




