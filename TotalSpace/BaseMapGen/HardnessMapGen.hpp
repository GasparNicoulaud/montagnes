//
//  HardnessMapGen.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 29/05/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#ifndef HardnessMapGen_hpp
#define HardnessMapGen_hpp

#include <stdio.h>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

GLuint genHardnessMap(GLuint bhm, int resolution, int seed, bool heavyTerracing);

#endif /* HardnessMapGen_hpp */
