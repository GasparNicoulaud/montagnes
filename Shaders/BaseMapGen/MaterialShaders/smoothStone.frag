#version 410 core

layout(location = 0) out vec4 albedoHeight;
layout(location = 1) out vec4 roughnessAO;

uniform int seed;

uniform vec3 baseColor;
uniform vec3 modulatedColor;

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

float rand1dTo1d(float value)
{
    float random = fract(sin(value + 0.546) * 143758.5453);
    return random;
}

//dotdir default :  = vec3(12.9898, 78.233, 37.719)
float rand3dTo1d(vec3 value, vec3 dotDir)
{
    float random = fract(sin(dot(cos(value), dotDir)) * 143758.5453);
    return random;
}

float rand2dTo1d(vec2 value, vec2 dotDir){
    vec2 smallValue = cos(value);
    float random = dot(smallValue, dotDir);
    random = fract(sin(random) * 143758.5453);
    return random;
}

vec2 rand2dTo2d(vec2 value){
    return vec2(
        rand2dTo1d(value, vec2(12.989, 78.233)),
        rand2dTo1d(value, vec2(39.346, 11.135))
    );
}

vec3 rand3dTo3d(vec3 value)
{
    return vec3(
        rand3dTo1d(value, vec3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, vec3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, vec3(73.156, 52.235, 09.151))
    );
}

vec3 voronoiNoise(vec3 value, vec3 period)
{
    vec3 baseCell = floor(value);

    //first pass to find the closest cell
    float minDistToCell = 10;
    vec3 toClosestCell;
    vec3 closestCell;
    for(int x1=-1; x1<=1; x1++){
        for(int y1=-1; y1<=1; y1++){
            for(int z1=-1; z1<=1; z1++){
                vec3 cell = baseCell + vec3(x1, y1, z1);
                vec3 tiledCell = mod(cell, period);
                vec3 cellPosition = cell + rand3dTo3d(tiledCell);
                vec3 toCell = cellPosition - value;
                float distToCell = length(toCell);
                if(distToCell < minDistToCell){
                    minDistToCell = distToCell;
                    closestCell = cell;
                    toClosestCell = toCell;
                }
            }
        }
    }

    //second pass to find the distance to the closest edge
    float minEdgeDistance = 10;
    for(int x2=-1; x2<=1; x2++){
        for(int y2=-1; y2<=1; y2++){
            for(int z2=-1; z2<=1; z2++){
                vec3 cell = baseCell + vec3(x2, y2, z2);
                vec3 tiledCell = mod(cell, period);
                vec3 cellPosition = cell + rand3dTo3d(tiledCell);
                vec3 toCell = cellPosition - value;

                vec3 diffToClosestCell = abs(closestCell - cell);
                bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y + diffToClosestCell.z < 0.1;
                if(!isClosestCell){
                    vec3 toCenter = (toClosestCell + toCell) * 0.5;
                    vec3 cellDifference = normalize(toCell - toClosestCell);
                    float edgeDistance = dot(toCenter, cellDifference);
                    minEdgeDistance = min(minEdgeDistance, edgeDistance);
                }
            }
        }
    }

    float random = rand3dTo1d(mod(closestCell,period),vec3(12.9898, 78.233, 37.719));
    //minEdgeDistance = sqrt(sqrt(sqrt(minEdgeDistance))); //ADDED BY ME BUT NOT HERE ORIGINALLY
    return vec3(minDistToCell, random, minEdgeDistance);
}

float easeIn(float interpolator){
    return interpolator * interpolator;
}

float easeOut(float interpolator){
    return 1 - easeIn(1 - interpolator);
}

float easeInOut(float interpolator){
    float easeInValue = easeIn(interpolator);
    float easeOutValue = easeOut(interpolator);
    return mix(easeInValue, easeOutValue, interpolator);
}

float perlinNoisePeriodic(vec2 value, vec2 period){
    vec2 cellsMimimum = floor(value);
    vec2 cellsMaximum = ceil(value);

    cellsMimimum = mod(cellsMimimum, period);
    cellsMaximum = mod(cellsMaximum, period);

    //generate random directions
    vec2 lowerLeftDirection = rand2dTo2d(vec2(cellsMimimum.x, cellsMimimum.y)) * 2 - 1;
    vec2 lowerRightDirection = rand2dTo2d(vec2(cellsMaximum.x, cellsMimimum.y)) * 2 - 1;
    vec2 upperLeftDirection = rand2dTo2d(vec2(cellsMimimum.x, cellsMaximum.y)) * 2 - 1;
    vec2 upperRightDirection = rand2dTo2d(vec2(cellsMaximum.x, cellsMaximum.y)) * 2 - 1;

    vec2 fraction = fract(value);

    //get values of cells based on fraction and cell directions
    float lowerLeftFunctionValue = dot(lowerLeftDirection, fraction - vec2(0, 0));
    float lowerRightFunctionValue = dot(lowerRightDirection, fraction - vec2(1, 0));
    float upperLeftFunctionValue = dot(upperLeftDirection, fraction - vec2(0, 1));
    float upperRightFunctionValue = dot(upperRightDirection, fraction - vec2(1, 1));

    float interpolatorX = easeInOut(fraction.x);
    float interpolatorY = easeInOut(fraction.y);

    //interpolate between values
    float lowerCells = mix(lowerLeftFunctionValue, lowerRightFunctionValue, interpolatorX);
    float upperCells = mix(upperLeftFunctionValue, upperRightFunctionValue, interpolatorX);

    float noise = mix(lowerCells, upperCells, interpolatorY);
    return noise;
}

float FBMPeriodic(vec2 value){
    float noise = 0;
    float frequency = 10;
    float factor = 1.0/frequency;

    for(int i=0; i<15; i++){
        noise += perlinNoisePeriodic(value * frequency + i * 0.72354, vec2(1.0) * frequency) * factor;
        factor *= 0.49;
        frequency *= 2.0;
    }

    return noise * 4.0;
}

float FBMPeriodicWithFreq(vec2 value, float frequency){
    float noise = 0;
    //float frequency = 10;
    float factor = 1.0/frequency;

    for(int i=0; i<15; i++){
        noise += perlinNoisePeriodic(value * frequency + i * 0.72354, vec2(1.0) * frequency) * factor;
        factor *= 0.49;
        frequency *= 2.0;
    }

    return noise * 4.0;
}


void main( void )
{
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,1337.1337);
    brseedAbs *= 0.01;
    
    float height = 0;
    float voroFreq;
    vec3 voroPeriod;
    vec2 voroWarping;
    
    voroFreq = 1.0/512.0;
    voroPeriod = vec3(4);
    voroWarping = vec2(FBMPeriodic(gl_FragCoord.xy / 2048.0) * 2 + 20,FBMPeriodic(gl_FragCoord.xy / 2048.0) * 2 + 20) * 64.0;
    height = max(voronoiNoise(vec3(gl_FragCoord.xy + voroWarping,brseedAbs + 1) * voroFreq,voroPeriod).z * 0.8 - 0.10,height);
    voroFreq = 1.0/256.0;
    voroPeriod = vec3(8);
    voroWarping = vec2(FBMPeriodic(gl_FragCoord.xy / 2048.0) * 4 - 40,FBMPeriodic(gl_FragCoord.xy / 2048.0) * 4 - 40) * 128.0;
    height = max(voronoiNoise(vec3(gl_FragCoord.xy + voroWarping,brseedAbs + 2) * voroFreq,voroPeriod).z * 0.4 + 0.14,height);
    voroFreq = 1.0/512.0;
    voroPeriod = vec3(4);
    voroWarping = vec2(FBMPeriodic(gl_FragCoord.xy / 2048.0) * 2 + 60,FBMPeriodic(gl_FragCoord.xy / 2048.0) * 2 + 60) * 64.0;
    height = max(voronoiNoise(vec3(gl_FragCoord.xy + voroWarping,brseedAbs + 3) * voroFreq,voroPeriod).z * 0.8 - 0.10,height);
    voroFreq = 1.0/256.0;
    voroPeriod = vec3(8);
    voroWarping = vec2(FBMPeriodic(gl_FragCoord.xy / 2048.0) * 4 - 80,FBMPeriodic(gl_FragCoord.xy / 2048.0) * 4 - 80) * 32.0;
    height = max(voronoiNoise(vec3(gl_FragCoord.xy + voroWarping,brseedAbs + 4) * voroFreq,voroPeriod).z * 0.4 + 0.14,height);
    
    
    float smoothnessValue = 0;
    float colorModulator = 0;
    
    colorModulator += pow(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 , 1000.0) * 600,5) - 0.3;
    
    colorModulator += 1.0 - abs(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 , 10.0) * 12);
    
    smoothnessValue += pow(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 , 1000.0) * 600,5) - 0.3;

    colorModulator = clamp(colorModulator * 0.5,-0.05,1.0);
    
    albedoHeight.rgb = modulatedColor * colorModulator + baseColor;
    
    //height = sqrt(abs(FBMPeriodic(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs + 4)))) + 0.5;
    
    albedoHeight.a = height * 0.8;
    //albedoHeight.a -= 0.5;
    
    albedoHeight.a += 2.10;
    
    //albedoHeight.r = gl_FragCoord.x / 2048.0;
    //albedoHeight.g = gl_FragCoord.y / 2048.0;
    
    smoothnessValue = clamp(smoothnessValue,0.0,0.8);

    roughnessAO.r = clamp(0.999 - smoothnessValue,0,1);
    
    //roughnessAO.r = 1.0;
    roughnessAO.g = 0.0; //computed in a second pass
}

