#version 150 core

out vec4 roughnessAO; //r is masked off as we only want to write to the g channel

uniform sampler2D albedoHeight;

void main(void)
{
    float ambientLight;
    
    vec3 zeroOneMinusone = vec3(0,1.0,-1.0);
    float invTextureSize = 1.0/2048.0;
    float center = texture(albedoHeight, gl_FragCoord.xy * invTextureSize).a;
    
    float accumulator = 0.0;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-2,-2)) * invTextureSize).a *  1;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-1,-2)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 0,-2)) * invTextureSize).a *  7;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 1,-2)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 2,-2)) * invTextureSize).a *  1;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-2,-1)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-1,-1)) * invTextureSize).a * 16;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 0,-1)) * invTextureSize).a * 26;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 1,-1)) * invTextureSize).a * 16;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 2,-1)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-2, 0)) * invTextureSize).a *  7;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-1, 0)) * invTextureSize).a * 26;
    //float tex22 = texture(geometryMap, (gl_FragCoord.xy + vec2( 0, 0)) * invTextureSize).r; //center already sampled
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 1, 0)) * invTextureSize).a * 26;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 2, 0)) * invTextureSize).a *  7;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-2, 1)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-1, 1)) * invTextureSize).a * 16;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 0, 1)) * invTextureSize).a * 26;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 1, 1)) * invTextureSize).a * 16;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 2, 1)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-2, 2)) * invTextureSize).a *  1;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2(-1, 2)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 0, 2)) * invTextureSize).a *  7;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 1, 2)) * invTextureSize).a *  4;
    accumulator += texture(albedoHeight, (gl_FragCoord.xy + vec2( 2, 2)) * invTextureSize).a *  1;

    //232
    
    float scaling = 70.0f;
    ambientLight = (center - accumulator/232.0f) * scaling;
    ambientLight += 0.5f;
    
    //ambientLight = 0.5f;

    ambientLight = clamp(ambientLight,0.0f,1.0f);
    roughnessAO = vec4(0.0,ambientLight,0.0,1.0);  //r is masked off as we only want to write to the g channel
}

