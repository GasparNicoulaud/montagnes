//
//  BlurMaxSediment.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 08/05/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "BlurMaxSediment.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"


void BlurMaxSediment(GLuint bhm, int bhmRes, int blurIter, float blurOffset)
{
    //gen a blurmap
    
    //make a copy of the heightmap
    
    //max(blurmap,copy of heightmap) into bhm
    
    GLuint blurMap; //will contain a blurred version of bhm,
    glGenTextures(1, &blurMap);
    glBindTexture(GL_TEXTURE_2D, blurMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    std::vector<GLushort> emptyDataSmaller(bhmRes * bhmRes, 0);  // 1 glushort of zeroes per pixel  , for heightmap texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, bhmRes, bhmRes, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &emptyDataSmaller[0]);
    
    GLuint blurMapCopy; //second texture for ping pong blurring
    glGenTextures(1, &blurMapCopy);
    glBindTexture(GL_TEXTURE_2D, blurMapCopy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, bhmRes, bhmRes, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &emptyDataSmaller[0]);
    
    GLuint heightMapCopy; //copy of bhm,
    glGenTextures(1, &heightMapCopy);
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmRes, bhmRes, 0, GL_RGB, GL_FLOAT, NULL);
    
    //shaders
    GLuint blurMapShader  = LoadAndLinkShader("passThru.vert", "BaseMapGen/blurMap.frag"); //blurring shader
    GLuint blurMapPingPongShader  = LoadAndLinkShader("passThru.vert", "BaseMapGen/blurMapPingPong.frag"); //blurring ping pong shader
    GLuint duplicateBhm  = LoadAndLinkShader("passThru.vert", "duplicateBhm.frag"); // simply duplicates a bhm
    GLuint maxBlurShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/maxBlur.frag"); // does the max(blurmap,copy of heightmap)
    
    //create blurmap from BHM
    glUseProgram(blurMapShader);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //blurring ping pong
    for (int i = 0; i < blurIter - 1; i++) { // -1 because the 'create blurmap from BHM' step already blurs the map once
        glUseProgram(blurMapPingPongShader);
        glBindTexture(GL_TEXTURE_2D, blurMap);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurMapCopy, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glBindTexture(GL_TEXTURE_2D, blurMapCopy);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    
    //make a copy of the heightmap
    glUseProgram(duplicateBhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightMapCopy, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //max(blurmap,copy of heightmap) into bhm
    glUseProgram(maxBlurShader);
    glUniform1f(glGetUniformLocation(maxBlurShader, "bluroffset"),blurOffset);
    GLuint bhmCopyLoc  = glGetUniformLocation(maxBlurShader, "bhmCopy");
    GLuint blurMapLoc = glGetUniformLocation(maxBlurShader, "blurMap");
    glUniform1i(bhmCopyLoc, 0);
    glUniform1i(blurMapLoc, 1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, blurMap);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glActiveTexture(GL_TEXTURE0); // Texture unit 0
}


