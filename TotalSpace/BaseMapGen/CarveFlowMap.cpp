//
//  CarveFlowMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 23/01/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "CarveFlowMap.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

void carveFlowMap(GLuint bhm, float strength)
{
    GLuint heightMapCopy; //copy of bhm,
    glGenTextures(1, &heightMapCopy);
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 2048, 2048, 0, GL_RGB, GL_FLOAT, NULL);
    
    GLuint duplicateBhm  = LoadAndLinkShader("passThru.vert", "duplicateBhm.frag"); // simply duplicates a bhm
    GLuint carveShader  = LoadAndLinkShader("passThru.vert", "BaseMapGen/carveFlowMap.frag");

    //make a copy of the heightmap
    glUseProgram(duplicateBhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightMapCopy, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //and now displace
    glUseProgram(carveShader);
    glUniform1f(glGetUniformLocation(carveShader, "strength"),strength);
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
