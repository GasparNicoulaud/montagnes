//
//  GaspErodeV2.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 28/12/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GaspErodeV2_hpp
#define GaspErodeV2_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void GaspErodeV2(GLuint bhm, int bhmRes, int iterations);

#endif /* GaspErodeV2_hpp */
