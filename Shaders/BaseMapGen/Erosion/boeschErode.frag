#version 150 

uniform sampler2D bhm;
uniform sampler2D hardness;

out vec4 out_Color;

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    float invTextureSize = 1.0/2048.0;
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    float weight = texelFetch(hardness, fragCoord, 0).r;
    vec3 us = texelFetch(bhm, fragCoord, 0).rgb;
    
    float left = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.zx) * invTextureSize).r;
    float right = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.yx) * invTextureSize).r;
    float top = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.xy) * invTextureSize).r;
    float bottom = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.xz) * invTextureSize).r;
    float left_top = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.zy) * invTextureSize).r;
    float right_top = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.yy) * invTextureSize).r;
    float left_bottom = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.zz) * invTextureSize).r;
    float right_bottom = texture(bhm, (gl_FragCoord.xy + zeroOneMinusOne.yz) * invTextureSize).r;
    
    vec4 a = vec4(left, right, top, bottom);
    vec4 b = vec4(left_top, right_top, left_bottom, right_bottom);
    
    vec4 comparision;
    float count = 1.0;
    float sum = us.r;
    float result;

    comparision = vec4(lessThan(a, vec4(us.r)));
    count += dot(comparision, comparision);
    sum += dot(comparision, a);

    comparision = vec4(lessThan(b, vec4(us.r)));
    count += dot(comparision, comparision);
    sum += dot(comparision, b);
    
    float magickNumber = 0.01f; // should be ([how many bhm units for a side of bhm map] / [2048])      * 2
    //magickNumber = 0.1;
    
    vec3 normal = normalize(vec3(left - right, magickNumber ,bottom - top));
    
    float factor = dot(normal, vec3(0.0, 1.0, 0.0));

    factor = factor - 0.05 * count;

    result = mix(sum/count, us.r, factor);
    
    result = mix(result, us.r, clamp(weight, 0.0, 1.0));

    out_Color = vec4(result,us.g,us.b,1.0);
}
