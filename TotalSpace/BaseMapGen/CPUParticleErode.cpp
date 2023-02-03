//
//  CPUParticleErode.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 27/12/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//
//  Copied from https://github.com/weigert/SimpleErosion

#define GL_SILENCE_DEPRECATION

#include "CPUParticleErode.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

glm::vec2 valueAt(float* heightMap, int i, int j, int resolution){
    if(i < 0 || i >= resolution || j < 0 || j >= resolution ){
        return glm::vec2(0);
    }
    return glm::vec2(heightMap[(i * resolution + j) * 2],heightMap[(i * resolution + j) * 2 + 1]);
}

void addSedimentAt(float* heightMap, int i, int j, int resolution, float value){
    if(i >= 0 && i < resolution && j >= 0 && j < resolution ){
        heightMap[(i * resolution + j) * 2 + 1] += value;
        heightMap[(i * resolution + j) * 2 + 1] = fmax(0,heightMap[(i * resolution + j) * 2 + 1]);
    }
}

void addValueAt(float* heightMap, int i, int j, int resolution, float value){
    if(i >= 0 && i < resolution && j >= 0 && j < resolution ){
        heightMap[(i * resolution + j) * 2] += value;
    }
}

glm::vec2 valueAtUnsafe(float* heightMap, int i, int j, int resolution){
    return glm::vec2(heightMap[(i * resolution + j) * 2],heightMap[(i * resolution + j) * 2 + 1]);
}

void addSedimentAtUnsafe(float* heightMap, int i, int j, int resolution, float value){
    heightMap[(i * resolution + j) * 2 + 1] += value;
    heightMap[(i * resolution + j) * 2 + 1] = fmax(0,heightMap[(i * resolution + j) * 2 + 1]);
}

void addValueAtUnsafe(float* heightMap, int i, int j, int resolution, float value){
    heightMap[(i * resolution + j) * 2] += value;
}

float lengthVec2(float x, float y)
{
    return sqrt(x*x + y*y);
}

float hardnessFromXY(int x, int y, float* hardnessMap){
    float hardness = hardnessMap[x*2048 + y];
    return hardness;
}

float * wordlMachineErodeAt2048optimised(float* heightMap, float* hardnessMap, double sedimentation)
{
    const float talusAngle = 0.002;
    const int batchcount = 32;
    const int resolution = 2048;
    
    for( int curbatch = 0; curbatch < batchcount; curbatch++)
    {
        const int particlePerBatch = (resolution*resolution)/100;
        for( int curParticleCount = 0; curParticleCount<particlePerBatch; curParticleCount++)
        {
            float particleXfloat = float(rand()%resolution);
            float particleYfloat = float(rand()%resolution);
            float particleSpeed = 0;
            float particlePrevSpeed = 0;
            float prevflowDirX = 0;
            float prevflowDirY = 0;
            const int maxParticleStep = (resolution * 2)/3;
            for( int particleStepCounter = 0; particleStepCounter < maxParticleStep; particleStepCounter++)
            {
                int particlePosXfloor = floor(particleXfloat);
                int particlePosYfloor = floor(particleYfloat);
                if(particlePosXfloor < 0 || particlePosXfloor >= resolution || particlePosYfloor < 0 || particlePosYfloor >= resolution){
                    break;
                }
                
                int XposPlus1 = fmin(particlePosXfloor + 1,resolution - 1);
                int XposMinus1 = fmax(particlePosXfloor - 1,0);
                int YposPlus1 = fmin(particlePosYfloor + 1,resolution - 1);
                int YposMinus1 = fmax(particlePosYfloor - 1,0);
                
                float rightHeight = valueAtUnsafe(heightMap, XposPlus1, particlePosYfloor, resolution).r;
                float leftHeight = valueAtUnsafe(heightMap, XposMinus1, particlePosYfloor, resolution).r;
                float topHeight = valueAtUnsafe(heightMap, particlePosXfloor, YposPlus1, resolution).r;
                float bottomHeight = valueAtUnsafe(heightMap, particlePosXfloor, YposMinus1, resolution).r;
                
                float ourHeight = valueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution).r;
                
                float deltaR = ourHeight - rightHeight;
                float deltaL = ourHeight - leftHeight;
                float deltaT = ourHeight - topHeight;
                float deltaB = ourHeight - bottomHeight;
                
//                float flowDirX = fmax(0.0,fmax(deltaR,deltaL));
//                float flowDirY = fmax(0.0,fmax(deltaT,deltaB));
                float flowDirX = fmax(deltaR,deltaL);
                float flowDirY = fmax(deltaT,deltaB);
                
                //if all neighbors are too high for us to flow to, drop everything and end this particle's life
                if(deltaR < talusAngle && deltaL < talusAngle && deltaT < talusAngle && deltaB < talusAngle)
                {
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)) * -1.0 + talusAngle * 3);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)) * -1.0 + talusAngle * 3);
                    break;
                }
                if(flowDirX == 0.0 && flowDirY == 0.0)
                {
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed);
                    break;
                }
                
                if(deltaR > deltaL)
                    flowDirX *= -1.0;
                if(deltaT > deltaB)
                    flowDirY *= -1.0;
                
                flowDirX = flowDirX * 0.1 + prevflowDirX * 0.9;
                flowDirY = flowDirY * 0.1 + prevflowDirY * 0.9;

                prevflowDirX = flowDirX;
                prevflowDirY = flowDirY;
                
                particleSpeed = lengthVec2(flowDirX,flowDirY);

                float mostSignificantFlowDirMagnitude = fmax(abs(flowDirX),abs(flowDirY));
                flowDirX /= mostSignificantFlowDirMagnitude;
                flowDirY /= mostSignificantFlowDirMagnitude;
                
                // check where we are going
                if(floor(particleXfloat - flowDirX) < 0 || floor(particleXfloat - flowDirX) >= resolution || floor(particleYfloat - flowDirY) < 0 || floor(particleYfloat - flowDirY) >= resolution)
                {
                    break;
                }
                float deltaHeightOfWereWereGoing = ourHeight - valueAtUnsafe(heightMap, floor(particleXfloat - flowDirX), floor(particleYfloat - flowDirY), resolution).r;
                if( deltaHeightOfWereWereGoing < 0)
                {
                    break;
                }
                
                float acceleration = particleSpeed - particlePrevSpeed;
                
                //if we accelerated, erode
                if(acceleration > 0)
                {
                    float rockHardness = hardnessFromXY(particlePosXfloor, particlePosYfloor, hardnessMap);
                    acceleration *= 1.0 - rockHardness;
                    float eroded_amount = acceleration * -1.0 * std::min(2.0 - sedimentation, 1.0) * 1.0;
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, eroded_amount);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, eroded_amount);
                }
                else if(acceleration < 0)//else, depose
                {
                    float deposed_amount = -acceleration * std::min(sedimentation, 1.0) * 1.0;
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, deposed_amount);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, deposed_amount);
                }
                particlePrevSpeed = particleSpeed;
                particleXfloat -= flowDirX;
                particleYfloat -= flowDirY;
            }
        }
        printf("batch %i/%i done\n",curbatch,batchcount);
    }
    return heightMap;
}

float * wordlMachineMicroErode(float* heightMap, float* hardnessMap)
{
    const float scale = 500.0;
    /*const*/ float rockHardness = 0.0; //works best with 0.0
    const float talusAngle = 0.000025;
    const int batchcount = 32;
    const int resolution = 2048;
    
    for( int curbatch = 0; curbatch < batchcount; curbatch++)
    {
        const int particlePerBatch = (resolution*resolution)/100;
        for( int curParticleCount = 0; curParticleCount<particlePerBatch; curParticleCount++)
        {
            float particleXfloat = float(rand()%resolution);
            float particleYfloat = float(rand()%resolution);
            float particleSpeed = 0;
            float particlePrevSpeed = 0;
            float prevflowDirX = 0;
            float prevflowDirY = 0;
            const int maxParticleStep = (resolution * 2)/3;
            for( int particleStepCounter = 0; particleStepCounter < maxParticleStep; particleStepCounter++)
            {
                int particlePosXfloor = floor(particleXfloat);
                int particlePosYfloor = floor(particleYfloat);
                if(particlePosXfloor < 0 || particlePosXfloor >= resolution || particlePosYfloor < 0 || particlePosYfloor >= resolution){
                    break;
                }
                
                int XposPlus1 = std::min(particlePosXfloor + 1,resolution - 1);
                int XposMinus1 = std::max(particlePosXfloor - 1,0);
                int YposPlus1 = std::min(particlePosYfloor + 1,resolution - 1);
                int YposMinus1 = std::max(particlePosYfloor - 1,0);
                
                float rightHeight = valueAtUnsafe(heightMap, XposPlus1, particlePosYfloor, resolution).r/scale;
                float leftHeight = valueAtUnsafe(heightMap, XposMinus1, particlePosYfloor, resolution).r/scale;
                float topHeight = valueAtUnsafe(heightMap, particlePosXfloor, YposPlus1, resolution).r/scale;
                float bottomHeight = valueAtUnsafe(heightMap, particlePosXfloor, YposMinus1, resolution).r/scale;
                
                float ourHeight = valueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution).r/scale;
                
                float deltaR = ourHeight - rightHeight;
                float deltaL = ourHeight - leftHeight;
                float deltaT = ourHeight - topHeight;
                float deltaB = ourHeight - bottomHeight;
                
                float flowDirX = fmax(0.0,fmax(deltaR,deltaL));
                float flowDirY = fmax(0.0,fmax(deltaT,deltaB));
                
                //if all neighbors are too high for us to flow to, drop everything and end this particle's life
                if(deltaR < talusAngle && deltaL < talusAngle && deltaT < talusAngle && deltaB < talusAngle)
                {
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)) * -1.0 + talusAngle * 3);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)) * -1.0 + talusAngle * 3);
                    break;
                }
                if(flowDirX == 0.0 && flowDirY == 0.0)
                {
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed);
                    break;
                }
                
                flowDirX = flowDirX * 0.1 + prevflowDirX * 0.9;
                flowDirY = flowDirY * 0.1 + prevflowDirY * 0.9;
                
                prevflowDirX = flowDirX;
                prevflowDirY = flowDirY;
                
                particleSpeed = lengthVec2(flowDirX,flowDirY);
                
                float mostSignificantFlowDir = std::max(abs(flowDirX),abs(flowDirY));
                
                float flowDirMultiplier = 1.0/mostSignificantFlowDir;
                
                flowDirX *= flowDirMultiplier;
                flowDirY *= flowDirMultiplier;
                
                if(deltaR > deltaL)
                    flowDirX *= -1.0;
                if(deltaT > deltaB)
                    flowDirY *= -1.0;
                
                // check where we are going
                if(floor(particleXfloat - flowDirX) < 0 || floor(particleXfloat - flowDirX) >= resolution || floor(particleYfloat - flowDirY) < 0 || floor(particleYfloat - flowDirY) >= resolution)
                {
                    break;
                }
                float deltaHeightOfWereWereGoing = ourHeight - valueAtUnsafe(heightMap, floor(particleXfloat - flowDirX), floor(particleYfloat - flowDirY), resolution).r;
                if( deltaHeightOfWereWereGoing < 0)
                {
                    break;
                }
                
                float acceleration = particleSpeed - particlePrevSpeed;
                
                //if we accelerated, erode
                if(acceleration > 0)
                {
                    // code mutation begins here
                    
                    rockHardness = hardnessFromXY(particlePosXfloor, particlePosYfloor, hardnessMap);
                    
                    // and ends here
                    
                    acceleration *= 1.0 - rockHardness;
//                    acceleration *= 0.5;
                    float eroded_amount = acceleration * -1.0 * scale * 5;
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, eroded_amount);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, eroded_amount);
                }
                else if(acceleration < 0)//else, depose
                {
                    float deceleration = -acceleration;
//                    deceleration *= 0.5;
                    float deposed_amount = deceleration * scale * 0.5;
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, deposed_amount);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, deposed_amount);
                }
                
                //particleSpeed = (particleSpeed + particlePrevSpeed) / 2.0;
                particlePrevSpeed = particleSpeed;
                particleXfloat -= flowDirX;
                particleYfloat -= flowDirY;
            }
        }
        printf("batch %i/%i done\n",curbatch,batchcount);
    }
    return heightMap;
}

float * wordlMachineErode(float* heightMap, int resolution, int batchcount, float talusAngle, float erosionStrength)
{
    float scale = 500.0 / 2.0;
    //scale = resolution / 4.0;
    scale = (500.0 * 2048.0) / resolution;
    //talusAngle = 0;
    //scale = 500.0;
    
    float rockHardness = 0.0;
    
    for( int curbatch = 0; curbatch < batchcount; curbatch++)
    {
        int particlePerBatch = (resolution*resolution)/100;
        for( int curParticleCount = 0; curParticleCount<particlePerBatch; curParticleCount++)
        {
            float particleXfloat = float(rand()%resolution);
            float particleYfloat = float(rand()%resolution);
            float particleSpeed = 0;
            float particlePrevSpeed = 0;
            float prevflowDirX = 0;
            float prevflowDirY = 0;
            int maxParticleStep = (resolution * 2)/3;
            for( int particleStepCounter = 0; particleStepCounter < maxParticleStep; particleStepCounter++)
            {
                int particlePosXfloor = floor(particleXfloat);
                int particlePosYfloor = floor(particleYfloat);
                if(particlePosXfloor < 0 || particlePosXfloor >= resolution || particlePosYfloor < 0 || particlePosYfloor >= resolution){
                    break; //exit if we drop out of the map
                }
                
                int XposPlus1 = std::min(particlePosXfloor + 1,resolution - 1);
                int XposMinus1 = std::max(particlePosXfloor - 1,0);
                int YposPlus1 = std::min(particlePosYfloor + 1,resolution - 1);
                int YposMinus1 = std::max(particlePosYfloor - 1,0);
                
                float rightHeight = valueAt(heightMap, XposPlus1, particlePosYfloor, resolution).r/scale;
                float leftHeight = valueAt(heightMap, XposMinus1, particlePosYfloor, resolution).r/scale;
                float topHeight = valueAt(heightMap, particlePosXfloor, YposPlus1, resolution).r/scale;
                float bottomHeight = valueAt(heightMap, particlePosXfloor, YposMinus1, resolution).r/scale;
                
                float ourHeight = valueAt(heightMap, particlePosXfloor, particlePosYfloor, resolution).r/scale;
                
                float deltaR = ourHeight - rightHeight;
                float deltaL = ourHeight - leftHeight;
                float deltaT = ourHeight - topHeight;
                float deltaB = ourHeight - bottomHeight;
                
                float flowDirX = fmax(0.0,fmax(deltaR,deltaL));
                float flowDirY = fmax(0.0,fmax(deltaT,deltaB));
                
                //if all neighbors are too high for us to flow to, drop everything and end this particle's life
                if(deltaR < talusAngle && deltaL < talusAngle && deltaT < talusAngle && deltaB < talusAngle)
                {
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)) * -1.0 + talusAngle * 3);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)) * -1.0 + talusAngle * 3);
                    break;
                }
                if(flowDirX == 0.0 && flowDirY == 0.0)
                {
                    addValueAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed);
                    addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed);
                    break;
                }
                
                flowDirX = flowDirX * 0.1 + prevflowDirX * 0.9;
                flowDirY = flowDirY * 0.1 + prevflowDirY * 0.9;
                
                particleSpeed = lengthVec2(flowDirX,flowDirY);
                
                float mostSignificantFlowDir = std::max(abs(flowDirX),abs(flowDirY));
                float flowDirMultiplier = 1.0/mostSignificantFlowDir;
                flowDirX *= flowDirMultiplier;
                flowDirY *= flowDirMultiplier;
                
                if(deltaR > deltaL)
                    flowDirX *= -1.0;
                if(deltaT > deltaB)
                    flowDirY *= -1.0;

                // check where we are going
                if(floor(particleXfloat - flowDirX) < 0 || floor(particleXfloat - flowDirX) >= resolution || floor(particleYfloat - flowDirY) < 0 || floor(particleYfloat - flowDirY) >= resolution)
                {
                    break;
                }
                float deltaHeightOfWereWereGoing = ourHeight - valueAtUnsafe(heightMap, floor(particleXfloat - flowDirX), floor(particleYfloat - flowDirY), resolution).r;
                if( deltaHeightOfWereWereGoing < 0)
                {
                    break;
                }
                
                float acceleration = particleSpeed - particlePrevSpeed;
                
                //if we accelerated, erode
                if(acceleration > 0)
                {
                    acceleration *= 1.0 - rockHardness;
                    acceleration *= 0.5;
                    addValueAt(heightMap, particlePosXfloor, particlePosYfloor, resolution, acceleration * -1.0 * scale * erosionStrength);
                    addSedimentAt(heightMap, particlePosXfloor, particlePosYfloor, resolution, acceleration * -1.0 * scale * erosionStrength);
                    //addToWearMap(acceleration)
                }
                else if(acceleration < 0)//else, depose
                {
                    float deceleration = -acceleration;
                    deceleration *= 0.5;
                    addValueAt(heightMap, particlePosXfloor, particlePosYfloor, resolution, deceleration * scale * erosionStrength);
                    addSedimentAt(heightMap, particlePosXfloor, particlePosYfloor, resolution, deceleration * scale * erosionStrength);
                }
                
                //particleSpeed = (particleSpeed + particlePrevSpeed) / 2.0;
                particlePrevSpeed = particleSpeed;
                particleXfloat -= flowDirX;
                particleYfloat -= flowDirY;
            }
        }
        if(batchcount >= 200){
            if(curbatch % 100 == 0){
                printf("batch %i/%i done\n",curbatch,batchcount);
            }
        } else {
            printf("batch %i/%i done\n",curbatch,batchcount);
        }
    }
    return heightMap;
}

float* CreateDownscaledMapArray(float* highResMapArray, int resolution, int targetResolution)
{
    float* downscaledMapArray = (float*)malloc(targetResolution * targetResolution * 2 * 4); //two floats so * 2 * 4
    for(int x = 0 ; x < targetResolution; x++)
    {
        for(int y = 0 ; y < targetResolution; y++)
        {
            //heights
            downscaledMapArray[(x * targetResolution + y) * 2] = highResMapArray[((x * 2) * resolution + (y * 2)) * 2];
            downscaledMapArray[(x * targetResolution + y) * 2] += highResMapArray[((x * 2 + 1) * resolution + (y * 2)) * 2];
            downscaledMapArray[(x * targetResolution + y) * 2] += highResMapArray[((x * 2) * resolution + (y * 2 + 1)) * 2];
            downscaledMapArray[(x * targetResolution + y) * 2] += highResMapArray[((x * 2 + 1) * resolution + (y * 2 + 1)) * 2];
            downscaledMapArray[(x * targetResolution + y) * 2] /= 4.0f;
            
            //seds
            downscaledMapArray[(x * targetResolution + y) * 2 + 1] = highResMapArray[((x * 2) * resolution + (y * 2)) * 2 + 1];
            downscaledMapArray[(x * targetResolution + y) * 2 + 1] += highResMapArray[((x * 2 + 1) * resolution + (y * 2)) * 2 + 1];
            downscaledMapArray[(x * targetResolution + y) * 2 + 1] += highResMapArray[((x * 2) * resolution + (y * 2 + 1)) * 2 + 1];
            downscaledMapArray[(x * targetResolution + y) * 2 + 1] += highResMapArray[((x * 2 + 1) * resolution + (y * 2 + 1)) * 2 + 1];
            downscaledMapArray[(x * targetResolution + y) * 2 + 1] /= 4.0f;
        }
    }
    return downscaledMapArray;
}

void CombineMapArraysIntoFirstArg(float* destinationMap, int destResolution, float* sourceMap, int srcResolution)
{
    for(int x = 0 ; x < destResolution - 1; x++)
    {
        for(int y = 0 ; y < destResolution - 1; y++)
        {
            float upliftFactor = 1.1;
            ///*
            //heights
//            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + (y / 2)) * 2] * 0.25f;
//            destinationMap[(x * destResolution + y) * 2] += sourceMap[(((x + 1) / 2) * srcResolution + (y / 2)) * 2] * 0.25f;
//            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + ((y + 1) / 2)) * 2] * 0.25f;
//            destinationMap[(x * destResolution + y) * 2] += sourceMap[(((x + 1) / 2) * srcResolution + ((y + 1) / 2)) * 2] * 0.25f;
//            destinationMap[(x * destResolution + y) * 2] /= 2.0f;
            
            destinationMap[(x * destResolution + y) * 2] = sourceMap[((x / 2) * srcResolution + (y / 2)) * 2] * 0.25f * upliftFactor;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[(((x + 1) / 2) * srcResolution + (y / 2)) * 2] * 0.25f * upliftFactor;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + ((y + 1) / 2)) * 2] * 0.25f * upliftFactor;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[(((x + 1) / 2) * srcResolution + ((y + 1) / 2)) * 2] * 0.25f * upliftFactor;
            //destinationMap[(x * destResolution + y) * 2] /= 2.0f;
            
            //seds
            destinationMap[(x * destResolution + y) * 2 + 1] += sourceMap[((x / 2) * srcResolution + (y / 2)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2 + 1] += sourceMap[(((x + 1) / 2) * srcResolution + (y / 2)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2 + 1] += sourceMap[((x / 2) * srcResolution + ((y + 1) / 2)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2 + 1] += sourceMap[(((x + 1) / 2) * srcResolution + ((y + 1) / 2)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2 + 1] /= 2.0f;
            //*/
            /*
            //heights
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + (y / 2)) * 2] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2 + 1) * srcResolution + (y / 2)) * 2] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + (y / 2 + 1)) * 2] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2 + 1) * srcResolution + (y / 2 + 1)) * 2] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] /= 2.0f;
            
            //seds
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + (y / 2)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2 + 1) * srcResolution + (y / 2)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2) * srcResolution + (y / 2 + 1)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] += sourceMap[((x / 2 + 1) * srcResolution + (y / 2 + 1)) * 2 + 1] * 0.25f;
            destinationMap[(x * destResolution + y) * 2] /= 2.0f;
            */
        }
    }
}

GLuint CPUParticleErode(GLuint bhm, int resolution, int iterations, float talusAngle, float erosionStrength)
{
    float* bhmArray = PutBhmIntoArray(bhm, resolution);
    float* downscaledMap2 = CreateDownscaledMapArray(bhmArray, resolution, resolution/2);
    printf("downscaled\n");
    float* downscaledMap4 = CreateDownscaledMapArray(downscaledMap2, resolution/2, resolution/4);
    printf("downscaled\n");
//    float* downscaledMap8 = CreateDownscaledMapArray(downscaledMap4, resolution/4, resolution/8);
//    printf("downscaled\n");
//    wordlMachineErode(downscaledMap8, resolution/8, iterations, talusAngle, erosionStrength);
//    CombineMapArraysIntoFirstArg(downscaledMap4, resolution/4, downscaledMap8, resolution/8);
//    printf("upscaled\n");
    wordlMachineErode(downscaledMap4, resolution/4, iterations, talusAngle, erosionStrength);
    CombineMapArraysIntoFirstArg(downscaledMap2, resolution/2, downscaledMap4, resolution/4);
    printf("upscaled\n");
    wordlMachineErode(downscaledMap2, resolution/2, iterations, talusAngle, erosionStrength);
    CombineMapArraysIntoFirstArg(bhmArray, resolution, downscaledMap2, resolution/2);
    printf("upscaled\n");
    return PutErrodedMapIntoBhm(bhmArray,resolution);
}

static int SEED = 0;

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int+1, y_int);
    int u = noise2(x_int, y_int+1);
    int v = noise2(x_int+1, y_int+1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

struct MapData{
    float* bhmArray;
    float* hardnessMap;
    double sedimentation;
};

// The function to be executed by all threads
void *Erode32Step_MT(void *vargp)
{
    // Store the value argument passed to this thread
    MapData* mapdata = (MapData*)vargp;
    
    wordlMachineErodeAt2048optimised(mapdata->bhmArray, mapdata->hardnessMap, mapdata->sedimentation);
    
    printf("Thread finished\n");
    
    return NULL;
}

GLuint CPUParticleErode128Steps(GLuint bhm, int resolution, double sedimentation)
{
    float* hardnessMap = (float*)malloc(resolution * resolution * 4);
    for (int x=0; x<2048; x++) {
        for (int y=0; y<2048; y++) {
//            float randval = noise2d(((float)x)/15.0,((float)y)/15.0);
//            hardnessMap[x * 2048 + y] = randval > 100.0 ? 0.5 : 0;
//            hardnessMap[x * 2048 + y] += randval > 200.0 ? 0.5 : 0;
//            hardnessMap[x * 2048 + y] = 1 - (randval / 256.0) * (randval / 256.0);
            
            hardnessMap[x * 2048 + y] = 0;
        }
    }
    static float* bhmArray = PutBhmIntoArray(bhm, resolution);
    static MapData mapdata;
    mapdata.bhmArray = bhmArray;
    mapdata.sedimentation = sedimentation;
    mapdata.hardnessMap = hardnessMap;
    //wordlMachineErodeAt2048optimised(bhmArray, hardnessMap, 0.1);
//    for (int x=0; x<2048; x++) {
//        for (int y=0; y<2048; y++) {
//            bhmArray[(x * 2048 + y) * 2] = hardnessMap[x * 2048 + y];
//        }
//    }
    
    #define T_NUM 8
    
    pthread_t tids[T_NUM];
    for (int i = 0; i < T_NUM; i++)
        pthread_create(&tids[i], NULL, Erode32Step_MT, (void *)&mapdata);

    for (int i = 0; i < T_NUM; i++)
        pthread_join(tids[i], NULL);
    
    return PutErrodedMapIntoBhm(bhmArray,resolution);
}

GLuint CPUParticleMicroErode(GLuint bhm, int resolution)
{
    float* hardnessMap = (float*)malloc(resolution * resolution * 4);
    for (int x=0; x<2048; x++) {
        for (int y=0; y<2048; y++) {
            float randval = noise2d(((float)x + 123)/8.0,((float)y + 456)/8.0);
            hardnessMap[x * 2048 + y] = randval > 100.0 ? 0.3 : 0;
            hardnessMap[x * 2048 + y] += randval > 200.0 ? 0.5 : 0;
            
            hardnessMap[x * 2048 + y] = 0;
        }
    }
    float* bhmArray = PutBhmIntoArray(bhm, resolution);
    wordlMachineMicroErode(bhmArray, hardnessMap);
    return PutErrodedMapIntoBhm(bhmArray,resolution);
}

GLuint CPUParticleErodeOnePass(GLuint bhm, int resolution, int iterations, float talusAngle, float erosionStrength)
{
    float* bhmArray = PutBhmIntoArray(bhm, resolution);
    wordlMachineErode(bhmArray, resolution, iterations, talusAngle, erosionStrength);
    return PutErrodedMapIntoBhm(bhmArray,resolution);
}

float* PutBhmIntoArray(GLuint bhm, int resolution)
{
    GLfloat* pixelData = (float*)malloc(resolution * resolution * 3 * 4); //bhm is RGB32F, hence the 3 * 4
    
    glBindTexture(GL_TEXTURE_2D, bhm);
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_FLOAT,pixelData);
    
    float* heightValues = (float*)malloc(resolution * resolution * 2 * 4); //no rgb anymore, we only want r (height) and g (sediments)
    
    for(int i = 0 ; i < resolution * resolution ; i++)
    {
        heightValues[i * 2] = pixelData[i * 3] * 500; //select r value, which is height
        heightValues[i * 2 + 1] = pixelData[i * 3 + 1] * 500; //select g value, which is sediments
    }
    
    free(pixelData);
    return heightValues;
}

GLuint PutErrodedMapIntoBhm(float* erodedMap, int resolution)
{
    float* bhmAsAnArray = (float*)malloc(resolution * resolution * 3 * 4); //bhm is RGB32F, hence the 3 * 4
    for(int i = 0 ; i < resolution * resolution ; i++)
    {
        bhmAsAnArray[i * 3] = erodedMap[i * 2]/500.0; //put height back
        bhmAsAnArray[i * 3 + 1] = erodedMap[i * 2 + 1]/500.0; //put sediments back
        bhmAsAnArray[i * 3 + 2] = 0; //put zeo in b channel
    }
    
    GLuint bhm;
    
    glGenTextures(1, &bhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution, resolution, 0, GL_RGB, GL_FLOAT, bhmAsAnArray);// R for height, G for sediment, B unused for now
    
    free(bhmAsAnArray);

    return bhm;
}
