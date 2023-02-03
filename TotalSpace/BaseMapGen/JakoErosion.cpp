//
//  JakoErosion.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 12/02/18.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include <assert.h>
#include "JakoErosion.hpp"
#include "ShaderLoader.hpp"

void jakoErode(GLuint bhm, int bhmRes, int iterations)
{
    //textures, one and two for the ping pong method
    GLuint terrainOne, terrainTwo; // GL_RGBA16UI, maybe we can compress it into RGB16UI (16b for terrain, 14 for sed, 14 for water and 4 for hardness)
    GLuint pipesOne, pipesTwo; //GL_RGBA32UI 32 * 4 bits = sed pipes + water pipes, each 8 directions and each ui 8bits
    
    //shaders
    GLuint erosShader = LoadAndLinkShader("passThru.vert", "jako.frag");
    GLuint bhmToJako  = LoadAndLinkShader("passThru.vert", "bhmToJako.frag");
    GLuint jakoToBhm  = LoadAndLinkShader("passThru.vert", "jakoToBhm.frag");
    GLuint postErosion  = LoadAndLinkShader("passThru.vert", "postErosion.frag");
    
    
    std::vector<GLushort> emptyData(bhmRes * bhmRes * 4, 0);  // 4 glushort of zeroes per pixel  , for terrain texture
    std::vector<GLuint> emptyDataLarger(bhmRes * bhmRes * 4, 0); // 4 gluint of zeroes per pixel , for pipe texture
    
    
    //terrain textures
    glGenTextures(1, &terrainOne);
    glBindTexture(GL_TEXTURE_2D, terrainOne);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI, bhmRes, bhmRes, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, &emptyData[0]);
    
    glGenTextures(1, &terrainTwo);
    glBindTexture(GL_TEXTURE_2D, terrainTwo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI, bhmRes, bhmRes, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, &emptyData[0]);
    
    
    //pipe textures
    glGenTextures(1, &pipesOne);
    glBindTexture(GL_TEXTURE_2D, pipesOne);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, bhmRes, bhmRes, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &emptyDataLarger[0]);
    
    glGenTextures(1, &pipesTwo);
    glBindTexture(GL_TEXTURE_2D, pipesTwo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, bhmRes, bhmRes, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &emptyDataLarger[0]);
    
    
    //put data from bhm into terrainOne
    
    
    GLfloat floatval;
    glReadPixels(bhmRes/2,bhmRes/2,1,1,GL_RED,GL_FLOAT, &floatval);
    printf("value at middle of the map : %f\n",floatval);
    
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    glUseProgram(bhmToJako);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainOne, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
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
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);

    
    
    glUseProgram(erosShader);
    glUniform1f(glGetUniformLocation(erosShader, "lod"),0.0);
    GLint terrainTexLoc = glGetUniformLocation(erosShader, "terrain");
    GLint pipesTexLoc  = glGetUniformLocation(erosShader, "pipes");
    glUniform1i(terrainTexLoc, 0);
    glUniform1i(pipesTexLoc, 1);
    
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    
    if(iterations <= 25)
    {
        glUniform1f(glGetUniformLocation(erosShader, "lod"),4.5);
    }
    
    int imax = iterations; //was 600
    for (int i = 0; i < imax; i++) { // erosion ping pong loop
        
        //ping
        
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, pipesOne);
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, terrainOne);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainTwo, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,   pipesTwo, 0);
        
        glDrawBuffers(2,  attachments);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, pipesTwo);
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, terrainTwo);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainOne, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,   pipesOne, 0);
        
        glDrawBuffers(2,  attachments);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
    }
    
    
    attachments[1] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  attachments);
    
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
    
    // put the map back in bhm
    glUseProgram(jakoToBhm);
    glBindTexture(GL_TEXTURE_2D, terrainOne);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpBhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // post erosion warp
    glUseProgram(postErosion);
    glBindTexture(GL_TEXTURE_2D, tmpBhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    
}


void prepareTexturesJako(GLuint* terrain1, GLuint* terrain2, GLuint* waterOutflow1, GLuint* waterOutflow2, GLuint* velField1, GLuint* velField2, GLuint* soilFlowCardinals1, GLuint* soilFlowCardinals2, GLuint* soilFlowCorners1, GLuint* soilFlowCorners2, GLuint bhm, int bhmRes, int seed)
{
    float allZero[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    //allocate all textures
    //terrain textures
    glGenTextures(1, terrain1);
    glBindTexture(GL_TEXTURE_2D, *terrain1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, allZero);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //rgb r rock, g water, b suspended sediments, a hardness
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bhmRes, bhmRes, 0, GL_RGBA, GL_FLOAT, NULL);
    
    glGenTextures(1, terrain2);
    glBindTexture(GL_TEXTURE_2D, *terrain2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, allZero);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //rgb r rock, g water, b suspended sediments, a hardness
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bhmRes, bhmRes, 0, GL_RGBA, GL_FLOAT, NULL);
    
    //water pipe textures
    glGenTextures(1, waterOutflow1);
    glBindTexture(GL_TEXTURE_2D, *waterOutflow1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, allZero);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bhmRes, bhmRes, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    
    glGenTextures(1, waterOutflow2);
    glBindTexture(GL_TEXTURE_2D, *waterOutflow2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, allZero);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bhmRes, bhmRes, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    
    //velocity fiels
    glGenTextures(1, velField1);
    glBindTexture(GL_TEXTURE_2D, *velField1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, allZero);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, bhmRes, bhmRes, 0, GL_RG, GL_FLOAT, NULL);
    
    glGenTextures(1, velField2);
    glBindTexture(GL_TEXTURE_2D, *velField2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, allZero);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, bhmRes, bhmRes, 0, GL_RG, GL_FLOAT, NULL);
    
    
    //init necessary textures
    GLuint initShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/JakoV2/initTextures.frag");
    glUseProgram(initShader);
    glUniform1i(glGetUniformLocation(initShader, "seed"),seed);
    
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *terrain1, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *waterOutflow1, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *velField1, 0);;
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    glDrawBuffers(3, attachments);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void erodeLoopJakoOpt(GLuint* terrain1, GLuint* terrain2, GLuint* waterOutflow1, GLuint* waterOutflow2, GLuint* velField1, GLuint* velField2, int iterations, float jako_rain_rate)
{
    GLuint firstPass = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/JakoV2/firstPass.frag");
    glUseProgram(firstPass);
    GLint hmLoc1  = glGetUniformLocation(firstPass, "terrain");
    GLint watLoc1 = glGetUniformLocation(firstPass, "waterOutflow");
    GLint rain_rateLoc = glGetUniformLocation(firstPass, "global_rain_rate");
    glUniform1i(hmLoc1, 0);
    glUniform1i(watLoc1, 1);
    glUniform1f(rain_rateLoc, jako_rain_rate);
    GLint iLoc = glGetUniformLocation(firstPass, "iteration");
    
    GLuint secondPass = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/JakoV2/secondPass.frag");
    glUseProgram(secondPass);
    GLint hmLoc2  = glGetUniformLocation(secondPass, "terrain");
    GLint watLoc2 = glGetUniformLocation(secondPass, "waterOutflow");
    glUniform1i(hmLoc2, 0);
    glUniform1i(watLoc2, 1);
    
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    //iterations = 1000;
    for(int i = 0; i < iterations ; i++)
    {
        //ping
        glUseProgram(firstPass);
        glUniform1i(iLoc,i);
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, *waterOutflow1);
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, *terrain1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *terrain2, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *waterOutflow2, 0);
        glDrawBuffers(2,  attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //pong
        glUseProgram(secondPass);
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, *waterOutflow2);
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, *terrain2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *terrain1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *waterOutflow1, 0);
        glDrawBuffers(2,  attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        if((i + 1) % 500 == 0)
        {
            glFinish();
            printf("took a small break in JakoV2 erosion at iteration %i\n",i + 1);
        }
    }
    
    GLuint oneAttachment[1] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  oneAttachment);
}

void putResultIntoBhmJako(GLuint terrain, GLuint bhm, int bhmSize)
{
    GLuint heightMapCopy; //copy of bhm,
    glGenTextures(1, &heightMapCopy);
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmSize, bhmSize, 0, GL_RGB, GL_FLOAT, NULL);
    
    GLuint duplicateBhm  = LoadAndLinkShader("passThru.vert", "duplicateBhm.frag"); // simply duplicates a bhm

    //make a copy of the heightmap
    glUseProgram(duplicateBhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightMapCopy, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //
    GLuint shader = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/JakoV2/heightMapBackToBhm.frag");
    glUseProgram(shader);
    GLint tloc  = glGetUniformLocation(shader, "terrain");
    GLint hmCopyLoc = glGetUniformLocation(shader, "bhmCopy");
    glUniform1i(tloc, 0);
    glUniform1i(hmCopyLoc, 1);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, terrain);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void jakoErodeV2(GLuint bhm, int bhmRes, int iterations, int seed, float jako_rain_rate)
{
    GLuint terrain1, terrain2; //rgb r rock, g water, b suspended sediments
    GLuint waterOutflow1, waterOutflow2; //rgba water pipes
    GLuint velField1, velField2; //rg velocity field
    
    GLuint soilFlowCardinals1, soilFlowCardinals2; //rgba soil flow pipes right left top bottom
    GLuint soilFlowCorners1, soilFlowCorners2; //rgba soil flow pipes top-right top-left bottom-right bottom-left
    
    if(iterations > 0)
    {
        prepareTexturesJako(&terrain1, &terrain2, &waterOutflow1, &waterOutflow2, &velField1, &velField2, &soilFlowCardinals1, &soilFlowCardinals2, &soilFlowCorners1, &soilFlowCorners2, bhm, bhmRes, seed + 321);
        erodeLoopJakoOpt(&terrain1, &terrain2, &waterOutflow1, &waterOutflow2, &velField1, &velField2, iterations, jako_rain_rate);
        putResultIntoBhmJako(terrain1, bhm, bhmRes);
    }
}



