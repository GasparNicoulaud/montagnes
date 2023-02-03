//
//  GaspErosion.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 25/05/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "GaspErosion.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

void gaspErode(GLuint bhm, int bhmRes, float seed, int iterations)
{
    //textures
    GLuint hardnessMap; //GL_R8UI
    //GLuint tmpBhm;
    
    GLuint terrain;//, terrainTwo; // GL_RG16UI, terrain R: heght, G: hardness
    GLuint pipes;//, pipesTwo; //GL_RGB32UI 2 * 32 bits = sed pipes each 8 directions and each ui 8bits, and G for terrain height transfer
    
    //shaders
    GLuint gaspHardness = LoadAndLinkShader("passThru.vert", "gaspHardness.frag");
    //GLuint gaspHardnessIntoBhm = LoadAndLinkShader("passThru.vert", "gaspHardnessIntoBhm.frag");
    //GLuint duplicateBhm  = LoadAndLinkShader("passThru.vert", "duplicateBhm.frag"); // simply duplicates a bhm
    
    GLuint bhmToGasp  = LoadAndLinkShader("passThru.vert", "bhmToGasp.frag");
    
    //maybe later try to merge the 2, switching with a uniform to improve perf :
    GLuint gaspErosionTerToPipes  = LoadAndLinkShader("passThru.vert", "gaspErosionTerToPipes.frag");
    GLuint gaspErosionPipesToTer  = LoadAndLinkShader("passThru.vert", "gaspErosionPipesToTer.frag");
    
    GLuint gaspToBhm  = LoadAndLinkShader("passThru.vert", "gaspToBhm.frag");
    
    //std::vector<GLushort> emptyData(bhmRes * bhmRes * 4, 0);  // 4 glushort of zeroes per pixel  , for terrain texture
    std::vector<GLuint> emptyDataLarger(bhmRes * bhmRes * 2, 0); // 2 gluint of zeroes per pixel , for pipe texture
    
    //hardness texture
    glGenTextures(1, &hardnessMap);
    glBindTexture(GL_TEXTURE_2D, hardnessMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, bhmRes, bhmRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    
    // btmphm text gen
    /*glGenTextures(1, &tmpBhm);
    glBindTexture(GL_TEXTURE_2D, tmpBhm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmRes, bhmRes, 0, GL_RGB, GL_FLOAT, NULL);*/
    
    //terrain textures
    glGenTextures(1, &terrain);
    glBindTexture(GL_TEXTURE_2D, terrain);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, bhmRes, bhmRes, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);
    
    /*glGenTextures(1, &terrainTwo);
    glBindTexture(GL_TEXTURE_2D, terrainTwo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, bhmRes, bhmRes, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);*/
    
    
    //pipe textures
    glGenTextures(1, &pipes);
    glBindTexture(GL_TEXTURE_2D, pipes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, bhmRes, bhmRes, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, &emptyDataLarger[0]);
    
    /*glGenTextures(1, &pipesTwo);
    glBindTexture(GL_TEXTURE_2D, pipesTwo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, bhmRes, bhmRes, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, &emptyDataLarger[0]);*/
    
    //hardness map gen
    glUseProgram(gaspHardness);
    glUniform1f(glGetUniformLocation(gaspHardness, "seed"), seed * 4 + 3);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hardnessMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    /*
    glUseProgram(duplicateBhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpBhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    
    glUseProgram(gaspHardnessIntoBhm);
    GLuint bhmLoc  = glGetUniformLocation(gaspHardnessIntoBhm, "bhm");
    GLuint hardnessLoc = glGetUniformLocation(gaspHardnessIntoBhm, "hardness");
    glUniform1i(bhmLoc, 0);
    glUniform1i(hardnessLoc, 1);
    glBindTexture(GL_TEXTURE_2D, tmpBhm);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, hardnessMap);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glActiveTexture(GL_TEXTURE0); // Texture unit 0
     */
    
    
    //put data from bhm into terrainOne
    
    /*
    GLfloat floatval;
    glReadPixels(bhmRes/2,bhmRes/2,1,1,GL_RED,GL_FLOAT, &floatval);
    printf("value at middle of the map : %f\n",floatval);
    
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    */
    
    
    //import bhm into terrainOne
    glUseProgram(bhmToGasp);
    
    GLint bhmLoc = glGetUniformLocation(bhmToGasp, "bhm");
    GLint hardnessLoc  = glGetUniformLocation(bhmToGasp, "hardness");
    glUniform1i(bhmLoc, 0);
    glUniform1i(hardnessLoc, 1);
    
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, bhm);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, hardnessMap);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrain, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    
    /*
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    */
    
    /*GLushort valueAtMiddle[4];
     glReadPixels(bhmRes/2,bhmRes/2,1,1,
     GL_RGBA_INTEGER,    //format
     GL_UNSIGNED_SHORT,  //type
     &valueAtMiddle[0]);
     std::cout << "r: " << valueAtMiddle[0] << std::endl;
     std::cout << "g: " << valueAtMiddle[1] << std::endl;
     std::cout << "b: " << valueAtMiddle[2] << std::endl;
     std::cout << "a: " << valueAtMiddle[3] << std::endl;
     std::cout << std::endl;*/
    /*
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    */
    
    
    
    int imax = iterations; //was 1500
    for (int i = 0; i < imax; i++) { // erosion ping pong loop
        //ping
        
        glUseProgram(gaspErosionTerToPipes);
        
        glBindTexture(GL_TEXTURE_2D, terrain);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pipes, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE); // in order to only modify r channel of terrain, which holds terrain height, and preserve b channel for hardness
        
        glUseProgram(gaspErosionPipesToTer);

        glBindTexture(GL_TEXTURE_2D, pipes);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrain, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //reset mask to default
    }
    
    
    /*
    GLuint tmpBhm;
    // bhm text gen
    glGenTextures(1, &tmpBhm);
    glBindTexture(GL_TEXTURE_2D, tmpBhm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //NULL means reserve texture memory, but texels are undefined
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmRes, bhmRes, 0, GL_RGB, GL_FLOAT, NULL);
    */
    
    // put the map back in bhm
    
    glUseProgram(gaspToBhm);
    glBindTexture(GL_TEXTURE_2D, terrain);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

