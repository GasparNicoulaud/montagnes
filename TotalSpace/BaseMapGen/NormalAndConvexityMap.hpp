//
//  NormalAndConvexityMap.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 13/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#ifndef NormalAndConvexityMap_hpp
#define NormalAndConvexityMap_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void genNormAndConvMap(GLuint bhm, GLuint normalConvexityMap, int bhmSize);
void genBlurredNormAndConvMap(GLuint bhm, GLuint normalConvexityMap, int bhmSize);

#endif /* NormalAndConvexityMap_hpp */
