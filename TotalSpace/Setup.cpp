//
//  Setup.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "GameObject.hpp"

static void error_callback(int error, const char* description)
{
    printf("!! GLFW error: %s\n", description);
}

void Setup(Game * game)
{
    //printf("Begin setup\n");
    bool GALAXY = false; //tha galxy/terrain swicth
    //GALAXY = true;
    game->exitMainLoop = false;
    game->inBattle = false;
    game->battleLoaded = false;
    game->battleRequired = !GALAXY;
    game->galaxyLoaded = false;
    game->galaxyRequired = GALAXY; // if we want galaxy maybe we should reenable aplha blending at the end of this function
    game->windowWidth = 3456;
    game->windowHeight = 2234;
    
    
    //glfw init
    glfwSetErrorCallback(error_callback);
    
    //glfwInitHint(0x00051001, GLFW_FALSE);
    
    if (!glfwInit()){
        printf("!! glfwInit fucks\n");
        exit(EXIT_FAILURE);
    }
    printf("glfwInit done, glfw version : %s\n",glfwGetVersionString());
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    //glfwWindowHint(GLFW_SAMPLES,4); //antialiasing, not really doing much on the retina screen anyway so maybe drop it ? of replace with FXAA ?
    glfwWindowHint(GLFW_DOUBLEBUFFER,GL_TRUE);
    //glfwWindowHint(GLFW_CENTER_CURSOR,GL_TRUE);
    
    //glfwWindowHint(G,GL_FALSE);
    
    game->window = glfwCreateWindow(game->windowWidth, game->windowHeight, "TotalSpace", glfwGetPrimaryMonitor(), NULL); //full screen
    //game->window = glfwCreateWindow(game->windowWidth, game->windowHeight, "TotalSpace", NULL, NULL); //windowed

    //glfwSetWindowOpacity(game->window, 0.5f);
    
    if (!game->window){
        glfwTerminate();
        printf("!! glfw window fucks\n");
        exit(EXIT_FAILURE);
    }
    
    //printf("Window sucessfully created\n");
    glfwSetWindowPos(game->window,0,0);
    
    glfwMakeContextCurrent(game->window);
    printf("OpengGL version : %s\n", glGetString(GL_VERSION));
    
    GLint n, i;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (i = 0; i < n; i++)
    {
        //printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
    }
               
    glfwSwapInterval(1); // should be set to 1 to limit fps to 60 and 0 to unlimit, its also the vsync
    glfwSetInputMode(game->window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    
    //opengl buisness
    glEnable(GL_DEPTH_TEST);
    
    if(GALAXY) //to get good transparency for stars
    {
        glDisable(GL_DEPTH_TEST);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    //glDisable(GL_BLEND); //maybe ?
}






