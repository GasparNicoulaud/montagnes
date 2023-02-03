#version 150 core

out vec4 out_Color;

uniform usampler2D blurredBhm;

void main(void)
{
    float normX = 0;
    float normY = 0;
    float convexity = 0;

    vec3 zeroOneMinusone = vec3(0,1.0,-1.0);
    float invTextureSize = 1.0/2048.0;
    float center = texture(blurredBhm, gl_FragCoord.xy * invTextureSize).r;
    float top = texture(blurredBhm, (gl_FragCoord.xy + zeroOneMinusone.xy) * invTextureSize).r;
    float right = texture(blurredBhm, (gl_FragCoord.xy + zeroOneMinusone.yx) * invTextureSize).r;
    float bottom = texture(blurredBhm, (gl_FragCoord.xy + zeroOneMinusone.xz) * invTextureSize).r;
    float left = texture(blurredBhm, (gl_FragCoord.xy + zeroOneMinusone.zx) * invTextureSize).r;

    normX = right - left;
    normY = top - bottom;

    convexity = center - (top + right + bottom + left) * 0.25;
    
    float uintToFloatRatio = 1.0/3000000.0;
    
    normX *= uintToFloatRatio;
    normY *= uintToFloatRatio;
    convexity *= uintToFloatRatio;
    
    out_Color = vec4(normX,normY,convexity,1);
}



