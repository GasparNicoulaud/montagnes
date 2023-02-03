//
//  GenShadowHeightMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 09/10/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "GenShadowHeightMap.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

GLuint GenShadowHeightMap(GLuint bhm, int resolution, float sunX, float sunY, float sunZ)
{
    float* heightMap = PutBhmHeightIntoArray(bhm, resolution);
    float* shadowHeightMap = ComputeShadowHeightMap(heightMap, resolution, sunX, sunY, sunZ);
    free(heightMap); //was malloc'd when created in PutBhmHeightIntoArray
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    GLuint shadowHeightMapTexture = PutShadowHeightMapIntoTexture(shadowHeightMap, resolution);

    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    return shadowHeightMapTexture;
}

float* PutBhmHeightIntoArray(GLuint bhm, int resolution)
{
    GLsizeiptr bufferSize = resolution * resolution * 3 * 4; //bhm is RGB32F, hence the 3 * 4
    
    GLfloat* pixelData = (float*)malloc(bufferSize);
    
    glBindTexture(GL_TEXTURE_2D, bhm);
    glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  GL_FLOAT,
                  pixelData);
    
    float* heightValues = (float*)malloc(resolution * resolution * 4); //no rgb anymore, we only want r here
    
    for(int i = 0 ; i < resolution * resolution ; i++)
    {
        heightValues[i] = pixelData[i * 3]; //select only r value, which is height
    }
    
    free(pixelData);
        
    return heightValues;
}

float getShadowHeightAt(float* map, int x, int y, int resolution)
{
    if(x < 0 || x >= resolution || y < 0 || y >= resolution)
    {
        return -999;
    }
    else
    {
        return map[x + y * resolution];
    }
}

float fmax(float a, float b){
    return a > b ? a : b;
}

float* ComputeShadowHeightMap(float *heightMap, int resolution, float sunX, float sunY, float sunZ)
{
    float* shadowHeightMap = (float*)malloc(resolution * resolution * 4);
    
    bool beginFromLeft = false;
    bool beginFromTop = false;
    bool beginFromRight = false;
    bool beginFromBottom = false;
    
    float weirdBias = 5.5; //hack to get correct shadow length, no idea why ??? without it shadows are 5.5x too long
    float heightFromMaterials = 98; //materials add 98 units of height...
    heightFromMaterials = 0; // but materials are not taken into account when lightmap is computed !
    
    if(sunX <= 0 && abs(sunX) >= abs(sunY))
    {
        beginFromLeft = true;
    }
    else if (sunY >= 0 && abs(sunY) >= abs(sunX))
    {
        beginFromTop = true;
    }
    else if (sunX >= 0 && abs(sunX) >= abs(sunY))
    {
        beginFromRight = true;
    }
    else
    {
        beginFromBottom = true;
    }

    float Xstep, Ystep, Zstep;
    float currentSunRayPositionX, currentSunRayPositionY;
    
    if(beginFromLeft)
    {
        // TODO : Fix this whole mess
        Xstep = 1.0;
        Ystep = (1.0/abs(sunX)) * sunY;
        Zstep = (1.0/abs(sunX)) * sunZ;
        
        Zstep -= heightFromMaterials;
        Zstep /= 500; //because geometry map is scaled *500
        
        Zstep *= weirdBias; //hack to get correct shadow length, no idea why ??? without it shadows are 5.5x too long
        
        int Xoffset = 1;
        int Yoffset = 0;
        currentSunRayPositionX = 0;
        currentSunRayPositionY = 0;
        for(int x = 0; x < resolution; x++)
        {
            for(int y = 0; y < resolution; y++)
            {
                shadowHeightMap[x + y * resolution] = fmax(heightMap[x + y * resolution], getShadowHeightAt(shadowHeightMap,x - Xoffset,y - Yoffset,resolution) - Zstep);
            }
            
            currentSunRayPositionY += Ystep;
            if(currentSunRayPositionY > 1)
            {
                currentSunRayPositionY -= 1;
                Yoffset = -1;
            }
            else if(currentSunRayPositionY < -1)
            {
                currentSunRayPositionY += 1;
                Yoffset = 1;
            }
            else
            {
                Yoffset = 0;
            }
        }
    }
    else if(beginFromTop)
    {
        // TODO : Fix this whole mess
        Xstep = (1.0/abs(sunY)) * sunX;
        Ystep = 1.0;
        Zstep = (1.0/abs(sunY)) * sunZ;
        
        Zstep -= heightFromMaterials;
        Zstep /= 500; //because geometry map is scaled *500
        
        Zstep *= weirdBias; //hack to get correct shadow length, no idea why ??? without it shadows are 5.5x too long
        
        int Xoffset = 0;
        int Yoffset = -1;
        currentSunRayPositionX = 0;
        currentSunRayPositionY = 0;
        for(int y = resolution - 1; y >= 0; y--)
        {
            for(int x = 0; x < resolution; x++)
            {
                shadowHeightMap[x + y * resolution] = fmax(heightMap[x + y * resolution], getShadowHeightAt(shadowHeightMap,x - Xoffset,y - Yoffset,resolution) - Zstep);
            }
            
            currentSunRayPositionX += Xstep;
            if(currentSunRayPositionX > 1)
            {
                currentSunRayPositionX -= 1;
                Xoffset = -1;
            }
            else if(currentSunRayPositionX < -1)
            {
                currentSunRayPositionX += 1;
                Xoffset = 1;
            }
            else
            {
                Xoffset = 0;
            }
        }
    }
    else if(beginFromRight)
    {
        // TODO : Fix this whole mess
        Xstep = -1.0;
        Ystep = (1.0/abs(sunX)) * sunY;
        Zstep = (1.0/abs(sunX)) * sunZ;
        
        Zstep -= heightFromMaterials;
        Zstep /= 500; //because geometry map is scaled *500
        
        Zstep *= weirdBias; //hack to get correct shadow length, no idea why ??? without it shadows are 5.5x too long
        
        int Xoffset = -1;
        int Yoffset = 0;
        currentSunRayPositionX = 0;
        currentSunRayPositionY = 0;
        for(int x = resolution - 1; x >= 0; x--)
        {
            for(int y = 0; y < resolution; y++)
            {
                shadowHeightMap[x + y * resolution] = fmax(heightMap[x + y * resolution], getShadowHeightAt(shadowHeightMap,x - Xoffset,y - Yoffset,resolution) - Zstep);
            }
            
            currentSunRayPositionY += Ystep;
            if(currentSunRayPositionY > 1)
            {
                currentSunRayPositionY -= 1;
                Yoffset = -1;
            }
            else if(currentSunRayPositionY < -1)
            {
                currentSunRayPositionY += 1;
                Yoffset = 1;
            }
            else
            {
                Yoffset = 0;
            }
        }
    }
    else if(beginFromBottom)
    {
        // TODO : Fix this whole mess
        Xstep = (1.0/abs(sunY)) * sunX;
        Ystep = -1.0;
        Zstep = (1.0/abs(sunY)) * sunZ;
        
        Zstep -= heightFromMaterials;
        Zstep /= 500; //because geometry map is scaled *500
        
        Zstep *= weirdBias; //hack to get correct shadow length, no idea why ??? without it shadows are 5.5x too long
        
        int Xoffset = 0;
        int Yoffset = 1;
        currentSunRayPositionX = 0;
        currentSunRayPositionY = 0;
        for(int y = 0; y < resolution; y++)
        {
            for(int x = 0; x < resolution; x++)
            {
                shadowHeightMap[x + y * resolution] = fmax(heightMap[x + y * resolution], getShadowHeightAt(shadowHeightMap,x - Xoffset,y - Yoffset,resolution) - Zstep);
            }
            
            currentSunRayPositionX += Xstep;
            if(currentSunRayPositionX > 1)
            {
                currentSunRayPositionX -= 1;
                Xoffset = -1;
            }
            else if(currentSunRayPositionX < -1)
            {
                currentSunRayPositionX += 1;
                Xoffset = 1;
            }
            else
            {
                Xoffset = 0;
            }
        }
    }

    
    
    /*// test pattern
    for(int i = 0 ; i < resolution * resolution ; i++)
    {
        shadowHeightMap[i] = i * 0.000001;
    }*/
    
    return shadowHeightMap;
}

GLuint PutShadowHeightMapIntoTexture(float* shadowHeightMap, int resolution)
{
    GLuint shadowHeightMapTexture;
    
    glGenTextures(1, &shadowHeightMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowHeightMapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, resolution, resolution, 0, GL_RED, GL_FLOAT, shadowHeightMap);
    
    return shadowHeightMapTexture;
}

void computeShadowHeightMapIntoLightMap(GLuint bhm, GLuint shadowHeightMap, GLuint lightMap)
{
    GLuint shmIntoLmShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/Lighting/shadowHeightMapIntoLightMap.frag");
    
    glUseProgram(shmIntoLmShader);
    GLuint bhmLoc  = glGetUniformLocation(shmIntoLmShader, "heightMap");
    GLuint shmLoc = glGetUniformLocation(shmIntoLmShader, "shadowHeightMap");
    
    glUniform1i(bhmLoc, 0);
    glUniform1i(shmLoc, 1);
    
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, shadowHeightMap);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, bhm);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightMap, 0);
    
    glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_FALSE); //only write to the drop shadow channel, don't overwrite ao data
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //reste mask to default
    
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}
