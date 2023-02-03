//
//  Sedimentation.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 08/12/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "Sedimentation.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

glm::vec2 valueAtSd(float* heightMap, int i, int j, int resolution){
    if(i < 0 || i >= resolution || j < 0 || j >= resolution ){
        return glm::vec2(0);
    }
    return glm::vec2(heightMap[(i * resolution + j) * 2],heightMap[(i * resolution + j) * 2 + 1]);
}

void addValueAtSd(float* heightMap, int i, int j, int resolution, float value){
    if(i >= 0 && i < resolution && j >= 0 && j < resolution ){
        heightMap[(i * resolution + j) * 2] += value;
        heightMap[(i * resolution + j) * 2 + 1] += value;
    }
}

float lengthVec2Sd(float x, float y)
{
    return sqrt(x*x + y*y);
}

float * sediment(float* heightMap, int resolution, int batchcount, float talusAngle)
{
    int abortedBatches = 0;
    for( int curbatch = 0; curbatch < batchcount; curbatch++)
    {
        int particlePerBatch = (resolution*resolution)/100;
        float Xsource = float(rand()%resolution);
        float Ysource = float(rand()%resolution);
        for( int curParticleCount = 0; curParticleCount<particlePerBatch; curParticleCount++)
        {
            float particleXfloat = Xsource;
            float particleYfloat = Ysource;
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
                
                float right = valueAtSd(heightMap, XposPlus1, particlePosYfloor, resolution).r;
                float left = valueAtSd(heightMap, XposMinus1, particlePosYfloor, resolution).r;
                float top = valueAtSd(heightMap, particlePosXfloor, YposPlus1, resolution).r;
                float bottom = valueAtSd(heightMap, particlePosXfloor, YposMinus1, resolution).r;

                float topright = valueAtSd(heightMap, XposPlus1, YposPlus1, resolution).r;
                float topleft = valueAtSd(heightMap, XposMinus1, YposPlus1, resolution).r;
                float bottomright = valueAtSd(heightMap, XposPlus1, YposPlus1, resolution).r;
                float bottomleft = valueAtSd(heightMap, XposMinus1, YposMinus1, resolution).r;
                
                float ourHeight = valueAtSd(heightMap, particlePosXfloor, particlePosYfloor, resolution).r;
                
                float deltaR = ourHeight - right;
                float deltaL = ourHeight - left;
                float deltaT = ourHeight - top;
                float deltaB = ourHeight - bottom;
                
                float deltaTR = (ourHeight - topright)/sqrt(2);
                float deltaTL = (ourHeight - topleft)/sqrt(2);
                float deltaBR = (ourHeight - bottomright)/sqrt(2);
                float deltaBL = (ourHeight - bottomleft)/sqrt(2);
                
                float minHeightToFlow = talusAngle * 0.5 + talusAngle * (0.5 - fmin(particleStepCounter * 0.002,0.5));
                //if all neighbors are too high for us to flow to, drop everything and end this particle's life
                if(
                   deltaR < minHeightToFlow &&
                   deltaL < minHeightToFlow &&
                   deltaT < minHeightToFlow &&
                   deltaB < minHeightToFlow &&
                   deltaTR < minHeightToFlow &&
                   deltaTL < minHeightToFlow &&
                   deltaBR < minHeightToFlow &&
                   deltaBL < minHeightToFlow
                )
                {
                    if(particleStepCounter < 10){
                        curParticleCount = particlePerBatch; // exit this location
                        abortedBatches++;
                        break;
                    }
                    float valToDrop = fmax(0,fmax(
                                           fmax(fmax(deltaR,deltaL),fmax(deltaT,deltaB)),
                                           fmax(fmax(deltaTR,deltaTL),fmax(deltaBR,deltaBL))
                                           )) + minHeightToFlow * 0.1;
                    
                    addValueAtSd(heightMap, particlePosXfloor, particlePosYfloor, resolution, valToDrop);
                    break;
                }
                
                
                if(
                   deltaR >= deltaL &&
                   deltaR >= deltaT &&
                   deltaR >= deltaB &&
                   deltaR >= deltaTR &&
                   deltaR >= deltaTL &&
                   deltaR >= deltaBR &&
                   deltaR >= deltaBL
                )
                {
                    particleXfloat += 1;
                    particleYfloat += 0;
                    continue;
                }
                if(
                   deltaL >= deltaR &&
                   deltaL >= deltaT &&
                   deltaL >= deltaB &&
                   deltaL >= deltaTR &&
                   deltaL >= deltaTL &&
                   deltaL >= deltaBR &&
                   deltaL >= deltaBL
                )
                {
                    particleXfloat += -1;
                    particleYfloat += 0;
                    continue;
                }
                if(
                   deltaT >= deltaR &&
                   deltaT >= deltaL &&
                   deltaT >= deltaB &&
                   deltaT >= deltaTR &&
                   deltaT >= deltaTL &&
                   deltaT >= deltaBR &&
                   deltaT >= deltaBL
                )
                {
                    particleXfloat += 0;
                    particleYfloat += 1;
                    continue;
                }
                if(
                   deltaB >= deltaR &&
                   deltaB >= deltaL &&
                   deltaB >= deltaT &&
                   deltaB >= deltaTR &&
                   deltaB >= deltaTL &&
                   deltaB >= deltaBR &&
                   deltaB >= deltaBL
                )
                {
                    particleXfloat += 0;
                    particleYfloat += -1;
                    continue;
                }
                
                if(deltaTR >= deltaR &&
                   deltaTR >= deltaL &&
                   deltaTR >= deltaT &&
                   deltaTR >= deltaB &&
                   deltaTR >= deltaTL &&
                   deltaTR >= deltaBR &&
                   deltaTR >= deltaBL)
                {
                    particleXfloat += 1;
                    particleYfloat += 1;
                    continue;
                }
                
                if(
                   deltaTL >= deltaR &&
                   deltaTL >= deltaL &&
                   deltaTL >= deltaT &&
                   deltaTL >= deltaB &&
                   deltaTL >= deltaTR &&
                   deltaTL >= deltaBR &&
                   deltaTL >= deltaBL
                )
                {
                    particleXfloat += -1;
                    particleYfloat += 1;
                    continue;
                }
                if(
                   deltaBR >= deltaR &&
                   deltaBR >= deltaL &&
                   deltaBR >= deltaT &&
                   deltaBR >= deltaB &&
                   deltaBR >= deltaTR &&
                   deltaBR >= deltaTL &&
                   deltaBR >= deltaBL
                )
                {
                    particleXfloat += 1;
                    particleYfloat += -1;
                    continue;
                }
                if(
                   deltaBL >= deltaR &&
                   deltaBL >= deltaL &&
                   deltaBL >= deltaT &&
                   deltaBL >= deltaB &&
                   deltaBL >= deltaTR &&
                   deltaBL >= deltaTL &&
                   deltaBL >= deltaBR
                )
                {
                    particleXfloat += -1;
                    particleYfloat += -1;
                    continue;
                }
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
    
    printf("total aborted batches %i\n",abortedBatches);
    return heightMap;
}

struct MapDataSD{
    float* bhmArray;
    int resolution;
    int iterations;
    float talusAngle;
};

// The function to be executed by all threads
void *ErodeStepSD_MT(void *vargp)
{
    // Store the value argument passed to this thread
    MapDataSD* mapdata = (MapDataSD*)vargp;
    
    sediment(mapdata->bhmArray, mapdata->resolution, mapdata->iterations, mapdata->talusAngle);
    
    printf("Thread finished\n");
    
    return NULL;
}

GLuint Sedimentation(GLuint bhm, int resolution, int iterations, float talusAngle)
{
    float* bhmArray = PutBhmIntoArraySd(bhm, resolution);
    static MapDataSD mapdata;
    mapdata.bhmArray = bhmArray;
    mapdata.resolution = resolution;
    mapdata.iterations = iterations / 8;
    mapdata.talusAngle = talusAngle;
    
    #define T_NUM 8

    pthread_t tids[T_NUM];
    for (int i = 0; i < T_NUM; i++)
        pthread_create(&tids[i], NULL, ErodeStepSD_MT, (void *)&mapdata);

    for (int i = 0; i < T_NUM; i++)
        pthread_join(tids[i], NULL);
    
    return PutErrodedMapIntoBhmSd(bhmArray,resolution);
}

float* PutBhmIntoArraySd(GLuint bhm, int resolution)
{
    GLfloat* pixelData = (float*)malloc(resolution * resolution * 3 * 4); //bhm is RGB32F, hence the 3 * 4
    
    glBindTexture(GL_TEXTURE_2D, bhm);
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_FLOAT,pixelData);
    
    float* heightValues = (float*)malloc(resolution * resolution * 2 * 4); //no rgb anymore, we only want r (height) and g (sediments)
    
    for(int i = 0 ; i < resolution * resolution ; i++)
    {
        heightValues[i * 2] = pixelData[i * 3]; //select r value, which is height
        heightValues[i * 2 + 1] = pixelData[i * 3 + 1]; //select g value, which is sediments
    }
    
    free(pixelData);
    return heightValues;
}

GLuint PutErrodedMapIntoBhmSd(float* erodedMap, int resolution)
{
    float* bhmAsAnArray = (float*)malloc(resolution * resolution * 3 * 4); //bhm is RGB32F, hence the 3 * 4
    for(int i = 0 ; i < resolution * resolution ; i++)
    {
        bhmAsAnArray[i * 3] = erodedMap[i * 2]; //put height back
        bhmAsAnArray[i * 3 + 1] = erodedMap[i * 2 + 1]; //put sediments back
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
