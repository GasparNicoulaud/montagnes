//
//  BoeschThermalErosion.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 29/05/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#ifndef BoeschThermalErosion_hpp
#define BoeschThermalErosion_hpp

#include <stdio.h>
//#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void boeschErode(GLuint bhm, int bhmRes, GLuint hardnessMap, int iterations);

#endif /* BoeschThermalErosion_hpp */
