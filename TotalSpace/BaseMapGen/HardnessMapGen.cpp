//
//  HardnessMapGen.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 29/05/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "HardnessMapGen.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

GLuint genHardnessMap(GLuint bhm, int resolution, int seed, bool heavyTerracing)
{
    GLuint hardnessMap;
    glGenTextures(1, &hardnessMap);
    glBindTexture(GL_TEXTURE_2D, hardnessMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, resolution, resolution, 0, GL_RED, GL_FLOAT, NULL);//NULL means reserve texture memory, but texels are undefined
    
    GLuint hardnessMapProgram = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/erosHardnessMap.frag");
    
    float terracingFactor = 0.4;
    if(heavyTerracing)
    {
        terracingFactor = 2.0;
    }
    
    //terracingFactor = 0;
    
    glUseProgram(hardnessMapProgram);
    glUniform1i(glGetUniformLocation(hardnessMapProgram, "seed"),seed);
    glUniform1f(glGetUniformLocation(hardnessMapProgram, "terracingFactor"),terracingFactor);
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hardnessMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    return hardnessMap;
}
