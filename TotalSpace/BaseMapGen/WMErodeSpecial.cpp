//
//  WMErodeSpecial.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 30/10/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "WMErodeSpecial.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

glm::vec2 valueAtS(float* heightMap, int i, int j, int resolution){
    if(i < 0 || i >= resolution || j < 0 || j >= resolution ){
        return glm::vec2(0);
    }
    return glm::vec2(heightMap[(i * resolution + j) * 2],heightMap[(i * resolution + j) * 2 + 1]);
}

void addValueAtS(float* heightMap, int i, int j, int resolution, float value){
    if(i >= 0 && i < resolution && j >= 0 && j < resolution ){
        heightMap[(i * resolution + j) * 2] += value;
    }
}

void addValueSpecial5(float* heightMap, int i, int j, int resolution, float value){
    
    value *= 1.0/273.0;
    value *= 5;
    
    addValueAtS(heightMap, i - 2, j - 2, resolution, value * 1);
    addValueAtS(heightMap, i - 1, j - 2, resolution, value * 4);
    addValueAtS(heightMap, i    , j - 2, resolution, value * 7);
    addValueAtS(heightMap, i + 1, j - 2, resolution, value * 4);
    addValueAtS(heightMap, i + 2, j - 2, resolution, value * 1);
    
    addValueAtS(heightMap, i - 2, j - 1, resolution, value * 4);
    addValueAtS(heightMap, i - 1, j - 1, resolution, value * 16);
    addValueAtS(heightMap, i    , j - 1, resolution, value * 26);
    addValueAtS(heightMap, i + 1, j - 1, resolution, value * 16);
    addValueAtS(heightMap, i + 2, j - 1, resolution, value * 4);
    
    addValueAtS(heightMap, i - 2, j    , resolution, value * 7);
    addValueAtS(heightMap, i - 1, j    , resolution, value * 26);
    addValueAtS(heightMap, i    , j    , resolution, value * 41);
    addValueAtS(heightMap, i + 1, j    , resolution, value * 26);
    addValueAtS(heightMap, i + 2, j    , resolution, value * 7);
    
    addValueAtS(heightMap, i - 2, j + 1, resolution, value * 4);
    addValueAtS(heightMap, i - 1, j + 1, resolution, value * 16);
    addValueAtS(heightMap, i    , j + 1, resolution, value * 26);
    addValueAtS(heightMap, i + 1, j + 1, resolution, value * 16);
    addValueAtS(heightMap, i + 2, j + 1, resolution, value * 4);
    
    addValueAtS(heightMap, i - 2, j + 2, resolution, value * 1);
    addValueAtS(heightMap, i - 1, j + 2, resolution, value * 4);
    addValueAtS(heightMap, i    , j + 2, resolution, value * 7);
    addValueAtS(heightMap, i + 1, j + 2, resolution, value * 4);
    addValueAtS(heightMap, i + 2, j + 2, resolution, value * 1);
}

// Function to create Gaussian filter
void FilterCreation(double GKernel[][9])
{
    // initialising standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;
 
    // sum is for normalization
    double sum = 0.0;
 
    // generating 5x5 kernel
    for (int x = -4; x <= 4; x++) {
        for (int y = -4; y <= 4; y++) {
            r = sqrt(x * x + y * y);
            GKernel[x + 4][y + 4] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += GKernel[x + 4][y + 4];
        }
    }
 
    // normalising the Kernel
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            GKernel[i][j] /= sum;
}

void addValueSpecial9(float* heightMap, int x, int y, int resolution, float value){
    
    double GKernel[9][9];
    FilterCreation(GKernel);

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            //printf("%f   ",GKernel[i][j]);
            addValueAtS(heightMap, x + i - 4, y + j - 4, resolution, value * GKernel[i][j] * 3);
        }
        //printf("\n");
    }
    
}

float lengthVec2S(float x, float y)
{
    return sqrt(x*x + y*y);
}

float * WMErodeSpecialInner(float* heightMap, float sedimentation, int batchcount)
{
    const float scale = 500.0;
    //const float sedimentCarryCapacity = 1.0;
    const float talusAngle = 0.00002;
    const int resolution = 2048;
    
    for(int curbatch = 0; curbatch < batchcount; curbatch++)
    {
        const int particlePerBatch = (resolution*resolution)/100;
        for(int curParticleCount = 0; curParticleCount<particlePerBatch; curParticleCount++)
        {
            float particleXfloat = float(rand()%resolution);
            float particleYfloat = float(rand()%resolution);
            float particleSpeed = 0;
            float particlePrevSpeed = 0;
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
                
                float rightHeight = valueAtS(heightMap, XposPlus1, particlePosYfloor, resolution).r/scale;
                float leftHeight = valueAtS(heightMap, XposMinus1, particlePosYfloor, resolution).r/scale;
                float topHeight = valueAtS(heightMap, particlePosXfloor, YposPlus1, resolution).r/scale;
                float bottomHeight = valueAtS(heightMap, particlePosXfloor, YposMinus1, resolution).r/scale;
                
                float ourHeight = valueAtS(heightMap, particlePosXfloor, particlePosYfloor, resolution).r/scale;
                
                float deltaR = ourHeight - rightHeight;
                float deltaL = ourHeight - leftHeight;
                float deltaT = ourHeight - topHeight;
                float deltaB = ourHeight - bottomHeight;
                
                float minHeightToFlow = talusAngle;
                //if all neighbors are too high for us to flow to, drop everything and end this particle's life
                if(deltaR < minHeightToFlow && deltaL < minHeightToFlow && deltaT < minHeightToFlow && deltaB < minHeightToFlow)
                {
                    addValueSpecial9(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed * scale * 0.5);
                    //addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, particleSpeed * scale);
                    break;
                }
                
                float flowDirX = fmax(0.0,fmax(deltaR,deltaL));
                float flowDirY = fmax(0.0,fmax(deltaT,deltaB));
                
                particleSpeed = lengthVec2S(flowDirX,flowDirY);
                
                float mostSignificantFlowDir = std::max(abs(flowDirX),abs(flowDirY));
                
                float flowDirMultiplier = 1.0/mostSignificantFlowDir;
                
                flowDirX *= flowDirMultiplier;
                flowDirY *= flowDirMultiplier;
                
                if(deltaR > deltaL)
                    flowDirX *= -1.0;
                if(deltaT > deltaB)
                    flowDirY *= -1.0;
                
                //particleSpeed *= 0.75;
                //particleSpeed *= sedimentCarryCapacity;
                
                float acceleration = particleSpeed - particlePrevSpeed;
                
                //if we accelerated, erode
                if(acceleration > 0)
                {
                    // code mutation begins here
                    
                    //rockHardness = hardnessFromXY(particlePosXfloor, particlePosYfloor, hardnessMap);
                    
                    // and ends here
                    acceleration *= 0.2;
                    float eroded_amount = acceleration * -1.0 * scale * std::min(2.0 - sedimentation, 1.0);
                    addValueSpecial9(heightMap, particlePosXfloor, particlePosYfloor, resolution, eroded_amount);
                    //addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, eroded_amount);
                }
                else if(acceleration < 0)//else, depose
                {
                    float deceleration = -acceleration;
                    deceleration *= 0.2;
                    float deposed_amount = deceleration * scale * std::min(sedimentation + 0.0, 1.0);
                    addValueSpecial9(heightMap, particlePosXfloor, particlePosYfloor, resolution, deposed_amount);
                    //addSedimentAtUnsafe(heightMap, particlePosXfloor, particlePosYfloor, resolution, deposed_amount);
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


float* PutBhmIntoArrayS(GLuint bhm, int resolution)
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

GLuint PutErrodedMapIntoBhmS(float* erodedMap, int resolution)
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

struct MapDataS{
    float* bhmArray;
    double sedimentation;
    int iterations;
};

// The function to be executed by all threads
void *ErodeStepS_MT(void *vargp)
{
    // Store the value argument passed to this thread
    MapDataS* mapdata = (MapDataS*)vargp;
    
    WMErodeSpecialInner(mapdata->bhmArray, mapdata->sedimentation, mapdata->iterations);
    
    printf("Thread finished\n");
    
    return NULL;
}

GLuint WMErodeSpecial(GLuint bhm, float sedimentation, int iterations)
{
    static float* bhmArray = PutBhmIntoArrayS(bhm, 2048);
    
    static MapDataS mapdata;
    mapdata.bhmArray = bhmArray;
    mapdata.sedimentation = sedimentation;
    mapdata.iterations = iterations * 0.25;
    
    #define T_NUM 8
    
    pthread_t tids[T_NUM];
    for (int i = 0; i < T_NUM; i++)
        pthread_create(&tids[i], NULL, ErodeStepS_MT, (void *)&mapdata);

    for (int i = 0; i < T_NUM; i++)
        pthread_join(tids[i], NULL);
    
//
//    WMErodeSpecialInner(bhmArray,sedimentation,iterations);
    return PutErrodedMapIntoBhmS(bhmArray,2048);
}
