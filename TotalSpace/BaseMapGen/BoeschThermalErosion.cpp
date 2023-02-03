//
//  BoeschThermalErosion.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 29/05/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "BoeschThermalErosion.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

void boeschErode(GLuint bhm, int bhmRes, GLuint hardnessMap, int iterations)
{
    //shaders
    GLuint boeschProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/boeschErode.frag");
    
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //tmpBhm text gen
    GLuint tmpBhm;
    glGenTextures(1, &tmpBhm);
    glBindTexture(GL_TEXTURE_2D, tmpBhm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER | GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, bhmRes, bhmRes, 0, GL_RGB, GL_FLOAT, NULL);
    
    glUseProgram(boeschProgram);
    
    GLint bhmTexLoc = glGetUniformLocation(boeschProgram, "bhm");
    GLint hardnessTexLoc  = glGetUniformLocation(boeschProgram, "hardness");
    glUniform1i(bhmTexLoc, 0);
    glUniform1i(hardnessTexLoc, 1);
    
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, hardnessMap);
    glActiveTexture(GL_TEXTURE0 + 0);
    
    int imax = iterations;
    for (int i = 0; i < imax; i++) { // erosion ping pong loop
        //ping
        glBindTexture(GL_TEXTURE_2D, bhm);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpBhm, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //maybe useless idk ??
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        glBindTexture(GL_TEXTURE_2D, tmpBhm);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //maybe useless idk ??
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    //unbind everything
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}
