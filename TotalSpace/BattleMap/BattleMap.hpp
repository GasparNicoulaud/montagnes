//
//  BattleMap.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#ifndef BattleMap_hpp
#define BattleMap_hpp

#define SQUARE_GRID_RES 128 //defauklt 128, better 256, but maybe optimal is 10th of FBO_RES ? //how many vertices on the side of a terrain batch
#define BATCH_MEM_SIZE 8000 //how many batches can we have at the same time maximum, should probably end up being dynamic
#define FBO_RES 512 //512 look good but slowish on the dynamicmap.frag step //resolution of the batch texture
#define BASE_HM_RES 2048 // default 2048

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct SquareBatch{
    SquareBatch * subBatch;
    glm::vec2 offset;
    float scale;
    GLuint texture; //old textures
    GLuint normalMap; //old textures
    
    GLuint albedoRoughness; //new textures
    GLuint heightHorizontalDisplacementAO; //new textures
    GLuint normals; //new textures
    
    bool isDivided;
    int posInDisplayList;
};

struct MapSettings{ //settings read from the map recipe file
    float baseNoiseFactor;
};

struct Buildings{
    //opengl stuff
    GLuint vao;
    GLuint program;
    GLint zOffsetLoc, MVPLoc;
};

enum MainShape{
    CANYONS,
    CLAWNOISE,
    RIDGENETWORK,
    SQRTPLAINS,
    BOULDERS,
    EXPDISTNOISE,
    SMALLPEAKS,
    PUREPERLIN,
    CLIFFSCALES,
    COMBINEDVORONOIS,
    QUILEZ
};

enum MaterialShaders{
    SAND,
    JAGGEDROCKS,
    CLIFFFACE,
    VORONOIROCKFIELD,
    SMOOTHSTONE,
    WHITESNOW,
    DIRTANDPEBBLES,
    POKEDVORONOI,
    ROCKSANDCRATERS,
    FINEGRAVEL,
    ELONGATEDBLOCKS,
    ROUGHROCK
};

struct RidgeMapRecipe{ //map generation recipe for ridge map
    float exclusionZoneRadius;
    float scale;
    int seedCount;
    float seedStartingHeight;
};

struct FreqAmpParameters{
    float frequency;
    float amplitude;
};

struct MaterialRecipe{
    int seed;
    MaterialShaders materialShader;
};

struct Recipe{ //map generation recipe
    int seed;
    float baseNoiseFactor;
    float erosionFactor;
    
    //this shouldnt be here but it is simpler that way...
    //height at the middle of the map for the solo building
    float heightAtMiddle;
    
    MainShape mainShape;
    
    RidgeMapRecipe ridgeMapRecipe;
    
    FreqAmpParameters clawNoiseParameters;
    FreqAmpParameters sqrtPlainsParameters;
    FreqAmpParameters canyonsParameters;
    FreqAmpParameters bouldersParameters;
    FreqAmpParameters smallPeaksParameters;
    FreqAmpParameters purePerlinParameters;
    FreqAmpParameters cliffScalesParameters;
    FreqAmpParameters combinedVoronoisParameters;
    FreqAmpParameters quilezParameters;
    
    float perlinDisplaceStrenth;
    float firstperlinDisplaceStrenth;
    
    FreqAmpParameters oldCratersParameters; //param for crater layer before erosion and blurmaxing
    FreqAmpParameters newCratersParameters; //param for crater layer after erosion and blurmaxing
    
    float flowmapCarvingStrength;
    
    float pyramidizeFactor;
    
    bool doClayJohnFilter;
    
    int boeschErosionIterations;
    
    float flipAfterMainShape;
    float flipAfterEverything;
    
    float firstBlurMaxStep;
    float finalBlurMaxStep;
    float midBlurMaxTerraceFreq;
    
    float PreWMTerraceFreq;
    int WMSpecialIterations;
    float WMSpecialSedimentation;
    float PostWMSpecialTalusAngle;
    
    bool applyTerracing;
    float teraccingAmount;
    
    int jakoV2Steps;
    float jako_rain_rate;
    float JakoTerraceFreq;
    
    bool doWMerosion;
    bool doGeoglicalEnhancement;
    double WMsedimentation;
    bool doWMMicroErode;
    float WMTerraceFreq;
    int WMThermalIterations;
    float postWMBlurMaxOffset;
    
    MaterialRecipe stoneRecipe;
    MaterialRecipe rocksRecipe;
    MaterialRecipe sandRecipe;
    MaterialRecipe flowRecipe;
    
    float sunPosX;
    float sunPosY;
    float sunPosZ;
    
    float horizontalDisplacementCoeff;
    
    float finalTalusCarveSlope;
};

struct StaticMapTextures{
    GLuint lightMap; //r for ao, g for drop shadow
    GLuint geometryMap; //r for height, g for concavity, b & a for normals
    GLuint materialMap; //r for stone, g for rocks, b for sand
    GLuint stoneMaterialAlbedoHeight; //rgb albedo, a height
    GLuint stoneMaterialRouhnessAO; //r for roughness, g for ambient oclusion
    GLuint rocksMaterialAlbedoHeight; //rgb albedo, a height
    GLuint rocksMaterialRouhnessAO; //r for roughness, g for ambient oclusion
    GLuint sandMaterialAlbedoHeight; //rgb albedo, a height
    GLuint sandMaterialRouhnessAO; //r for roughness, g for ambient oclusion
    GLuint flowMaterialAlbedoHeight; //rgb albedo, a height
    GLuint flowMaterialRouhnessAO; //r for roughness, g for ambient oclusion
};

struct DynamicMapShaderLocations{
    GLint materialMapLoc;
    GLint lightMapLoc;
    GLint geometryMapLoc;
    GLint stoneMaterialAlbedoHeightLoc;
    GLint stoneMaterialRouhnessAOLoc;
    GLint rocksMaterialAlbedoHeightLoc;
    GLint rocksMaterialRouhnessAOLoc;
    GLint sandMaterialAlbedoHeightLoc;
    GLint sandMaterialRouhnessAOLoc;
    GLint flowMaterialAlbedoHeightLoc;
    GLint flowMaterialRouhnessAOLoc;
};

struct FinalRenderShaderLocations{
    GLint heightHorizontalDisplacementAOLoc;
    GLint albedoRoughnessLoc;
    GLint normalMapLoc;
    GLint lightMapLoc;
};

struct BattleMap {
    
    float time = 0; //time in sec of life of the map
    
    float lodSwitch = 6000; //lod switches when closer than this distance, currently unused
    float minRad = 50; //minimum radius of a batch
    
    float radius; //radius of the smallest circle you can inscribe the map into, in meters, also the size of a side of the 6 main triangles
    SquareBatch mainBatch;
    
    int gridsToDisplayCount = 0;
    const int bmc = BATCH_MEM_SIZE;
    
    SquareBatch * listOfSquareBatchesToDisplay[BATCH_MEM_SIZE]; //will need to be dynamic maybe ?
    bool activeBatch[BATCH_MEM_SIZE];
    

    const int gridres = SQUARE_GRID_RES;
    
    
    //Shader Programs and their Uniform Locations
    GLuint terrain_program; //final render
    FinalRenderShaderLocations finalRenderShaderLocations;
    GLint modprojLoc,
    camposLocFinalRender,
    sunlightDirectionLocFinalRender,
    offsetLocFinalRender,
    scaleLocFinalRender,
    localHeightmapLoc,
    localNormalmapLoc,
    globalLightMapLoc;
    
    GLuint height_program; //dynamic map shader
    DynamicMapShaderLocations dynamicMapShaderLocations;
    GLint offsetLocDynamicMap,
    scaleLocDynamicMap,
    flowmapCarvingStrengthLoc;
    
    GLuint normalMap_program;
    GLint scaleLocNormalMap;
    
    GLuint skybox_program;
    GLint inversemodprojLoc,
    camposLocSkybox,
    sunlightDirectionLocSkybox;
    
    //base height map texture handle
    GLuint bmhTex;
    StaticMapTextures staticMapTextures;
    
    //vao and vbo stuff
    GLuint skyboxVao;
    GLuint vboID;
    GLuint vaoID;
    GLuint iboID;
    GLuint vboIDHD;
    GLuint vaoIDHD;
    GLuint fboID;
    
    //fbo stuff
    float fboWidth = FBO_RES;
    float fboHeight = FBO_RES;// * sqrtf(3) * 0.5;
    
    //MapSettings settings;
    
    Buildings buildings;
    
    Recipe recipe;
    
    // deffered rendering g buffer
    GLuint gBufferFbo;
    GLuint gBufferPosition;
    GLuint gBufferAlbedoRoughness;
    GLuint gBufferNormal;
    GLuint gBufferDepthBuffer;
    
    // deferred rendering shader
    GLuint fillGbuffer;
    GLuint fillGbuffermodprojLoc;
    GLuint fillGbuffercamposLoc;
    GLuint fillGbufferoffsetLoc;
    GLuint fillGbufferscaleLoc;
    GLuint fillGbufferlocalHeightmapLoc;
    GLuint fillGbufferlocalNormalmapLoc;
    FinalRenderShaderLocations fillGbufferShaderLocations;
    GLuint drawGbufferProgram;
    GLuint drawGbufferPositionLoc;
    GLuint drawGbufferAlbedoRoughnesLoc;
    GLuint drawGbufferNormalSamplerLoc;
    GLuint drawGbufferGlobalLightMapLoc;
    GLuint drawGbufferCampos;
    GLuint drawGbufferSunlightDirection;
    
    GLuint gBufferSkybox_program;
    GLuint gBufferSkybox_programInversemodprojLoc;
    GLuint gBufferSkybox_programCamposLoc;
    
    // screenshot frame buffer object
    GLuint screenshotFbo;
    GLuint screenshotTexture;
    GLuint screenshotDepthBuffer;
    
    typedef uint8_t BYTE;
    BYTE* screenshotPixels;
    
    // video frame buffer object
    GLuint videoFbo;
    GLuint videoTexture;
    GLuint videoDepthBuffer;
    
    BYTE* videoPixels;
};

#endif /* BattleMap_hpp */
