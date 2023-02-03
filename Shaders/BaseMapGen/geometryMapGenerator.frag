#version 410 core

out vec4 geometryMap;

uniform sampler2D bhm;

void main(void)
{
    float invTextureSize = 1.0/2048.0;
    float center = texture(bhm, gl_FragCoord.xy * invTextureSize).r;
    
    const float nd = 0.0002f; // should be 1.5f / texture size
    
    vec2 aorig = gl_FragCoord.xy * invTextureSize + vec2(-nd,0);
    vec2 borig = gl_FragCoord.xy * invTextureSize + vec2(nd,0);
    vec2 corig = gl_FragCoord.xy * invTextureSize + vec2(0,-nd);
    vec2 dorig = gl_FragCoord.xy * invTextureSize + vec2(0,nd);
    vec3 a = vec3(aorig,texture(bhm, aorig).r);
    vec3 b = vec3(borig,texture(bhm, borig).r);
    vec3 c = vec3(corig,texture(bhm, corig).r);
    vec3 d = vec3(dorig,texture(bhm, dorig).r);
    
    const vec2 size = vec2(nd * 250,0.0);
    vec3 va = normalize(vec3(size.xy,b.z-a.z));
    vec3 vb = normalize(vec3(size.yx,d.z-c.z));
    vec3 normal = cross(va,vb);
    
    //normal.xy = vec2(0.0,1.0);
    
    geometryMap = vec4(center * 500,normal.x,normal.y,1.0);
}
/*
 
 padding for the weiiirrd bug
 
 
 
 
 
 
 */
