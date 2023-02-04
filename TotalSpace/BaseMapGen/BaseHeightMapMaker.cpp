//
//  BaseHeightMapMaker.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 18/04/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "BaseHeightMapMaker.hpp"
#include "PostErosionFlowMap.hpp"
#include "ShaderLoader.hpp"
#include "JakoErosion.hpp"
#include "GaspErosion.hpp"
#include "RidgeMap.hpp"
#include "BlurMaxSediment.hpp"
#include "HardnessMapGen.hpp"
#include "BoeschThermalErosion.hpp"
#include "GenRidgeMapVertices.hpp"
#include "RidgeVertexToBHM.hpp"
#include "PerlinDisplace.hpp"
#include "NormalAndConvexityMap.hpp"
#include "ClayJohnFilter.hpp"
#include "Craters.hpp"
#include "ComputeAOintoLightMap.hpp"
#include "ComputeGometryMap.hpp"
#include "ComputeMaterialMap.hpp"
#include "CarveFlowMap.hpp"
#include "GenMaterialTextures.hpp"
#include "TerraceFilter.hpp"
#include "GenShadowHeightMap.hpp"
#include "CPUParticleErode.hpp"
#include "GaspErodeV2.hpp"
#include "TalusCarve.hpp"
#include "WMErodeSpecial.hpp"
#include "Sedimentation.hpp"
#include <FreeImagePlus.h>


using namespace glm;

void genBHMGPU(int bmhRes, Recipe recipe, BattleMap* battleMap)
{
    printf("Begin BMH generation\n");
    glClearColor(0,0,0,1);
    
    //GLuint shaderOne = LoadAndLinkShader("passThru.vert", "bhmOne.frag");
    GLuint clawMarkSahder = LoadAndLinkShader("passThru.vert", "BaseMapGen/clawMarkNoise.frag");
    GLuint vboID;
    GLuint vaoID;
    GLuint fboID;
    GLuint bhmTex;
    GLuint normalConvexityMap;
    
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    //remplissage du vbo
    float quad[6*3] = {
        -1, 1,0,
        -1,-1,0,
         1, 1,0,
        
        -1,-1,0,
         1, 1,0,
         1,-1,0,};
    glBufferData(GL_ARRAY_BUFFER, 6*3 * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 6*3 * sizeof(float), quad);
    //vbo done
    //now we do the vao
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //vao done
    
    glViewport(0, 0, bmhRes, bmhRes);
    
    // bhm text gen
    glGenTextures(1, &bhmTex);
    glBindTexture(GL_TEXTURE_2D, bhmTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER | GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bmhRes, bmhRes, 0, GL_RGB, GL_FLOAT, NULL);// R for height, G for sediment, B unused for now
    
    // bhm normal and convexity map
    glGenTextures(1, &normalConvexityMap);
    glBindTexture(GL_TEXTURE_2D, normalConvexityMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bmhRes, bmhRes, 0, GL_RGB, GL_FLOAT, NULL);
    
    // bhm lightmap gen
    glGenTextures(1, &battleMap->staticMapTextures.lightMap);
    glBindTexture(GL_TEXTURE_2D, battleMap->staticMapTextures.lightMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, bmhRes, bmhRes, 0, GL_RG, GL_HALF_FLOAT, NULL);
    
    //if(glIsFramebuffer(fboID) == GL_TRUE) //error checking, check that fboID isnt already a framebuffer
    //    glDeleteFramebuffers(1, &fboID);
    
    // fbo gen
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    
    //bhm text attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhmTex, 0/*mipmap level*/);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //error check
    {
        glDeleteFramebuffers(1, &fboID);
        glDeleteTextures(1, &bhmTex);
        printf("!!!!!! bad FBO\n");
    }
    
    // BHM ACTUAL GENERATION STRART HERE ________________________________________
    
    //first layer of the map
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(recipe.mainShape == QUILEZ)
    {
        printf("quilezParameters.frequency = %f\n",recipe.quilezParameters.frequency);
        printf("quilezParameters.amplitude = %f\n",recipe.quilezParameters.amplitude);
        
        GLuint quilezProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/quilez.frag");
        glUseProgram(quilezProgram);
        glUniform1i(glGetUniformLocation(quilezProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(quilezProgram, "globalFrequency"),recipe.quilezParameters.frequency);
        glUniform1f(glGetUniformLocation(quilezProgram, "globalAmplification"),recipe.quilezParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : Quilez \n");
    }
    else if(recipe.mainShape == COMBINEDVORONOIS)
    {
        printf("combinedVoronoisParameters.frequency = %f\n",recipe.combinedVoronoisParameters.frequency);
        printf("combinedVoronoisParameters.amplitude = %f\n",recipe.combinedVoronoisParameters.amplitude);
        
        GLuint combinedVoronoisProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/combinedVoronois.frag");
        glUseProgram(combinedVoronoisProgram);
        glUniform1i(glGetUniformLocation(combinedVoronoisProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(combinedVoronoisProgram, "globalFrequency"),recipe.combinedVoronoisParameters.frequency);
        glUniform1f(glGetUniformLocation(combinedVoronoisProgram, "globalAmplification"),recipe.combinedVoronoisParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : combined voronois\n");
    }
    else if(recipe.mainShape == CLIFFSCALES)
    {
        printf("cliffScalesParameters.frequency = %f\n",recipe.cliffScalesParameters.frequency);
        printf("cliffScalesParameters.amplitude = %f\n",recipe.cliffScalesParameters.amplitude);
        
        GLuint cliffScalesProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/cliffScales.frag");
        glUseProgram(cliffScalesProgram);
        glUniform1i(glGetUniformLocation(cliffScalesProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(cliffScalesProgram, "globalFrequency"),recipe.cliffScalesParameters.frequency);
        glUniform1f(glGetUniformLocation(cliffScalesProgram, "globalAmplification"),recipe.cliffScalesParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : cliff scales\n");
    }
    else if(recipe.mainShape == PUREPERLIN)
    {
        printf("purePerlinParameters.frequency = %f\n",recipe.purePerlinParameters.frequency);
        printf("purePerlinParameters.amplitude = %f\n",recipe.purePerlinParameters.amplitude);
        
        GLuint purePerlinProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/purePerlin.frag");
        glUseProgram(purePerlinProgram);
        glUniform1i(glGetUniformLocation(purePerlinProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(purePerlinProgram, "globalFrequency"),recipe.purePerlinParameters.frequency);
        glUniform1f(glGetUniformLocation(purePerlinProgram, "globalAmplification"),recipe.purePerlinParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : Pure perlin\n");
    }
    else if(recipe.mainShape == RIDGENETWORK)
    {
        double currentTime = glfwGetTime();
        
        GLuint sqrtPlainsProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/sqrtPlains.frag");
        glUseProgram(sqrtPlainsProgram);
        glUniform1i(glGetUniformLocation(sqrtPlainsProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(sqrtPlainsProgram, "globalFrequency"),0.2);
        glUniform1f(glGetUniformLocation(sqrtPlainsProgram, "globalAmplification"),0.1);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        perlinDisplace(bhmTex, bmhRes, recipe.seed + 123, 20.0 * recipe.perlinDisplaceStrenth, 0.05, 0.0);
        
        #define RIDGE_VERTEX_COUNT 1000
        #define RIDGE_VERTEX_ARRAY_SIZE (RIDGE_VERTEX_COUNT * 18)
        float ridgeVertices[RIDGE_VERTEX_ARRAY_SIZE];
        
        printf("ridgeMapRecipe.scale = %f\n",recipe.ridgeMapRecipe.scale);
        printf("ridgeMapRecipe.seedCount = %i\n",recipe.ridgeMapRecipe.seedCount);
        printf("ridgeMapRecipe.seedStartingHeight = %f\n",recipe.ridgeMapRecipe.seedStartingHeight);
        printf("ridgeMapRecipe.exclusionZoneRadius = %f\n",recipe.ridgeMapRecipe.exclusionZoneRadius);
        
         
        genRigdeMapVertices(ridgeVertices, RIDGE_VERTEX_COUNT, 10000000,
                            recipe.ridgeMapRecipe.seedCount,
                            recipe.seed,
                            recipe.ridgeMapRecipe.scale,
                            recipe.ridgeMapRecipe.exclusionZoneRadius,
                            recipe.ridgeMapRecipe.seedStartingHeight);
        
        printf("time spent on ridgemap generation : %.4fs\n",glfwGetTime() - currentTime);
            
        //this function uses a vbo and vao inside it, so after it we must rebind the correct vao containing a normal quad
        ridgeVerticesIntoBHM(ridgeVertices, RIDGE_VERTEX_COUNT, bhmTex, bmhRes);
        glBindVertexArray(vaoID);
        
        printf("main shape : ridget network\n");
        
        //last param is slope factor,higher means sharper slopes, probably best in [0.001 - 0.005] range, a high value like 0.005 basically deactivates it
        pyramidize(bhmTex,bmhRes,recipe.pyramidizeFactor); //seems to help for some clawnoises but hurts others
        printf("pyramidize Factor = %f, 0.001 flattens, 0.005 does nothing\n",recipe.pyramidizeFactor);
    }
    else if(recipe.mainShape == SQRTPLAINS)
    {
        printf("sqrtPlainsParameters.frequency = %f\n",recipe.sqrtPlainsParameters.frequency);
        printf("sqrtPlainsParameters.amplitude = %f\n",recipe.sqrtPlainsParameters.amplitude);
        
        GLuint sqrtPlainsProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/sqrtPlains.frag");
        glUseProgram(sqrtPlainsProgram);
        glUniform1i(glGetUniformLocation(sqrtPlainsProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(sqrtPlainsProgram, "globalFrequency"),recipe.sqrtPlainsParameters.frequency);
        glUniform1f(glGetUniformLocation(sqrtPlainsProgram, "globalAmplification"),recipe.sqrtPlainsParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        perlinDisplace(bhmTex, bmhRes, recipe.seed + 123, 20.0 * recipe.perlinDisplaceStrenth, 0.05, 0.0);
        printf("main shape : square root plains\n");
    }
    else if(recipe.mainShape == CANYONS)
    {
        //recipe.canyonsParameters.amplitude = 1.0;
        
        printf("canyonsParameters.frequency = %f\n",recipe.canyonsParameters.frequency);
        printf("canyonsParameters.amplitude = %f\n",recipe.canyonsParameters.amplitude);
        
        GLuint canyonsProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/canyons.frag");
        glUseProgram(canyonsProgram);
        glUniform1i(glGetUniformLocation(canyonsProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(canyonsProgram, "globalFrequency"),recipe.canyonsParameters.frequency);
        glUniform1f(glGetUniformLocation(canyonsProgram, "globalAmplification"),recipe.canyonsParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        //perlinDisplace(bhmTex, bmhRes, recipe.seed + 123, 20.0 * recipe.perlinDisplaceStrenth, 0.05, 0.0);
        printf("main shape : canyons\n");
    }
    else if(recipe.mainShape == CLAWNOISE)
    {
        printf("clawNoiseParameters.frequency = %f\n",recipe.clawNoiseParameters.frequency);
        printf("clawNoiseParameters.amplitude = %f\n",recipe.clawNoiseParameters.amplitude);

        glUseProgram(clawMarkSahder);
        glUniform1i(glGetUniformLocation(clawMarkSahder, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(clawMarkSahder, "globalFrequency"),recipe.clawNoiseParameters.frequency);
        glUniform1f(glGetUniformLocation(clawMarkSahder, "globalAmplification"),recipe.clawNoiseParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : claw marks\n");
    }
    else if(recipe.mainShape == BOULDERS)
    {
        printf("bouldersParameters.frequency = %f\n",recipe.bouldersParameters.frequency);
        printf("bouldersParameters.amplitude = %f\n",recipe.bouldersParameters.amplitude);
        
        GLuint bouldersProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/boulders.frag");
        glUseProgram(bouldersProgram);
        glUniform1i(glGetUniformLocation(bouldersProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(bouldersProgram, "globalFrequency"),recipe.bouldersParameters.frequency);
        glUniform1f(glGetUniformLocation(bouldersProgram, "globalAmplification"),recipe.bouldersParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : boulders\n");
    }
    else if(recipe.mainShape == EXPDISTNOISE)
    {
        //printf("bouldersParameters.frequency = %f\n",recipe.bouldersParameters.frequency);
        //printf("bouldersParameters.amplitude = %f\n",recipe.bouldersParameters.amplitude);
        
        GLuint expdistProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/expdist.frag");
        glUseProgram(expdistProgram);
        glUniform1i(glGetUniformLocation(expdistProgram, "seed"),recipe.seed);
        //glUniform1f(glGetUniformLocation(bouldersProgram, "globalFrequency"),recipe.bouldersParameters.frequency);
        //glUniform1f(glGetUniformLocation(bouldersProgram, "globalAmplification"),recipe.bouldersParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : Exponentially Distributed Noise (non-random amp and freq)\n");
    }
    else if(recipe.mainShape == SMALLPEAKS)
    {
        printf("smallPeaksParameters.frequency = %f\n",recipe.smallPeaksParameters.frequency);
        printf("smallPeaksParameters.amplitude = %f\n",recipe.smallPeaksParameters.amplitude);
        
        GLuint smallPeaksProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/MainShape/smallPeaks.frag");
        glUseProgram(smallPeaksProgram);
        glUniform1i(glGetUniformLocation(smallPeaksProgram, "seed"),recipe.seed);
        glUniform1f(glGetUniformLocation(smallPeaksProgram, "globalFrequency"),recipe.smallPeaksParameters.frequency);
        glUniform1f(glGetUniformLocation(smallPeaksProgram, "globalAmplification"),recipe.smallPeaksParameters.amplitude);
        glDrawArrays(GL_TRIANGLES, 0, 6); //draw the 2 triangles so 6 vertices
        
        printf("main shape : Small Peaks (min of several FBMs)\n");
    }
    else
    {
        printf("NO MAIN SHAPE ???\nrecipe.mainShape BAD VALUE ???\n");
        assert(1==0);
    }
    
    bool blurmaxingAndErosionAndClayJohn = true;
    
    //deactivate most filters
//    blurmaxingAndErosionAndClayJohn = false;
//    recipe.applyTerracing = false;
//    recipe.flowmapCarvingStrength = 0.0;
//    recipe.firstperlinDisplaceStrenth = 0.0;
//    recipe.jakoV2Steps = 0;
//    recipe.doWMerosion = false;
//    recipe.doGeoglicalEnhancement = false;
//    recipe.doWMMicroErode = false;
//    recipe.finalTalusCarveSlope = 1.0;
//    recipe.PreWMTerraceFreq = 0;
//    recipe.WMSpecialIterations = 0;
//    recipe.PostWMSpecialTalusAngle = 1.0;
//    recipe.WMSpecialSedimentation = 0;
//    recipe.WMTerraceFreq = 0;
//    recipe.WMsedimentation = 0;
//    recipe.WMThermalIterations = 0;
    
    // Note for someday : using texelFetchOffset skips any filtering,
    // hence GL_CLAMP_TO_EDGE is ignored and when sampling over borders we get zeroes.
    // This cause the border walls or cliffs. Probably should fix at some point...
    
    //recipe.applyTerracing = true;
    
    printf("[0 - 1.0] firstperlinDisplaceStrenth = %f\n",recipe.firstperlinDisplaceStrenth);
    perlinDisplace(bhmTex, bmhRes, recipe.seed + 1000, 100 * recipe.firstperlinDisplaceStrenth, 0.02, 0);
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    if(recipe.applyTerracing)
    {
        printf("Terracing with amount = %f\n",recipe.teraccingAmount);
        //last param is slope factor,higher means sharper slopes, probably best in [0.001 - 0.005] range, a high value like 0.005 basically deactivates it
        pyramidize(bhmTex,bmhRes,0.015); //seems to help for some clawnoises but hurts others
        TerraceFilter(bhmTex,recipe.seed,recipe.teraccingAmount);
        pyramidize(bhmTex,bmhRes,0.020); //seems to help for some clawnoises but hurts others
        BlurMaxSediment(bhmTex,bmhRes, 10, 0);
//        GLuint hardnessMap = genHardnessMap(bhmTex, bmhRes, recipe.seed, true); //last param is for terraced hardeness map
//        boeschErode(bhmTex, bmhRes, hardnessMap, 50);
    }
    else
    {
        printf("no terracing\n");
    }
        
    if(blurmaxingAndErosionAndClayJohn)
    {
        //last param is slope factor,higher means sharper slopes, probably best in [0.001 - 0.005] range, a high value like 0.005 basically deactivates it
        //pyramidize(bhmTex,bmhRes,recipe.pyramidizeFactor); //seems to help for some clawnoises but hurts others
        //printf("pyramidize Factor = %f, 0.001 flattens, 0.005 does nothing\n",recipe.pyramidizeFactor);
        
        pyramidize(bhmTex,bmhRes,0.01);
        
        printf("perlinDisplaceStrenth = %f\n",recipe.perlinDisplaceStrenth);
        
        // old craters
        if(recipe.oldCratersParameters.amplitude > 5){
            printf("old craters with amp : %f and freq : %f\n",recipe.oldCratersParameters.amplitude, recipe.oldCratersParameters.frequency);
            addCraters(bhmTex, bmhRes, recipe.seed, recipe.oldCratersParameters.amplitude, recipe.oldCratersParameters.frequency);
        }
        
        perlinDisplace(bhmTex, bmhRes, recipe.seed + 100, 20.0 * recipe.perlinDisplaceStrenth, 0.05, recipe.flipAfterMainShape);

        perlinDisplace(bhmTex, bmhRes, recipe.seed, 1.0 * recipe.perlinDisplaceStrenth, 0.5, 0.0);

        perlinDisplace(bhmTex, bmhRes, recipe.seed + 300, 5.0 * recipe.perlinDisplaceStrenth, 1.0, 0.0);
        
        recipe.doClayJohnFilter = false; // sadly, clayjohn seems broken, maybe try to fix it someday...
//        if(recipe.doClayJohnFilter) // sadly, clayjohn seems broken, maybe try to fix it someday...
//        {
//
//            //genNormAndConvMap(bhmTex, normalConvexityMap, bmhRes);
//            genBlurredNormAndConvMap(bhmTex, normalConvexityMap, bmhRes); // sadly, clayjohn seems broken, maybe try to fix it someday...
//
//            //works okayish, but needs a blurred normalmap
//            clayJohnFilter(bhmTex, normalConvexityMap, bmhRes); // sadly, clayjohn seems broken, maybe try to fix it someday...
//            pyramidize(bhmTex,bmhRes,0.01); // sadly, clayjohn seems broken, maybe try to fix it someday...
//
//            printf("Applied clayJohn Filter and a pyramidize at 0.01\n");
//        }
//        else
//        {
//            printf("No clayJohn Filter\n");
//        }
        
        GLuint hardnessMap = genHardnessMap(bhmTex, bmhRes, recipe.seed + 10.0, true); //last param is for terraced hardeness map

        boeschErode(bhmTex, bmhRes, hardnessMap, recipe.boeschErosionIterations);
        printf("boeschErode with %i iterations\n",recipe.boeschErosionIterations);
    }

    if(blurmaxingAndErosionAndClayJohn)
    {
        // new craters buggy apparently, lets fix this later
//        if(recipe.newCratersParameters.amplitude > 1){
//            printf("new craters with amp : %f and freq : %f\n",recipe.newCratersParameters.amplitude, recipe.newCratersParameters.frequency);
//            addCraters(bhmTex, bmhRes, (recipe.seed + 1234) * 0.01, recipe.newCratersParameters.amplitude, recipe.newCratersParameters.frequency);
//        }

        perlinDisplace(bhmTex, bmhRes, recipe.seed + 400, 15.0 * recipe.perlinDisplaceStrenth, 0.05, recipe.flipAfterEverything);

        printf("heightmap flip values : %f and %f\n",recipe.flipAfterMainShape,recipe.flipAfterEverything);
        
        BlurMaxSediment(bhmTex,bmhRes,150, 0.05 * recipe.firstBlurMaxStep);
        BlurMaxSediment(bhmTex,bmhRes,100, 0.05 * recipe.firstBlurMaxStep);
        printf("Fisrt BlurMaxSteps ([0 - 5.0], high number means less sediment) final : %f\n",recipe.firstBlurMaxStep);
        
        if(recipe.midBlurMaxTerraceFreq > 10){
            talusCarve(bhmTex,bmhRes,0.8); // 0 to 1
            printf("Terracing with a frequency of %f\n",recipe.midBlurMaxTerraceFreq);
            TerraceFilter(bhmTex,recipe.seed,recipe.midBlurMaxTerraceFreq);
            talusCarve(bhmTex,bmhRes,0.95); // 0 to 1
        }
        
        BlurMaxSediment(bhmTex,bmhRes,40, 0.05 * recipe.finalBlurMaxStep);
        printf("BlurMaxSteps ([0 - 3.0], high number means less sediment) final : %f\n",recipe.finalBlurMaxStep);
        
    }
    
    if(recipe.jakoV2Steps > 0){
        printf("jako eroding with %i steps and %f rain rate\n",recipe.jakoV2Steps, recipe.jako_rain_rate);
        jakoErodeV2(bhmTex, bmhRes, recipe.jakoV2Steps, recipe.seed, recipe.jako_rain_rate);
        if(recipe.JakoTerraceFreq > 10){
            talusCarve(bhmTex,bmhRes,0.8); // 0 to 1
            printf("with terracing with a frequency of %f\n",recipe.JakoTerraceFreq);
            TerraceFilter(bhmTex,recipe.seed,recipe.JakoTerraceFreq);
            talusCarve(bhmTex,bmhRes,0.95); // 0 to 1
        }
        
        //maybe after jako eroding, sometimes, we should do bhm = average(bhm, bhmBeforeJako); to keep some of the detail remove by jako
    }
    
    if(recipe.PreWMTerraceFreq > 15){
        talusCarve(bhmTex,bmhRes,0.9); // 0 to 1
        printf("PreWM terracing with a frequency of %f\n",recipe.PreWMTerraceFreq);
        TerraceFilter(bhmTex,recipe.seed,recipe.PreWMTerraceFreq);
        talusCarve(bhmTex,bmhRes,1.50); // 0 to 1
    }

    if(recipe.WMSpecialIterations > 0){
        printf("WM Special Eroding with sedimentation : %f for %i iterations\n",recipe.WMSpecialSedimentation,recipe.WMSpecialIterations);
        bhmTex = WMErodeSpecial(bhmTex,recipe.WMSpecialSedimentation,recipe.WMSpecialIterations/2);
        printf("now thermal kinda erosion\n");
//        bhmTex = CPUParticleErodeOnePass(bhmTex, bmhRes, recipe.WMSpecialIterations/2, 0.00005, 0.5);
        bhmTex = Sedimentation(bhmTex, bmhRes, recipe.WMSpecialIterations * 50, 0.005);
        printf("Talus carving with slope : %f\n",recipe.PostWMSpecialTalusAngle);
        talusCarve(bhmTex,bmhRes,recipe.PostWMSpecialTalusAngle); // 0 to 1
    }
    
    if(recipe.doWMerosion)
    {
        printf("doing WM erosion with sedimentation : %f (0 is only removal, 2 is only sediments)\n",recipe.WMsedimentation);
        float talusAngle;
        float erosStrength;
        
        talusAngle = 0.00002;
        erosStrength = 1.0;
        
        if(recipe.doGeoglicalEnhancement){
            printf("with geological enhancment\n");
            bhmTex = CPUParticleErode(bhmTex, bmhRes, 200, talusAngle, erosStrength); //low talus angle so liquid behaving
        }
        
        if(recipe.WMTerraceFreq > 15){
            talusCarve(bhmTex,bmhRes,0.8); // 0 to 1
            printf("with terracing with a frequency of %f\n",recipe.WMTerraceFreq);
            TerraceFilter(bhmTex,recipe.seed,recipe.WMTerraceFreq);
            talusCarve(bhmTex,bmhRes,0.95); // 0 to 1
        }
        
        bhmTex = CPUParticleErode128Steps(bhmTex, bmhRes, recipe.WMsedimentation);

//        talusAngle = 0.01;
//        erosStrength = 0.2;
        printf("now thermal erosion\n");
//        bhmTex = CPUParticleErodeOnePass(bhmTex, bmhRes, recipe.WMThermalIterations, talusAngle, erosStrength);
        bhmTex = Sedimentation(bhmTex, bmhRes, recipe.WMThermalIterations, 0.004);
        
        talusCarve(bhmTex,bmhRes,0.9); // 0 to 1
    }
    
    printf("blurmaxing with offset %f\n",recipe.postWMBlurMaxOffset);
    BlurMaxSediment(bhmTex,bmhRes,5,recipe.postWMBlurMaxOffset);
    
    bhmTex = Sedimentation(bhmTex, bmhRes, 3000, 0.003);
    
    if(recipe.doWMMicroErode){
        printf("doing WM micro erosion\n");
        bhmTex = CPUParticleMicroErode(bhmTex, bmhRes);
    }
    
    talusCarve(bhmTex,bmhRes,recipe.finalTalusCarveSlope); // 0 to 1
    printf("talus carve (1 is nothing, 0 is flatten) = %f\n",recipe.finalTalusCarveSlope);
    
    addFlowMap(bhmTex,bmhRes);
    //exit(0);
    
//    bhmTex = WMErodeSpecial(bhmTex);
//    talusCarve(bhmTex,bmhRes,0.5); // 0 to 1
//    TerraceFilter(bhmTex,recipe.seed,5);
//    talusCarve(bhmTex,bmhRes,0.55); // 0 to 1
//    bhmTex = WMErodeSpecial(bhmTex);
//    talusCarve(bhmTex,bmhRes,0.6); // 0 to 1
//    bhmTex = WMErodeSpecial(bhmTex);
//    talusCarve(bhmTex,bmhRes,0.7); // 0 to 1
//    TerraceFilter(bhmTex,recipe.seed,10);
//    talusCarve(bhmTex,bmhRes,1.0); // 0 to 1
//    bhmTex = CPUParticleErode128Steps(bhmTex, bmhRes, 0.01);
//    bhmTex = CPUParticleMicroErode(bhmTex, bmhRes);
//    talusCarve(bhmTex,bmhRes,1.2); // 0 to 1
    
    //doesnt look that good... maybe should be active even more rarely
    //printf("flow map carving strength = %f (between 0 and 1)\n",recipe.flowmapCarvingStrength);
    //carveFlowMap(bhmTex,recipe.flowmapCarvingStrength);
    
    //heightmap export special :
//    float* heightmapPixels = new float[2048*2048*4];
//    float minval = 100.0;
//    float maxval = -100.0;
//    glReadPixels(0, 0, 2048, 2048, GL_RGBA, GL_FLOAT, heightmapPixels);
//    for(int x=0;x<2048;x++){ // collect min and max val
//        for(int y=0;y<2048;y++){
//            float value = heightmapPixels[((x*2048) + y) * 4];
//            if(value < minval){
//                minval = value;
//            }
//            if(value > maxval){
//                maxval = value;
//            }
//        }
//    }
//    for(int x=0;x<2048;x++){ // renomr to [0,1]
//        for(int y=0;y<2048;y++){
//            heightmapPixels[((x*2048) + y) * 4] = (heightmapPixels[((x*2048) + y) * 4] - minval)/(maxval - minval);
//        }
//    }
////    for(int x=0;x<2048;x++){ //log
////        for(int y=0;y<2048;y++){
////            if(x%128 == 0 && y%128 == 0){
////                float value = heightmapPixels[((x*2048) + y) * 4];
////                printf("%.4f ",value);
////            }
////            if(x%128 == 0 && y == 2047){
////                printf("\n");
////            }
////        }
////    }
//    BYTE* heightmapBytes = new BYTE[2048*2048*3];
//    for(int x=0;x<2048;x++){
//        for(int y=0;y<2048;y++){
//            float Hvalue = heightmapPixels[((x*2048) + y) * 4];
//            heightmapBytes[((x*2048) + y) * 3 + 0] = BYTE(Hvalue * 256);
//            heightmapBytes[((x*2048) + y) * 3 + 1] = BYTE(((Hvalue * 256) - heightmapBytes[((x*2048) + y) * 3 + 0]) * 256);
//            heightmapBytes[((x*2048) + y) * 3 + 2] = 0;
//        }
//    }
//    for(int x=0;x<2048;x++){ //log
//        for(int y=0;y<2048;y++){
//            if(x%128 == 0 && y%128 == 0){
//                BYTE value = heightmapBytes[((x*2048) + y) * 3];
//                printf("%i ",value);
//            }
//            if(x%128 == 0 && y == 2047){
//                printf("\n");
//            }
//        }
//    }
//    
//    err = glGetError();
//    if(err != 0)
//    printf("Error = %i\n %i %i %i \n", err, GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_OPERATION);
//    assert(err == 0);
//
//
//    FIBITMAP* image = FreeImage_ConvertFromRawBits(
//                        heightmapBytes
//                        , (int)(2048)
//                        , (int)(2048)
//                        , (int)(3 * 2048)
//                        , (unsigned)24
//                        , (unsigned)0x0000FF, (unsigned)0xFF0000, (unsigned)0x00FF00, false
//                       );
//
//    const char heightmapPath[] = "/Users/gaspar/Desktop/heightmap.png";
//    FreeImage_Save(FIF_PNG, image, heightmapPath, 0);
//    // Free resources
//    FreeImage_Unload(image);

    

    battleMap->bmhTex = bhmTex;
    
    //convert bhm toStaticMapTextures
    
    GLuint shadowHeightMap = GenShadowHeightMap(bhmTex, bmhRes, recipe.sunPosX, recipe.sunPosY, recipe.sunPosZ);
    
    computeAOintoLightMap(bhmTex, battleMap->staticMapTextures.lightMap); //only does ao
    computeShadowHeightMapIntoLightMap(bhmTex, shadowHeightMap, battleMap->staticMapTextures.lightMap); //does drop shadows
    computeGeometryMap(bhmTex, &battleMap->staticMapTextures.geometryMap); //only does height for now, had planned ot put convexity and normals but maybe not needed
    computeMaterialMap(bhmTex, &battleMap->staticMapTextures.materialMap); // computes the materialMap wich indicates where each material goes

    /*
    recipe.stoneRecipe.materialShader = ROCKSANDCRATERS;
    recipe.rocksRecipe.materialShader = ELONGATEDBLOCKS;
    recipe.sandRecipe.materialShader = VORONOIROCKFIELD;
    recipe.flowRecipe.materialShader = VORONOIROCKFIELD;
    */
    
    //Materials generation
    
    printf("stone material : ");
    genMaterialTexture(&battleMap->staticMapTextures.stoneMaterialAlbedoHeight,
                       &battleMap->staticMapTextures.stoneMaterialRouhnessAO,
                       recipe.stoneRecipe,
                       bmhRes); //resolution needs to be the same as bhm because we use the same framebuffer
    
    printf("rock material : ");
    genMaterialTexture(&battleMap->staticMapTextures.rocksMaterialAlbedoHeight,
                       &battleMap->staticMapTextures.rocksMaterialRouhnessAO,
                       recipe.rocksRecipe,
                       bmhRes); //resolution needs to be the same as bhm because we use the same framebuffer
    
    printf("sand material : ");
    genMaterialTexture(&battleMap->staticMapTextures.sandMaterialAlbedoHeight,
                       &battleMap->staticMapTextures.sandMaterialRouhnessAO,
                       recipe.sandRecipe,
                       bmhRes); //resolution needs to be the same as bhm because we use the same framebuffer
    
    printf("flow material : ");
    genMaterialTexture(&battleMap->staticMapTextures.flowMaterialAlbedoHeight,
                       &battleMap->staticMapTextures.flowMaterialRouhnessAO,
                       recipe.flowRecipe,
                       bmhRes); //resolution needs to be the same as bhm because we use the same framebuffer
    
    
    // we disable hardware linear filtering because it is not good enough when interpolating in in the dynamicmap generation
    // see https://www.iquilezles.org/www/articles/hwinterpolation/hwinterpolation.htm
    // that means we have to do our own software inerpolation in the dynamicmap.frag shader
    glBindTexture(GL_TEXTURE_2D, bhmTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST | GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}




