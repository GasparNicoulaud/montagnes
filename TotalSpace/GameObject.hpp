//
//  GameObject.h
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GameObject_h
#define GameObject_h

#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
//#include <GLFW/glfw3.h>
#include <OpenGL/glext.h>
#include "Setup.hpp"
#include "BattleMap.hpp"
#include "BattleMapMaker.hpp"
#include "Camera.hpp"
#include "Galaxy.hpp"
#include "GalaxyViewer.hpp"
#include "GalaxyGenerator.hpp"
#include "UIhandler.hpp"

struct Game {
    bool exitMainLoop;
    bool inBattle;
    bool galaxyLoaded;
    bool galaxyRequired;
    bool battleLoaded;
    bool battleRequired;
    BattleMap battleMap;
    Galaxy galaxy;
    
    Camera mainCam;
    
    //windown buisness
    GLFWwindow* window;
    int windowHeight;
    int windowWidth;
    
    double lastMX; //last mouse coordinates
    double lastMY;
    
    //framecounter;
    int frameCount;

};

#endif /* GameObject_h */
