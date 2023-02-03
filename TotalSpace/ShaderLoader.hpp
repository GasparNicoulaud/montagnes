//
//  ShaderLoader.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 11/02/18.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#ifndef ShaderLoader_hpp
#define ShaderLoader_hpp

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>

GLuint LoadAndLinkShader(const char *vert_path,const char *frag_path);


#endif /* ShaderLoader_hpp */
