//
//  WMErodeSpecial.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 30/10/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#ifndef WMErodeSpecial_hpp
#define WMErodeSpecial_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GLuint WMErodeSpecial(GLuint bhm, float sedimentation, int iterations);

#endif /* WMErodeSpecial_hpp */
