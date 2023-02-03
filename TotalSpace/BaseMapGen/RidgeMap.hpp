//
//  CrestMap.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 06/03/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#ifndef CrestMap_hpp
#define CrestMap_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void genDLARidgeMap(GLuint bhm, int seed);
void ridgeMapReduce(GLuint ridgeMap1, GLuint ridgeMap2);

void ridgeMapUpscale(GLuint majorRidgeMap, GLuint minorRidgeMap, GLuint ridgeTexOneHD, GLuint ridgeTexTwoHD);
//void ridgeMapHDDLA(GLuint ridgeTexOneHD, GLuint ridgeTexTwoHD, GLuint ridgeGrowShaderNoDisk, float seedf);
void ridgeMapPyramidize(GLuint ridgeTexOneHD, GLuint ridgeTexOne16UI, GLuint ridgeTexTwo16UI, float seed);
void ridgeMapToBhm(GLuint bhm, GLuint ridgeTexOne16UI);

#endif /* CrestMap_hpp */
