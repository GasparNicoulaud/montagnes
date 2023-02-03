#version 410 core

out vec4 materialMap;

uniform sampler2D bhm;

void main(void)
{
    float invTextureSize = 1.0/2048.0;
    vec4 bhmVal = texture(bhm, gl_FragCoord.xy * invTextureSize).rgba;
    
    
    vec4 top = texture(bhm, (gl_FragCoord.xy + vec2(0,1)) * invTextureSize).rgba;
    vec4 topRight = texture(bhm, (gl_FragCoord.xy + vec2(1,1)) * invTextureSize).rgba;
    vec4 right = texture(bhm, (gl_FragCoord.xy + vec2(1,0)) * invTextureSize).rgba;
    vec4 bottomRight = texture(bhm, (gl_FragCoord.xy + vec2(1,-1)) * invTextureSize).rgba;
    vec4 bottom = texture(bhm, (gl_FragCoord.xy + vec2(0,-1)) * invTextureSize).rgba;
    vec4 bottomLeft = texture(bhm, (gl_FragCoord.xy + vec2(-1,-1)) * invTextureSize).rgba;
    vec4 left = texture(bhm, (gl_FragCoord.xy + vec2(-1,0)) * invTextureSize).rgba;
    vec4 topLeft = texture(bhm, (gl_FragCoord.xy + vec2(-1,1)) * invTextureSize).rgba;
    
    float height = bhmVal.r;
    float sediment = bhmVal.g * 100.0;
    //sediment *= sediment;
    float flowtrace = bhmVal.b;
    
    //if terrain is nearly flat or convex, it can be sand
    float inhibitor = 20.0;
    float threshold = -0.002;
    float topDiff = top.r - bhmVal.r ;
    if(topDiff < threshold)
    {
        topDiff *= inhibitor;
    }
    float topRightDiff = topRight.r - bhmVal.r;
    if(topRightDiff < threshold)
    {
        topRightDiff *= inhibitor;
    }
    float rightDiff = right.r - bhmVal.r;
    if(rightDiff < threshold)
    {
        rightDiff *= inhibitor;
    }
    float bottomRightDiff = bottomRight.r - bhmVal.r;
    if(bottomRightDiff < threshold)
    {
        bottomRightDiff *= inhibitor;
    }
    float bottomDiff = bottom.r - bhmVal.r;
    if(bottomDiff < threshold)
    {
        bottomDiff *= inhibitor;
    }
    float bottomLeftDiff = bottomLeft.r - bhmVal.r;
    if(bottomLeftDiff < threshold)
    {
        bottomLeftDiff *= inhibitor;
    }
    float leftDiff = left.r - bhmVal.r;
    if(leftDiff < threshold)
    {
        leftDiff *= inhibitor;
    }
    float topLeftDiff = topLeft.r - bhmVal.r;
    if(topLeftDiff < threshold)
    {
        topLeftDiff *= inhibitor;
    }
    
    float convexity = topDiff + topRightDiff + rightDiff + bottomRightDiff + bottomDiff + bottomLeftDiff + leftDiff + topLeftDiff;
    convexity += 0.01;
    
    convexity *= 100.0;
    
    
    float topHeight    = top.r;
    float bottomHeight = bottom.r;
    float leftHeight   = left.r;
    float rightHeight  = right.r;
    
    float slopeVert = topHeight - bottomHeight;
    float slopeHori = leftHeight - rightHeight;
    
    float slope = abs(slopeVert) + abs(slopeHori);
    flowtrace = max(0, flowtrace - 2000);

    flowtrace = sqrt(sqrt(flowtrace)) * 1000.0 * slope;
    
    
    
    
    float stone = clamp(1.0 - sediment * 2,0.0,1.0);
    float sand = clamp(convexity * 1.0,0.0,1.0);
    float rocks = clamp((sediment - sand * 0.5) * 0.1,0.0,1.0);
    float flow = clamp(flowtrace * 1.0,0.0,100.0);;
    
    float exponent = 1.0;
    stone = pow(stone,exponent);
    rocks = pow(rocks,exponent);
    sand = pow(sand,exponent);
    flow = pow(flow,exponent);
    
    materialMap = vec4(stone,rocks,sand,flow);
    
    
    float gaussianSurroundingAverage = (right.r * 2.0 +  left.r * 2.0 +  top.r * 2.0 +  bottom.r * 2.0 + topRight.r + topLeft.r +  bottomRight.r + bottomLeft.r) / 12.0;
    float concavity = (gaussianSurroundingAverage - bhmVal.r) * 500.0; //concavity is around at 1.0 when slope is around 45°
    //concavity *= 10; //make concaivty more visible
    
    //materialMap = vec4(1.0,1.0,1.0,1.0); //debug to compare material heights
    
    const vec2 size = vec2(15.0,0.0);
    vec3 va = normalize(vec3(size.xy,(right.r-left.r) * 500.0));
    vec3 vb = normalize(vec3(size.yx,(top.r-bottom.r) * 500.0));
    vec3 surfaceNormal = normalize(cross(va,vb));
    float tiltAngle = acos(dot(surfaceNormal, vec3(0,0,1)));
    float flatness = max(0,0.5 - tiltAngle) * 2.0;
    if(right.r + 0.1 < bhmVal.r || left.r + 0.1 < bhmVal.r || top.r + 0.1 < bhmVal.r || bottom.r + 0.1 < bhmVal.r)
    {
        //flatness = 0;
    }
    
    //new method :
    materialMap = vec4(1,0,0,0);
    concavity = clamp(flatness,0,1);
    materialMap = mix(materialMap,vec4(0,1,0,0),flatness);
    convexity = clamp(sediment,0,1);
    materialMap = mix(materialMap,vec4(0,0,1,0),sediment);
    convexity = clamp(flowtrace,0,1);
    materialMap = mix(materialMap,vec4(0,0,0,1),flowtrace);
    
    /*//debug to show sediments
    if (bhmVal.g > 0)
    {
        materialMap = vec4(0,0,1,0);
    }
    else
    {
        materialMap = vec4(1,0,0,0);
    }
    */
    
    /*//simple flatness discriminator
    if (flatness > 0.2)
    {
        materialMap = vec4(0,0,1,0);
    }
    else
    {
        materialMap = vec4(1,0,0,0);
    }*/
    
    
    materialMap = normalize(materialMap);
    
    //materialMap = vec4(0.0,0.0,0.0,1.0);
}
