//
//  ComputeAOintoLightMap.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 23/01/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#ifndef ComputeAOintoLightMap_hpp
#define ComputeAOintoLightMap_hpp

#include <stdio.h>
#include <list>
#include <vector>
#include <math.h>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
#include "GameObject.hpp"

void computeAOintoLightMap(GLuint geometryMap, GLuint lightMap);

#endif /* ComputeAOintoLightMap_hpp */
