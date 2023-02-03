#version 150

uniform sampler2D bhm;
uniform float slopeFactor;
out vec4 out_Color;

float sampleArea(ivec2 fragCoord)
{
    float minHeight = 999;
    float invTextureSize = 1.0/2048.0;
    
    float ridgeHit;
    
    /*float slopeFactor = 0.004; // default kinda sharp
    slopeFactor = 0.0035; // less sharp
    slopeFactor = 0.0025; // even less sharp*/

    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-2,-2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-2,-2)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-2,-1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-2,-1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-2,0)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-2,0)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-2,1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-2,1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-2,2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-2,2)) * slopeFactor));
    
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-1,-2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-1,-2)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-1,-1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-1,-1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-1,0)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-1,0)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-1,1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-1,1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(-1,2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(-1,2)) * slopeFactor));
    
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(0,-2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(0,-2)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(0,-1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(0,-1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(0,0)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(0,0)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(0,1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(0,1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(0,2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(0,2)) * slopeFactor));
    
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(1,-2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(1,-2)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(1,-1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(1,-1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(1,0)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(1,0)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(1,1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(1,1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(1,2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(1,2)) * slopeFactor));
    
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(2,-2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(2,-2)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(2,-1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(2,-1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(2,0)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(2,0)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(2,1)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(2,1)) * slopeFactor));
    ridgeHit = texture(bhm, (gl_FragCoord.xy + vec2(2,2)) * invTextureSize).r;
    minHeight = min(minHeight, ridgeHit + (length(ivec2(2,2)) * slopeFactor));
    
    return minHeight;
}

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    float outc = sampleArea(fragCoord);
    
    float outg = texelFetchOffset(bhm, fragCoord, 0, ivec2(0,0)).g;
    float outb = texelFetchOffset(bhm, fragCoord, 0, ivec2(0,0)).b;
    
    out_Color = vec4(outc,outg,outb,1);
}





