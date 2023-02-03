#version 150 core

out vec4 ambientOcclusion;

uniform sampler2D geometryMap;

void main(void)
{
    float ambientLight;
    
    vec3 zeroOneMinusone = vec3(0,1.0,-1.0);
    float invTextureSize = 1.0/2048.0;
    float center = texture(geometryMap, gl_FragCoord.xy * invTextureSize).r;
    
    float accumulator = 0.0;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-2,-2)) * invTextureSize).r *  1;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-1,-2)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 0,-2)) * invTextureSize).r *  7;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 1,-2)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 2,-2)) * invTextureSize).r *  1;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-2,-1)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-1,-1)) * invTextureSize).r * 16;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 0,-1)) * invTextureSize).r * 26;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 1,-1)) * invTextureSize).r * 16;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 2,-1)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-2, 0)) * invTextureSize).r *  7;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-1, 0)) * invTextureSize).r * 26;
    //float tex22 = texture(geometryMap, (gl_FragCoord.xy + vec2( 0, 0)) * invTextureSize).r; //center already sampled
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 1, 0)) * invTextureSize).r * 26;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 2, 0)) * invTextureSize).r *  7;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-2, 1)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-1, 1)) * invTextureSize).r * 16;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 0, 1)) * invTextureSize).r * 26;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 1, 1)) * invTextureSize).r * 16;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 2, 1)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-2, 2)) * invTextureSize).r *  1;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2(-1, 2)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 0, 2)) * invTextureSize).r *  7;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 1, 2)) * invTextureSize).r *  4;
    accumulator += texture(geometryMap, (gl_FragCoord.xy + vec2( 2, 2)) * invTextureSize).r *  1;

    //232
    
    float scaling = 50.0f;
    ambientLight = (center - accumulator/232.0f) * scaling;
    ambientLight += 0.5f;
    ambientLight = clamp(ambientLight,0.0f,1.0f);

    ambientOcclusion = vec4(ambientLight,ambientLight,ambientLight,1.0);
}
