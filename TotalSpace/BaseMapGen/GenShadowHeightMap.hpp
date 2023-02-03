//
//  GenShadowHeightMap.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 09/10/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GenShadowHeightMap_hpp
#define GenShadowHeightMap_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

GLuint GenShadowHeightMap(GLuint bhm, int resolution, float sunX, float sunY, float sunZ);
float* PutBhmHeightIntoArray(GLuint bhm, int resolution);
float* ComputeShadowHeightMap(float* heightMap, int resolution, float sunX, float sunY, float sunZ);
GLuint PutShadowHeightMapIntoTexture(float* shadowHeightMap, int resolution);

void computeShadowHeightMapIntoLightMap(GLuint bhm, GLuint shadowHeightMap, GLuint lightMap);

#endif /* GenShadowHeightMap_hpp */
