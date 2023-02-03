#version 410 core

layout(location = 0) out vec4 albedoHeight;
layout(location = 1) out vec4 roughnessAO;

uniform int seed;

uniform vec3 baseColor;
uniform vec3 modulatedColor;


// Modulo 289, optimizes to code without divisions
float mod289f(float x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// Permutation polynomial (ring size 289 = 17*17)
float permutef(float x) {
    return mod289f(((x*34.0)+1.0)*x);
}

// Hashed 2-D gradients with an extra rotation.
// (The constant 0.0243902439 is 1/41)
vec2 rgrad2(vec2 p, float rot) {
    // For more isotropic gradients, sin/cos can be used instead.
    float u = permutef(permutef(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
    u = fract(u) * 6.28318530718; // 2*pi
    return vec2(cos(u), sin(u));
}

//
// 2-D tiling simplex noise with rotating gradients,
// but without the analytical derivative.
//
float psrnoise(vec2 pos, vec2 per, float rot) {
    // Offset y slightly to hide some rare artifacts
    pos.y += 0.001;
    // Skew to hexagonal grid
    vec2 uv = vec2(pos.x + pos.y*0.5, pos.y);
    
    vec2 i0 = floor(uv);
    vec2 f0 = fract(uv);
    // Traversal order
    vec2 i1 = (f0.x > f0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    
    // Unskewed grid points in (x,y) space
    vec2 p0 = vec2(i0.x - i0.y * 0.5, i0.y);
    vec2 p1 = vec2(p0.x + i1.x - i1.y * 0.5, p0.y + i1.y);
    vec2 p2 = vec2(p0.x + 0.5, p0.y + 1.0);
    
    // Integer grid point indices in (u,v) space
    i1 = i0 + i1;
    vec2 i2 = i0 + vec2(1.0, 1.0);
    
    // Vectors in unskewed (x,y) coordinates from
    // each of the simplex corners to the evaluation point
    vec2 d0 = pos - p0;
    vec2 d1 = pos - p1;
    vec2 d2 = pos - p2;
    
    // Wrap i0, i1 and i2 to the desired period before gradient hashing:
    // wrap points in (x,y), map to (u,v)
    vec3 xw = mod(vec3(p0.x, p1.x, p2.x), per.x);
    vec3 yw = mod(vec3(p0.y, p1.y, p2.y), per.y);
    vec3 iuw = xw + 0.5 * yw;
    vec3 ivw = yw;
    
    // Create gradients from indices
    vec2 g0 = rgrad2(vec2(iuw.x, ivw.x), rot);
    vec2 g1 = rgrad2(vec2(iuw.y, ivw.y), rot);
    vec2 g2 = rgrad2(vec2(iuw.z, ivw.z), rot);
    
    // Gradients dot vectors to corresponding corners
    // (The derivatives of this are simply the gradients)
    vec3 w = vec3(dot(g0, d0), dot(g1, d1), dot(g2, d2));
    
    // Radial weights from corners
    // 0.8 is the square of 2/sqrt(5), the distance from
    // a grid point to the nearest simplex boundary
    vec3 t = 0.8 - vec3(dot(d0, d0), dot(d1, d1), dot(d2, d2));
    
    // Set influence of each surflet to zero outside radius sqrt(0.8)
    t = max(t, 0.0);
    
    // Fourth power of t
    vec3 t2 = t * t;
    vec3 t4 = t2 * t2;
    
    // Final noise value is:
    // sum of ((radial weights) times (gradient dot vector from corner))
    float n = dot(t4, w);
    
    // Rescale to cover the range [-1,1] reasonably well
    return 11.0*n;
}


//
// 2-D tiling simplex noise with fixed gradients,
// without the analytical derivative.
// This function is implemented as a wrapper to "psrnoise",
// at the minimal cost of three extra additions.
//
float snoise(vec2 pos) {
    return psrnoise(pos * 0.05, vec2(100,100), 0.0);
}



vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

float basicFBM(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    float retval = 0;
    for (int i = 0; i < 8; i++) {
        float noiseval = snoise(pos * pow(2,i));
        retval += noiseval / pow(2,i);
    }
    return retval;
}

float sqrtPlainsNoise(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    
    float retval = 0;
    for (int i = 0; i < 8; i++) {
        pos = rotate(pos,seed * pow(12.34,i));
        float noiseval = sqrt(sqrt(max(snoise(pos * pow(2,i)),0)));
        retval += noiseval / pow(2,i);
    }
    return retval;
}

float rand1dTo1d(float value)
{
    float random = fract(sin(value + 0.546) * 143758.5453);
    return random;
}

//dotdir default :  = vec3(12.9898, 78.233, 37.719)
float rand3dTo1d(vec3 value, vec3 dotDir)
{
    //make value smaller to avoid artefacts
    vec3 smallValue = cos(value);
    //get scalar value from 3d vector
    float random = dot(smallValue, dotDir);
    //make value more random by making it bigger and then taking the factional part
    random = fract(sin(random) * 143758.5453);
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
    minEdgeDistance = sqrt(sqrt(sqrt(minEdgeDistance))); //ADDED BY ME BUT NOT HERE ORIGINALLY
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
    
    
    float height = sqrtPlainsNoise(gl_FragCoord.xy,brseedAbs + 1);
    
    //voronoiNoise.x makes holes, .y makes plates, .z makes polygons
    
    height = 0;
    float voroFreq;
    vec3 voroPeriod;
    vec2 voroWarping;
    
    voroFreq = 1.0/512.0;
    voroPeriod = vec3(4);
    voroWarping = vec2(FBMPeriodic(gl_FragCoord.xy / 2048.0) * 2 + 20,FBMPeriodic(gl_FragCoord.xy / 2048.0) * 2 + 20) * 256.0;
    height = max(voronoiNoise(vec3(gl_FragCoord.xy + voroWarping,brseedAbs + 1) * voroFreq,voroPeriod).z * 0.8 - 0.10,height);
    voroFreq = 1.0/256.0;
    voroPeriod = vec3(8);
    voroWarping = vec2(FBMPeriodic(gl_FragCoord.xy / 2048.0) * 4 - 40,FBMPeriodic(gl_FragCoord.xy / 2048.0) * 4 - 40) * 128.0;
    height = max(voronoiNoise(vec3(gl_FragCoord.xy + voroWarping,brseedAbs + 2) * voroFreq,voroPeriod).z * 0.4 + 0.14,height);
    
    
    float perlinNoise = abs(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs + 2), 5.0)) * 0.3 + 0.1;
    perlinNoise *= abs(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs - 4), 10.0)) * 3.0;
    float highFreqPerlinNoise = FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs + 5), 1300.0) * 75;
    
    highFreqPerlinNoise = highFreqPerlinNoise * highFreqPerlinNoise * sign(highFreqPerlinNoise) + highFreqPerlinNoise;
    
    height += perlinNoise;
        
    float smoothnessValue = highFreqPerlinNoise;
    
    float colorModulator = abs(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs + 15), 50.0) * 150);
    smoothnessValue += pow(FBMPeriodicWithFreq(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs - 10), 500.0) * 300,5) - 0.3;
    
    //height = sqrt(abs(FBMPeriodic(gl_FragCoord.xy / 2048.0 + vec2(brseedAbs + 4)))) + 0.5;
    albedoHeight.rgb = baseColor * colorModulator * perlinNoise + modulatedColor * highFreqPerlinNoise;
    albedoHeight.a = height * 0.5;
    
    albedoHeight.a += 2.02;
    
    
    smoothnessValue = clamp(smoothnessValue,0.0,0.8);
    roughnessAO.r = clamp(0.999 - smoothnessValue,0,1);
    
    
    roughnessAO.g = 0.0; //computed in a second pass
}
