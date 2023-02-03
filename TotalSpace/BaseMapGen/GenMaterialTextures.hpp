//
//  GenMaterialTextures.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 19/02/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GenMaterialTextures_hpp
#define GenMaterialTextures_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
#include "GameObject.hpp"

void genMaterialTexture(GLuint* albedoHeightTexture, GLuint* roughnessAOTexture, MaterialRecipe materialRecipe, int textSize);

#endif /* GenMaterialTextures_hpp */
