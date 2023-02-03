//
//  PerlinDisplace.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 13/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "PerlinDisplace.hpp"
#include "ShaderLoader.hpp"

void perlinDisplace(GLuint bhm, int bhmSize, int seed, float amplitude, float frequency, float flipHeightMap)
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
    GLuint displace  = LoadAndLinkShader("passThru.vert", "BaseMapGen/perlinDisplace.frag");

    //make a copy of the heightmap
    glUseProgram(duplicateBhm);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightMapCopy, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //and now displace
    glUseProgram(displace);
    glUniform1i(glGetUniformLocation(displace, "seed"),seed);
    glUniform1f(glGetUniformLocation(displace, "amplitude"),amplitude);
    glUniform1f(glGetUniformLocation(displace, "frequency"),frequency);
    glUniform1f(glGetUniformLocation(displace, "flipHeightMap"),flipHeightMap);
    glBindTexture(GL_TEXTURE_2D, heightMapCopy);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
