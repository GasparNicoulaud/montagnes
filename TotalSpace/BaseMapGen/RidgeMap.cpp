//
//  CrestMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 06/03/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include <assert.h>
#include "RidgeMap.hpp"
#include "ShaderLoader.hpp"

void genDLARidgeMap(GLuint bhm, int seed)
{
    float seedf = (seed % 9999999) / 99999.0;
    int ridgeMapRes = 256;
    float iterCoef = 1;
    GLuint ridgeShader = LoadAndLinkShader("passThru.vert", "ridge.frag");
    GLuint ridgeGrowShader = LoadAndLinkShader("passThru.vert", "DLAGrow.frag");
    GLuint ridgeGrowShaderNoDisk = LoadAndLinkShader("passThru.vert", "DLAGrowNoDisk.frag");
    GLuint majorRidgeMap;
    GLuint ridgeTexOne;
    GLuint ridgeTexTwo;
    
    GLuint ridgeTexOneHD;
    GLuint ridgeTexTwoHD;
    
    //std::vector<GLubyte> randomData(2048 * 2048 * 2, 1);  // 1 test

    
    GLuint ridgeTexOne16UI;
    GLuint ridgeTexTwo16UI;
    
    glGenTextures(1, &ridgeTexOne16UI);
    glBindTexture(GL_TEXTURE_2D, ridgeTexOne16UI);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, 2048, 2048, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);
    
    glGenTextures(1, &ridgeTexTwo16UI);
    glBindTexture(GL_TEXTURE_2D, ridgeTexTwo16UI);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, 2048, 2048, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);
    
    std::vector<GLubyte> emptyData(ridgeMapRes * ridgeMapRes, 0);  // 1 glubyte of zeroes per pixel
    
    glGenTextures(1, &ridgeTexOne);
    glBindTexture(GL_TEXTURE_2D, ridgeTexOne);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, ridgeMapRes, ridgeMapRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    
    glGenTextures(1, &ridgeTexTwo);
    glBindTexture(GL_TEXTURE_2D, ridgeTexTwo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, ridgeMapRes, ridgeMapRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    
    glGenTextures(1, &ridgeTexOneHD);
    glBindTexture(GL_TEXTURE_2D, ridgeTexOneHD);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 2048, 2048, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    
    glGenTextures(1, &ridgeTexTwoHD);
    glBindTexture(GL_TEXTURE_2D, ridgeTexTwoHD);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 2048, 2048, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    
    glViewport(0, 0, ridgeMapRes, ridgeMapRes);
    
    //seedf = 1.0;
    // gen seeds
    glUseProgram(ridgeShader);
    glUniform1f(glGetUniformLocation(ridgeShader, "seed"), seedf * 3 + 41);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Disk version, for starter
    //DLA growth loop
    glUseProgram(ridgeGrowShader);
    GLint timeLoc = glGetUniformLocation(ridgeGrowShader, "time");
    
    int imax = 200 * iterCoef; //default 300
    for (int i = 0; i < imax; i++) { // ridge growth ping pong loop
        
        glUniform1f(timeLoc,i + seedf);
        
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeTexOne);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexTwo, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glUniform1f(timeLoc,i + 0.5 + seedf);
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeTexTwo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    //From then on, nodisk shader, which is much faster
    glUseProgram(ridgeGrowShaderNoDisk);
    timeLoc = glGetUniformLocation(ridgeGrowShaderNoDisk, "time");
    
    seedf *= seedf;
    
    imax = 1300 * iterCoef; //default 2500
    for (int i = 0; i < imax; i++) { // ridge growth ping pong loop
        
        glUniform1f(timeLoc,i + seedf);
        
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeTexOne);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexTwo, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glUniform1f(timeLoc,i + 0.5 + seedf);
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeTexTwo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    ridgeMapReduce(ridgeTexOne,ridgeTexTwo);
    
    glGenTextures(1, &majorRidgeMap);
    glBindTexture(GL_TEXTURE_2D, majorRidgeMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, ridgeMapRes, ridgeMapRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, ridgeMapRes, ridgeMapRes);
    
    glUseProgram(ridgeGrowShaderNoDisk);
    timeLoc = glGetUniformLocation(ridgeGrowShaderNoDisk, "time");

    seedf *= 123.456;
    
    imax = 300 * iterCoef; //default 400
    for (int i = 0; i < imax; i++) { // ridge growth ping pong loop
        
        glUniform1f(timeLoc,i + seedf);
        
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeTexOne);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexTwo, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glUniform1f(timeLoc,i + 0.5 + seedf);
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeTexTwo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    ridgeMapUpscale(majorRidgeMap, ridgeTexOne, ridgeTexOneHD, ridgeTexTwoHD);
    
    ridgeMapPyramidize(ridgeTexOneHD, ridgeTexOne16UI, ridgeTexTwo16UI,seedf);
    
    ridgeMapToBhm(bhm, ridgeTexOne16UI);

}

void ridgeMapToBhm(GLuint bhm, GLuint ridgeTexOne16UI)
{
    GLuint ridgeToBhm  = LoadAndLinkShader("passThru.vert", "RidgeToBhm.frag");
    glUseProgram(ridgeToBhm);
    
    glBindTexture(GL_TEXTURE_2D, ridgeTexOne16UI);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ridgeMapPyramidize(GLuint ridgeTexOneHD, GLuint ridgeTexOne16UI, GLuint ridgeTexTwo16UI, float seed)
{
    GLuint byteToShort= LoadAndLinkShader("passThru.vert", "byteToShort.frag");
    glUseProgram(byteToShort);
    
    glBindTexture(GL_TEXTURE_2D, ridgeTexOneHD);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne16UI, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    GLuint ridgeSlope  = LoadAndLinkShader("passThru.vert", "ridgeSlope.frag");
    
    glUseProgram(ridgeSlope);
    
    int imax = 700; //default 20 maybe ?
    for (int i = 0; i < imax; i++) { // ridge slope ping pong loop
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeTexOne16UI);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexTwo16UI, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeTexTwo16UI);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne16UI, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    GLuint perlinize= LoadAndLinkShader("passThru.vert", "perlinize.frag");
    glUseProgram(perlinize);
    glUniform1f(glGetUniformLocation(perlinize, "seed"), seed - 3 * 41);
    
    glBindTexture(GL_TEXTURE_2D, ridgeTexTwo16UI);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne16UI, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    GLuint pyramidize  = LoadAndLinkShader("passThru.vert", "pyramidize.frag");
    
    glUseProgram(pyramidize);
    
    imax = 300; //good pyramidization cycles : 300
    for (int i = 0; i < imax; i++) { // pyramidize ping pong loop
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeTexOne16UI);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexTwo16UI, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeTexTwo16UI);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOne16UI, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void ridgeMapUpscale(GLuint majorRidgeMap, GLuint minorRidgeMap, GLuint ridgeTexOneHD, GLuint ridgeTexTwoHD)
{
    GLuint upscaleNearest = LoadAndLinkShader("passThru.vert", "upscaleNearest.frag");
    GLuint upscaleRefine = LoadAndLinkShader("passThru.vert", "upscaleRefine.frag");
    
    glViewport(0, 0, 2048, 2048);

    glUseProgram(upscaleNearest);
    GLuint minor  = glGetUniformLocation(upscaleNearest, "sampler");
    GLuint major = glGetUniformLocation(upscaleNearest, "majorRidge");
    glUniform1i(minor, 0);
    glUniform1i(major, 1);
    glActiveTexture(GL_TEXTURE0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, minorRidgeMap);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, majorRidgeMap);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOneHD, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    
    //refine loop
    glUseProgram(upscaleRefine);
    
    int imax = 1;
    for (int i = 0; i < imax; i++) { // reduce ping pong loop
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeTexOneHD);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexTwoHD, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeTexTwoHD);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeTexOneHD, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}


void ridgeMapReduce(GLuint ridgeMap1, GLuint ridgeMap2)
{
    
    GLuint ridgeReduceShader = LoadAndLinkShader("passThru.vert", "ridgeMapReduce.frag");
    
    //reduce loop
    glUseProgram(ridgeReduceShader);
    
    int imax = 20;
    for (int i = 0; i < imax; i++) { // reduce ping pong loop
        
        
        //ping
        glBindTexture(GL_TEXTURE_2D, ridgeMap1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeMap2, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        glBindTexture(GL_TEXTURE_2D, ridgeMap2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ridgeMap1, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}









