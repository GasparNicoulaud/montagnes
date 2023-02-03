#version 150

uniform sampler2D bhmCopy;
uniform usampler2D blurMap;
uniform float bluroffset;
out vec4 out_Color;

void main(void)
{
    
    vec3 bhmVal = texture(bhmCopy, gl_FragCoord.xy/2048.0f).rgb;
    float blurVal = float(texture(blurMap, gl_FragCoord.xy/2048.0f + vec2(0,0.0)).r);
    //blurVal += 50;
    blurVal /= 3000000.0; //2,147,483,647
    blurVal -= 1.0 + bluroffset; //-1 to seet to zero, the rest to adjust the blur height
    //float bhmVal = float(texelFetch(bhmCopy, ivec2((ivec2(gl_FragCoord.xy) + ivec2(0,0))/8), 0).r);
    //float blurVal = float(texelFetch(blurMap, ivec2((ivec2(gl_FragCoord.xy) + ivec2(0,0))/8), 0).r);
    
    //blurVal += 0.02;
    
    float outc = max(blurVal,bhmVal.r);
    float sedimentHeight = max(0,blurVal - bhmVal.r) + bhmVal.g;
    //float outc = blurVal / 10000.0;
    
    //outc = sin(gl_FragCoord.x/100.0) * 0.1 + bhmVal * 0.0001;

    //outc = blurVal;
    
    out_Color = vec4(outc,sedimentHeight,bhmVal.b,1);
}





