//
//  PostErosionFlowMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 14/04/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include <assert.h>
#include "JakoErosion.hpp"
#include "ShaderLoader.hpp"

void addFlowMap(GLuint bhm, int bhmRes)
{
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    GLuint heightMap; // only the height of the terrain,
    glGenTextures(1, &heightMap);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    std::vector<GLushort> emptyDataSmaller(bhmRes * bhmRes, 0);  // 1 glushort of zeroes per pixel  , for heightmap texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, bhmRes, bhmRes, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &emptyDataSmaller[0]);
    
    
    //flowmap textures, one and two for the ping pong method
    GLuint flowMapOne, flowMapTwo; // GL_RG16UI, R : 13bits for current water, 3bits for direction choosed G : 16bits for flow trace
    glGenTextures(1, &flowMapOne);
    glBindTexture(GL_TEXTURE_2D, flowMapOne);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    std::vector<GLushort> emptyData(bhmRes * bhmRes * 3, 0);  // 2 glushort of zeroes per pixel  , for flowmap texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16UI, bhmRes, bhmRes, 0, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, &emptyData[0]);
    
    glGenTextures(1, &flowMapTwo);
    glBindTexture(GL_TEXTURE_2D, flowMapTwo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16UI, bhmRes, bhmRes, 0, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, &emptyData[0]);
    
    //shaders
     //creates heightmap from BHM
    GLuint bhmToHeightMap  = LoadAndLinkShader("passThru.vert", "BaseMapGen/FlowMapGen/bhmToHeightMap.frag");
    
     // read from heightmap and flowmap and write to flowmap
    GLuint flowShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/FlowMapGen/flowMap.frag");
    
    // read G channel from flowmap and write to bhm B channel
    GLuint flowToBhm  = LoadAndLinkShader("passThru.vert", "BaseMapGen/FlowMapGen/flowToBhm.frag");
    
    // simply duplicates a bhm
    GLuint duplicateBhm  = LoadAndLinkShader("passThru.vert", "duplicateBhm.frag");
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //create heightmap from BHM
    glUseProgram(bhmToHeightMap);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    //now the flowmap loop
    
    glUseProgram(flowShader);
    GLuint fcLoc = glGetUniformLocation(flowShader, "frameCount");
    
    GLuint hmLoc  = glGetUniformLocation(flowShader, "heightMap");
    GLuint flowLoc = glGetUniformLocation(flowShader, "flowMap");
    glUniform1i(hmLoc, 0);
    glUniform1i(flowLoc, 1);
    
    glActiveTexture(GL_TEXTURE0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, heightMap);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    
    for (int i = 0; i < 50; i++) { // flowmap ping pong loop
        //ping
        glUniform1i(fcLoc,i*2); //framecounter for flow occasionnal randomisation
        glBindTexture(GL_TEXTURE_2D, flowMapOne);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, flowMapTwo, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        glUniform1i(fcLoc,i*2 + 1);
        glBindTexture(GL_TEXTURE_2D, flowMapTwo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, flowMapOne, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0); // Texture unit 0

    //flowmap loop done, now flowtobhm
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //create bhm carbon copy first
    GLuint bhmCopy;
    glGenTextures(1, &bhmCopy);
    glBindTexture(GL_TEXTURE_2D, bhmCopy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmRes, bhmRes, 0, GL_RGB, GL_FLOAT, NULL);
    
    glUseProgram(duplicateBhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhmCopy, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //done duplicating bhm
    
    glUseProgram(flowToBhm);
    GLuint bhmLoc  = glGetUniformLocation(flowToBhm, "bhm");
    flowLoc = glGetUniformLocation(flowToBhm, "flowMap");
    glUniform1i(bhmLoc, 0);
    glUniform1i(flowLoc, 1);
    glBindTexture(GL_TEXTURE_2D, bhmCopy);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, flowMapOne);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glActiveTexture(GL_TEXTURE0); // Texture unit 0
}



