//
//  NormalAndConvexityMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 13/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "NormalAndConvexityMap.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

void genNormAndConvMap(GLuint bhm, GLuint normalConvexityMap, int bhmSize)
{
    GLuint normAndConvShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/normAndConv.frag");
    
    glUseProgram(normAndConvShader);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalConvexityMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}

void genBlurredNormAndConvMap(GLuint bhm, GLuint normalConvexityMap, int bhmSize)
{
    GLuint blurMap; //will contain a blurred version of bhm,
    glGenTextures(1, &blurMap);
    glBindTexture(GL_TEXTURE_2D, blurMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    // 1 glushort of zeroes per pixel  , for heightmap texture
    std::vector<GLushort> emptyDataSmaller(bhmSize * bhmSize, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, bhmSize, bhmSize, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &emptyDataSmaller[0]);
    
    GLuint blurMapCopy; //second texture for ping pong blurring
    glGenTextures(1, &blurMapCopy);
    glBindTexture(GL_TEXTURE_2D, blurMapCopy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, bhmSize, bhmSize, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &emptyDataSmaller[0]);
    
    GLuint blurMapShader  = LoadAndLinkShader("passThru.vert", "BaseMapGen/blurMap.frag"); //blurring shader
    GLuint blurMapPingPongShader  = LoadAndLinkShader("passThru.vert", "BaseMapGen/blurMapPingPong.frag"); //blurring ping pong shader
    
    //create blurmap from BHM
    glUseProgram(blurMapShader);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //blurring ping pong
    int blurIter = 50;
    for (int i = 0; i < blurIter - 1; i++) {
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
    
    GLuint normAndConvShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/normAndConvUI.frag");
    
    glUseProgram(normAndConvShader);
    glBindTexture(GL_TEXTURE_2D, blurMap);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalConvexityMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}
