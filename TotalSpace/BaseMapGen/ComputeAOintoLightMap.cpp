//
//  ComputeAOintoLightMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 23/01/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "ComputeAOintoLightMap.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

void computeAOintoLightMap(GLuint geometryMap, GLuint lightMap)
{
    GLuint aoShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/Lighting/ambientOcclusion.frag");
    
    glUseProgram(aoShader);
    glBindTexture(GL_TEXTURE_2D, geometryMap);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightMap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
