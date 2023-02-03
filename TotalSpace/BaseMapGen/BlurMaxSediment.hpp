//
//  BlurMaxSediment.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 25/05/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#ifndef BlurMaxSediment_hpp
#define BlurMaxSediment_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

void BlurMaxSediment(GLuint bhm, int bhmRes, int blurIter, float blurOffset);

#endif /* BlurMaxSediment_hpp */

