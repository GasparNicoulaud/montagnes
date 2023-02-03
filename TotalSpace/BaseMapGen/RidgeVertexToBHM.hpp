//
//  RidgeVertexToBHM.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 11/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#ifndef RidgeVertexToBHM_hpp
#define RidgeVertexToBHM_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void ridgeVerticesIntoBHM(float *ridgeVertices, int RidgeVertexCount, GLuint bhm, int bhmSize);

void pyramidize(GLuint bhm, int bhmSize, float slopeFactor);

#endif /* RidgeVertexToBHM_hpp */
