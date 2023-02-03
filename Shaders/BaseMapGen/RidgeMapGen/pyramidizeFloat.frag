#version 150

uniform sampler2D sampler;
uniform float slopeFactor;
out vec4 out_Color;

float sampleArea()
{
    //int maxrad = 10;
    
    float maxHeight = -999;
    
    float ridgeHit;
    
    float invTextureSize = 1.0/2048.0;
    
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-2,-2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,-2)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-2,-1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,-1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-2,0)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,0)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-2,1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-2,2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,2)) * slopeFactor));
    
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-1,-2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,-2)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-1,-1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,-1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-1,0)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,0)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-1,1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(-1,2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,2)) * slopeFactor));
    
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(0,-2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,-2)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(0,-1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,-1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(0,0)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,0)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(0,1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(0,2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,2)) * slopeFactor));
    
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(1,-2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,-2)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(1,-1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,-1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(1,0)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,0)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(1,1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(1,2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,2)) * slopeFactor));
    
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(2,-2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,-2)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(2,-1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,-1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(2,0)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,0)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(2,1)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,1)) * slopeFactor));
    ridgeHit = texture(sampler, (gl_FragCoord.xy + ivec2(2,2)) * invTextureSize).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,2)) * slopeFactor));
    
    return maxHeight;
}

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    float invTextureSize = 1.0/2048.0;
    
    float outc = sampleArea();
    
    float outg = texture(sampler, gl_FragCoord.xy * invTextureSize).g;
    float outb = texture(sampler, gl_FragCoord.xy * invTextureSize).b;
    
    out_Color = vec4(outc,outg,outb,1);
}





