#version 150 core

out vec4 out_Color;

uniform sampler2D heightMap;
uniform float scale;

// from http://www.java-gaming.org/index.php?topic=35123.0
vec4 cubic(float v){
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}


vec4 textureBicubic(sampler2D sampler, vec2 texCoords){

   vec2 texSize = textureSize(sampler, 0);
   vec2 invTexSize = 1.0 / texSize;

   texCoords = texCoords * texSize - 0.5;


    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4 (xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture(sampler, offset.xz);
    vec4 sample1 = texture(sampler, offset.yz);
    vec4 sample2 = texture(sampler, offset.xw);
    vec4 sample3 = texture(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
       mix(sample3, sample2, sx), mix(sample1, sample0, sx)
    , sy);
}

void main(void)
{
    vec2 textSize = textureSize(heightMap, 0);
    
    vec3 hereOnLocalHeightmap = texture(heightMap,gl_FragCoord.xy/textSize.x).rgb;
    //vec3 hereOnLocalHeightmap = texture(heightMap,gl_FragCoord.xy).rgb;

    float zStretch = (1.0f/scale);
    
    const float nd = 0.003f; // should be 1.5f / texture size 
    
    const float sampleScaling = 1.0f/10000.0f;
    //const float sampleScaling = 0.1;
    //sampleScaling =
    
    vec2 aorig = gl_FragCoord.xy/textSize.x + vec2(-nd,0);
    vec2 borig = gl_FragCoord.xy/textSize.x + vec2(nd,0);
    vec2 corig = gl_FragCoord.xy/textSize.x + vec2(0,-nd);
    vec2 dorig = gl_FragCoord.xy/textSize.x + vec2(0,nd);
    //vec3 a = vec3(aorig,texture(heightMap, aorig).x * (sampleScaling * zStretch));
    //vec3 b = vec3(borig,texture(heightMap, borig).x * (sampleScaling * zStretch));
    //vec3 c = vec3(corig,texture(heightMap, corig).x * (sampleScaling * zStretch));
    vec3 d = vec3(dorig,texture(heightMap, dorig).x * (sampleScaling * zStretch));
    
    
    vec3 a = vec3(aorig + texture(heightMap, aorig).yz * (sampleScaling * zStretch),texture(heightMap, aorig).x * (sampleScaling * zStretch));
    vec3 b = vec3(borig + texture(heightMap, borig).yz * (sampleScaling * zStretch),texture(heightMap, borig).x * (sampleScaling * zStretch));
    vec3 c = vec3(corig + texture(heightMap, corig).yz * (sampleScaling * zStretch),texture(heightMap, corig).x * (sampleScaling * zStretch));
    //float light = normalize(cross(b - a, d - c)).z;
    
    //if(abs(light) > 0.95) // snow threshold
    //{
        //materialColor = vec3(0.97,0.98,1); //uncoment to get snow
        //materialColor = vec3(fract(pos.z/10.0),0.98,1);
    //}
    
    
    
    
    //zStretch = 1.0f;///inoutscale;
    
    /*
    const vec2 size = vec2(nd,0.0);
    vec3 va = normalize(vec3(size.xy,b.z-a.z));
    vec3 vb = normalize(vec3(size.yx,d.z-c.z));
    vec3 normal = cross(va,vb);// + vec3(0.5,0.5,0);
    */
    
    //normal = normalize(normal);
    
    vec3  normal;
    vec3  vector1;
    vec3  vector2;

    vector1 = b - a;
    vector2 = c - a;
    normal = normalize (cross (vector1, vector2)) * -1.0;
    
    
    out_Color = vec4(normal.x,normal.y,normal.z,1);
    //out_Color = vec4(100,1,0,1);
}

