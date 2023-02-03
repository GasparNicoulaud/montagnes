//
//  Sedimentation.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 08/12/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#ifndef Sedimentation_hpp
#define Sedimentation_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GLuint Sedimentation(GLuint bhm, int resolution, int iterations, float talusAngle);
float * PutBhmIntoArraySd(GLuint bhm, int resolution);
float * sediment(float* heightMap, int resolution, int batchcount, float talusAngle);
GLuint PutErrodedMapIntoBhmSd(float* erodedMap, int resolution);
glm::vec2 valueAtSd(float* heightMap, int i, int j, int resolution);
void addValueAtSd(float* heightMap, int i, int j, int resolution, float value);

#endif /* Sedimentation_hpp */
