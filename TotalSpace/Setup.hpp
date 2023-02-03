//
//  Setup.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#ifndef Setup_hpp
#define Setup_hpp

#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

struct Game;

void Setup(Game * game);

#endif /* Setup_hpp */
