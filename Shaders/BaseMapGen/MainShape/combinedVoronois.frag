
#version 150 core

out vec4 out_Color;

uniform int seed;
uniform float globalFrequency;
uniform float globalAmplification;

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
    /*#if 0
     // Map from a line to a diamond such that a shift maps to a rotation.
     float u = permute(permute(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
     u = 4.0 * fract(u) - 2.0;
     // (This vector could be normalized, exactly or approximately.)
     return vec2(abs(u)-1.0, abs(abs(u+1.0)-2.0)-1.0);
     #else */
    // For more isotropic gradients, sin/cos can be used instead.
    float u = permutef(permutef(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
    u = fract(u) * 6.28318530718; // 2*pi
    return vec2(cos(u), sin(u));
    //#endif
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
    for (int i = 0; i < 16; i++) {
        float noiseval = snoise(pos * pow(2,i));
        retval += noiseval / pow(2,i);
    }
    return retval;
}

float FBMWithRotations(vec2 pos, float seed)
{
    pos *= 0.01;
    pos += vec2(cos(seed * 2),sin(seed * -3)) * 10;
    
    float lacunarity = 2.0;
    float retval = 0;
    for (int i = 0; i < 12; i++) {
        pos = rotate(pos,seed * pow(12.34,i));
        float noiseval = snoise(pos * pow(lacunarity,i));
        retval += noiseval / pow(lacunarity,i);
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
    //minEdgeDistance = sqrt(minEdgeDistance); //ADDED BY ME BUT NOT HERE ORIGINALLY
    return vec3(minDistToCell, random, minEdgeDistance);
}


void main(void)
{
    float globalFreq = globalFrequency;
    float globalAmp = globalAmplification;
    
    float brseedf = seed * 0.01;
    float brseedAbs = abs(brseedf);
    brseedAbs = mod(brseedAbs,1337.1337);
    brseedAbs *= 0.01;
    
    float warpingFreq = 1.0;
    float warpingAmp = 200;
    
    float warpingMaskFreq = 5.0;
    float warpingMaskAmp = 0.5;

    float basicperlinForWarping1 = FBMWithRotations(gl_FragCoord.xy * warpingFreq,brseedAbs - 2) * warpingAmp;
    float basicperlinForWarping2 = FBMWithRotations(gl_FragCoord.xy * warpingFreq,brseedAbs + 3) * warpingAmp;
    float warpingAmount = clamp((FBMWithRotations(gl_FragCoord.xy * warpingMaskFreq,brseedAbs + 13) + 0.5) * warpingMaskAmp,0,1);
    
    vec2 samplePoint = mix(gl_FragCoord.xy * globalFreq + vec2(basicperlinForWarping1,basicperlinForWarping2),gl_FragCoord.xy * globalFreq,warpingAmount);
    
    float basicperlin = FBMWithRotations(samplePoint * 1.0,brseedAbs) * 1.0;
    
    float voronoi1 = voronoiNoise(vec3(samplePoint + vec2(0000.0),brseedAbs + 0000)/512.0,vec3(4.0)).z * 2;
    float voronoi2 = voronoiNoise(vec3(samplePoint + vec2(1000.0),brseedAbs + 1000)/256.0,vec3(8.0)).z * 2;
    float voronoi3 = voronoiNoise(vec3(samplePoint + vec2(2000.0),brseedAbs + 2000)/512.0,vec3(4.0)).z * 2;
    float voronoi4 = voronoiNoise(vec3(samplePoint + vec2(3000.0),brseedAbs + 3000)/256.0,vec3(8.0)).z * 2;
    float voronoi5 = voronoiNoise(vec3(samplePoint + vec2(4000.0),brseedAbs + 4000)/512.0,vec3(4.0)).z * 2;
    float voronoi6 = voronoiNoise(vec3(samplePoint + vec2(5000.0),brseedAbs + 5000)/256.0,vec3(8.0)).z * 2;
    
    float outputHeight = (max(max(voronoi1 + voronoi2, voronoi3 + voronoi4),voronoi5 + voronoi6) + basicperlin) * 0.6 * globalAmp;
    
    out_Color = vec4(outputHeight,0,0,1.0);
}





