//
//  RidgeVertexToBHM.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 11/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "RidgeVertexToBHM.hpp"
#include "ShaderLoader.hpp"

void ridgeVerticesIntoBHM(float *ridgeVertices, int RidgeVertexCount, GLuint bhm, int bhmSize)
{
    GLuint ridgeToBhm = LoadAndLinkShader("passThru.vert", "BaseMapGen/RidgeMapGen/ridgeVerticesToBhm.frag");
    glUseProgram(ridgeToBhm);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint buf;
    glGenBuffers(1, &buf);
    
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, RidgeVertexCount * 3 * 2 * 3 * sizeof(float), ridgeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_LINES, 0, RidgeVertexCount * 3 * 2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void pyramidize(GLuint bhm, int bhmSize, float slopeFactor)
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

    GLuint pyramidize  = LoadAndLinkShader("passThru.vert", "BaseMapGen/RidgeMapGen/pyramidizeFloat.frag");
    
    glUseProgram(pyramidize);
    glUniform1f(glGetUniformLocation(pyramidize, "slopeFactor"),slopeFactor);
 
    int imax = 300; //good pyramidization cycles : 300
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

