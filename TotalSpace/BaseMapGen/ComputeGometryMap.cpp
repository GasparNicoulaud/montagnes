//
//  ComputeGometryMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 19/02/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "ComputeGometryMap.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

//creates a texture for r for height, g for concavity, b & a for normals, but only does r for now
void computeGeometryMap(GLuint bhm, GLuint *geometryMap)
{
    glGenTextures(1, geometryMap);
    //printf("Geometry map identifier at generation : %i\n", *geometryMap);
    glBindTexture(GL_TEXTURE_2D, *geometryMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 2048, 0, GL_RGBA, GL_FLOAT, NULL); //r for height, g for concavity, b & a for normals
    
    GLuint geometryShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/geometryMapGenerator.frag");
    
    glUseProgram(geometryShader);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *geometryMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}
