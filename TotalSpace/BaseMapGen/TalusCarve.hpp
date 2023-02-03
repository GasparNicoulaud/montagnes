//
//  TalusCarve.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 23/10/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#ifndef TalusCarve_hpp
#define TalusCarve_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void talusCarve(GLuint bhm, int bhmSize, float slopeFactor);

#endif /* TalusCarve_hpp */
