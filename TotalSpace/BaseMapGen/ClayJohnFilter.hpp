//
//  ClayJohnFilter.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 13/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#ifndef ClayJohnFilter_hpp
#define ClayJohnFilter_hpp

#include <stdio.h>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void clayJohnFilter(GLuint bhm, GLuint normalConvexityMap, int bhmSize);

#endif /* ClayJohnFilter_hpp */
