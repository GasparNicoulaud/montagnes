//
//  UIhandler.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 17/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

//#include <curses.h>
//#include <AppKit/AppKit.h>
//#include <iostream>
# include <cstdlib>
#include "UIhandler.hpp"

int nbFrames = 0;
float lastTime = 0;

void setWindowFPS (GLFWwindow* win)
{
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    
    if ( currentTime - lastTime >= 1.0 ){ // If last cout was more than 1 sec ago
        char title [256];
        title [255] = '\0';
        
        snprintf(title, 255, "TotalSpace - [FPS: %i , Average Frame Time : %2.3f ms]", nbFrames, (currentTime - lastTime)/nbFrames*1000);
        //wprintf("TotalSpace - [FPS: %i , Average Frame Time : %2.3f ms]\n", nbFrames, (currentTime - lastTime)/nbFrames*1000);
        
        glfwSetWindowTitle (win, title);
        
        nbFrames = 0;
        lastTime = currentTime;
    }
}

void updateUI (Game * game)
{
    if (glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if(game->galaxyLoaded)
        {
            //were in galaxy screen so lets see if we intersect a body
            double xpos, ypos;
            glfwGetCursorPos(game->window, &xpos, &ypos);
            selectBody(xpos, ypos);
        }
    }
    
}

void checkForScreenshotRequest(Game * game)
{
    if (glfwGetKey(game->window, GLFW_KEY_SPACE ) == GLFW_PRESS){ //faire un creenshot en hhhddd
        std::system("say screenshot");
        printf("screenshot requested\n");
        game->mainCam.screenshotRequested = true;
    }

}

void checkForVideoRequest(Game * game)
{
    if (glfwGetKey(game->window, GLFW_KEY_V ) == GLFW_PRESS){ //lancer un enregistrement video
        std::system("say video start");
        printf("video begin\n");
        game->mainCam.videoFramesRequiredTotal = 60 * 30; //30 seconds at 60fps
        game->mainCam.videoFramesRequiredLeft = game->mainCam.videoFramesRequiredTotal;
    }

}
