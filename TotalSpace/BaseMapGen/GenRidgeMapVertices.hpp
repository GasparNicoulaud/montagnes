//
//  GenRidgeMapVertices.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 10/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GenRidgeMapVertices_hpp
#define GenRidgeMapVertices_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
//#include <math.h>

void genRigdeMapVertices(float *ridgeVertices, int maxRidgeVertexCount, int generationIterationCount, int ridgeSeedCount, int seed, float scale, float exclusionZoneRadius, float seedStartingHeight);

#endif /* GenRidgeMapVertices_hpp */
