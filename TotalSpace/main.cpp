//
//  main.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>       /* time */
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is better than 2
#include <GLFW/glfw3.h>
#include <FreeImagePlus.h>
#include <thread>         // std::this_thread::sleep_until
#include "GameObject.hpp"
#include "BuildingGeometryGenerator.hpp"
#include "RecipeGenerator.hpp"
#include "updateMapVisRep.hpp"
#include "drawMap.hpp"
#include "DefferedRender.hpp"

int main(int argc, const char * argv[]) {
    
    bool REPLAY_PREVIOUS_MAP = false;
    //REPLAY_PREVIOUS_MAP = true;
    
    //exit(0);
    
    srand(0); // first things first, init the random generator
    
    //setup everything
    Game * game = new Game;
    
    if(game->galaxyRequired)
    {
        GenerateGalaxy(); //gen the galaxy file
    }
    
    Setup(game);
    FreeImage_Initialise();
    
    game->frameCount = 0;
    
    //main loop
    //printf("Entering main loop\n");
    //auto start = std::chrono::steady_clock::now();
    while(!game->exitMainLoop)
    {
        
        auto now = std::chrono::steady_clock::now();
        //auto diff = now - start;
        auto end = now + std::chrono::milliseconds(16);
        
        if(game->galaxyRequired) // galaxy mode
        {
            if(!game->galaxyLoaded)
            {
                game->galaxy = LoadGalaxy(game);
            }
            else
            {
                updateCam(game);
                ViewGalaxy(game);
                glfwSwapBuffers(game->window);
                glfwPollEvents();
            }
        }
        
        //draw, update, etc...
        if(game->battleRequired) //battle mode
        {
            if(!REPLAY_PREVIOUS_MAP)
            {
                saveRecipe(genRecipe());
            }
            loapRecipe(game);
            
            //load battle
            // Measure time it takes to gen the map
            double currentTime = glfwGetTime();
            
            initBattleMap(game); // new code

            printf("time of total map generation : %.4fs\n",glfwGetTime() - currentTime);
            
            //gen building geometry
            genBuildingGeometry(game);
            
            //init the mouse pos so that when camera controls kicks in, it doesnt begin by making a big move from some garbage lastPos
            double xpos, ypos;
            glfwGetCursorPos(game->window, &xpos, &ypos);
            game->lastMX = xpos;
            game->lastMY = ypos;
        }
        if(game->battleLoaded)
        {
            game->inBattle = true;

        }
        if(game->inBattle)
        {
            game->battleMap.time += 1.0/60.0;
            
            // screenshotbuisness
            checkForScreenshotRequest(game);
            
            // video buisness
            checkForVideoRequest(game);
            
            //updating camera data, this also checks for input to update the camera
            if(game->mainCam.videoFramesRequiredLeft == 0) //unless we are recordind a video, then dont move the camera even if there is input
            {
                updateCam(game);
            }
            
            if(/*game->frameCount % 10 == 0*/!game->mainCam.movingFast) //update only every 10 frames
            {
                updateMapVisRep(game);
            }
            //drawMap(game);
            defferedDrawMap(game);
            
            std::this_thread::sleep_until(end);
            glfwSwapBuffers(game->window);
            
            glfwPollEvents();
        }
        
        updateUI(game);
        setWindowFPS(game->window); //put the fps in the window title
        
        //glfwSetWindowPos(game->window,1,0); //weird workaround mojave black screen bug
        
        if(glfwWindowShouldClose(game->window)){ //check event like cmd+q or window close
            game->exitMainLoop = true;
        }
        
        if (game->mainCam.screenshotRequested) {
            
             FIBITMAP* image = FreeImage_ConvertFromRawBits(game->battleMap.screenshotPixels
                                                 , (int)(game->mainCam.screenshotWidth)
                                                 , (int)(game->mainCam.screenshotHeight)
                                                 , (int)(3 * game->mainCam.screenshotWidth)
                                                 , (unsigned)24
                                                 , (unsigned)0x0000FF, (unsigned)0xFF0000, (unsigned)0x00FF00, false);
             
            
            const char base_string[] = "/Users/gaspar/Prog/TotalSpace/HDscreenshots/screenshot";
            const char extension[] = ".png";
            char out_string [100];
            //int number = game->frameCount;
            unsigned long int timesteamp = time(NULL);
            sprintf(out_string, "%s%li%s", base_string, timesteamp, extension);
            
            FreeImage_Save(FIF_PNG, image, out_string, 0);
            
            // Free resources
            FreeImage_Unload(image);
            
            game->mainCam.screenshotRequested = false;
            std::system("say ok");
            printf("screenshot done\n");
        }
        
        if (game->mainCam.videoFramesRequiredLeft > 0) {
                
             FIBITMAP* image = FreeImage_ConvertFromRawBits(game->battleMap.videoPixels
                                                 , (int)(game->mainCam.videoWidth)
                                                 , (int)(game->mainCam.videoHeight)
                                                 , (int)(3 * game->mainCam.videoWidth)
                                                 , (unsigned)24
                                                 , (unsigned)0x0000FF, (unsigned)0xFF0000, (unsigned)0x00FF00, false);
             
            
            const char base_string[] = "/Users/gaspar/Prog/TotalSpace/videoframes/passage9/frame";
            const char extension[] = ".png";
            char filePath [100];
            int frameNumber = game->mainCam.videoFramesRequiredTotal - game->mainCam.videoFramesRequiredLeft;
            char frameNumberWithLeadingZeroes[6];
            snprintf (frameNumberWithLeadingZeroes, 6, "%05i", frameNumber);
            sprintf(filePath, "%s%s%s", base_string, frameNumberWithLeadingZeroes, extension);
            
            FreeImage_Save(FIF_PNG, image, filePath, 0);
            
            // Free resources
            FreeImage_Unload(image);
            
            game->mainCam.videoFramesRequiredLeft --;
            
            if(game->mainCam.videoFramesRequiredLeft == 0)
            {
                std::system("say video recording done");
                printf("video done\n");
            }
        }
    
        game->frameCount ++;
    }
    
    
    
    //delete everything
    printf("Destroying the window\n");
    glfwDestroyWindow(game->window);
    glfwTerminate();
    FreeImage_DeInitialise();
    
    return 0;
}
