//
//  JakoErosion.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 12/02/18.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#ifndef JakoErosion_hpp
#define JakoErosion_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void jakoErode(GLuint bhm, int bhmRes, int iterations);
void jakoErodeV2(GLuint bhm, int bhmRes, int iterations, int seed, float jako_rain_rate);

#endif /* JakoErosion_hpp */
