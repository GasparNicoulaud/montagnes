//
//  TalusCarve.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 23/10/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "TalusCarve.hpp"
#include "ShaderLoader.hpp"

void talusCarve(GLuint bhm, int bhmSize, float slopeFactor)
{
    //tmpBhm text gen
    GLuint tmpBhm;
    glGenTextures(1, &tmpBhm);
    glBindTexture(GL_TEXTURE_2D, tmpBhm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmSize, bhmSize, 0, GL_RGB, GL_FLOAT, NULL);

    GLuint pyramidize  = LoadAndLinkShader("passThru.vert", "BaseMapGen/talusCarve.frag");
    slopeFactor = slopeFactor * (0.015 - 0.003) + 0.003;// 0.015 should be the maximum, it does nearly nothing, 0.003 minimum as it erodes hard.
    glUseProgram(pyramidize);
    glUniform1f(glGetUniformLocation(pyramidize, "slopeFactor"),slopeFactor);
 
    int imax = 100; //good talus carve cycles : 100
    for (int i = 0; i < imax; i++) { // pyramidize ping pong loop
         //ping
         glBindTexture(GL_TEXTURE_2D, bhm);
         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpBhm, 0);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glDrawArrays(GL_TRIANGLES, 0, 6);
        
         //pong
         glBindTexture(GL_TEXTURE_2D, tmpBhm);
         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glDrawArrays(GL_TRIANGLES, 0, 6);
     }
 }

