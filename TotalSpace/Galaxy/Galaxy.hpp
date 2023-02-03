//
//  Galaxy.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 18/04/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#ifndef Galaxy_hpp
#define Galaxy_hpp

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum AstreType{
    BH, //black hole
    STAR,
    PLANET
};

enum GravityType{
    NONE,
    LOW,
    NORMAL,
    HIGH
};

struct Location{
    glm::vec3 pos; // pos astre to system center
    GravityType gravityType;
};

struct Astre{
    glm::vec3 pos; // pos relative to system center
    AstreType astreType;
    GravityType gravityType;
    int locationCount;
    Location * locations;
};

struct System{
    glm::vec3 pos; // pos relative to galaxy center
    glm::vec3 color; //color of the star of the system
    int astreCount;
    Astre * astres;
};

struct Galaxy{
    glm::vec3 pos; // pos of the center of the galaxy, 0,0,0
    int systemCount;
    System * systems;
    
    //galaxy opengl buisness
    GLuint VAO;
    GLuint VBO;
    
    GLuint stars_program;
    GLuint elementbuffer;
    
    unsigned int * indices;
};

#endif /* Galaxy_hpp */
