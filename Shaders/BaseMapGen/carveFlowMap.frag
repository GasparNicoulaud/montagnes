#version 150 core

out vec4 out_Color;

uniform sampler2D bhm;

uniform float strength;

void main(void)
{
    float invTextureSize = 1.0/2048.0;
    vec4 bhmValue = texture(bhm, gl_FragCoord.xy * invTextureSize).rgba;
    
    float samplingWidth = 0.5;
    float topHeight    = texture(bhm, (gl_FragCoord.xy + vec2( 0, 1) * samplingWidth) * invTextureSize).r;
    float bottomHeight = texture(bhm, (gl_FragCoord.xy + vec2( 0,-1) * samplingWidth) * invTextureSize).r;
    float leftHeight   = texture(bhm, (gl_FragCoord.xy + vec2(-1, 0) * samplingWidth) * invTextureSize).r;
    float rightHeight  = texture(bhm, (gl_FragCoord.xy + vec2( 1, 0) * samplingWidth) * invTextureSize).r;
    
    float slopeVert = topHeight - bottomHeight;
    float slopeHori = leftHeight - rightHeight;
    
    float slope = abs(slopeVert) + abs(slopeHori);
    
    slope *= 10.0;
    
    float flowToCarve = bhmValue.b;
    flowToCarve = max(0, flowToCarve - 2000);

    bhmValue.r -= sqrt(sqrt(flowToCarve)) * 0.005 * strength * min(slope + 0.1,1);
    
    bhmValue.b *= slope;
    
    out_Color = bhmValue;
}


