//
//  Camera.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 16/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#include "Camera.hpp"


void initCam(Game * game)
{
    //video buisness
    game->mainCam.videoWidth = 720; // must be under 16384
    game->mainCam.videoHeight = 480; // must be under 16384
    game->mainCam.videoFramesRequiredLeft = 0;
    game->mainCam.videoFramesRequiredTotal = 0;
    
    //screenshot buisness
    game->mainCam.screenshotWidth = 3456 * 4; // must be under 16384, so multiplier at 5 Max
    game->mainCam.screenshotHeight = 2234 * 4; // must be under 16384
    game->mainCam.screenshotRequested = false;
    
    //placing camera
    /*game->mainCam.pos = vec3(0,-1000,1100);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 1.57;
    game->mainCam.vertangle = 2.1415;*/
    
    // from corner
    game->mainCam.pos = vec3(-6543,-6833,3343);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 0.801328;
    game->mainCam.vertangle = 2.0939;
    
    // from center looking down
    /*game->mainCam.pos = vec3(-1500,0,12000);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 0.0;
    game->mainCam.vertangle = 3.14;*/
    
    // from corner but closer
    /*game->mainCam.pos = vec3(-4543,-4833,3343);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 0.801328;
    game->mainCam.vertangle = 2.0939;*/
    
    // focus on a detail
    /*game->mainCam.pos = vec3(-2000,-2833,2343);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 1.001328;
    game->mainCam.vertangle = 2.0939;*/
    
    // focus on center
    /*game->mainCam.pos = vec3(-500,-533,1543);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 1.001328;
    game->mainCam.vertangle = 2.4939;*/
    
    game->mainCam.FOV = 0.9;
}

void updateCam(Game * game)
{
    game->mainCam.fixedMode = false;
    game->mainCam.movingFast = false; //prevents loading of higher definition terrain when pressing maj
    
    bool DEMOMODE = true;
    DEMOMODE = false;
    
    float moveSpeed = 0.6; //in meters per frame //default 2
    moveSpeed = 1.0;
    if(game->galaxyLoaded)
    {
        moveSpeed = 0.2;
    }
    float mouseSensitivity = 0.005;
    
    if (glfwGetKey(game->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){ //maj pour accelerer
        moveSpeed *= 30;
        game->mainCam.movingFast = true; //prevents loading of higher definition terrain when pressing maj
    }
    
    if(game->mainCam.fixedMode) //rts mode
    {
        //orientation buisness
        game->mainCam.horangle = 0;
        game->mainCam.vertangle = 3 - (1 - (game->mainCam.pos.z - 50) * 0.0006896551724) * 0.6;
        
        //position buisness
        if (glfwGetKey(game->window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS){ //Z
            game->mainCam.pos.x += moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS){ //S
            game->mainCam.pos.x -= moveSpeed;
        }
        
        //position buisness
        if (glfwGetKey(game->window, GLFW_KEY_Z) == GLFW_PRESS){ //W
            game->mainCam.pos.z += moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_X) == GLFW_PRESS){ //X
            game->mainCam.pos.z -= moveSpeed;
        }
        
        game->mainCam.pos.z = clamp((double)game->mainCam.pos.z,50.0,1500.0);
        
        game->mainCam.orientation = vec3(cos(game->mainCam.horangle)*sin(game->mainCam.vertangle),sin(game->mainCam.horangle)*sin(game->mainCam.vertangle),cos(game->mainCam.vertangle));
        
        
        
        if (glfwGetKey(game->window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS){ //D
            game->mainCam.pos += normalize(cross(game->mainCam.orientation,game->mainCam.up))  * moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS){ //Q
            game->mainCam.pos -= normalize(cross(game->mainCam.orientation,game->mainCam.up))  * moveSpeed;
        }
    }
    else if(DEMOMODE) //going in circle mode
    {
        //game->mainCam.pos = vec3(0,-1000,1100);
        game->mainCam.up = vec3(0,0,1);
        game->mainCam.horangle = 0.0;
        game->mainCam.vertangle = 2.1415;
        
        float ringRadius = 5000;
        float rotationspeed = 0.0001;
        
        game->mainCam.pos.z = 200;

        game->mainCam.pos.x = sin(game->frameCount * rotationspeed) * ringRadius;
        game->mainCam.pos.y = cos(game->frameCount * rotationspeed) * ringRadius;
        
        game->mainCam.orientation = -game->mainCam.pos + vec3(0,0,-2000);
    }
    else //normal control mode
    {
        
        //orientation buisness
        double xpos, ypos;
        glfwGetCursorPos(game->window, &xpos, &ypos);
        
        game->mainCam.horangle -= (xpos-game->lastMX) * mouseSensitivity;
        game->mainCam.vertangle += (ypos-game->lastMY) * mouseSensitivity;
        float PI = 3.14159265359f;
        game->mainCam.vertangle = clamp((double)game->mainCam.vertangle,0.1,PI - 0.1); // bound between 0.1 and pi - 0.1
        
        
        game->lastMX = xpos;
        game->lastMY = ypos;
        
        //position buisness
        
        if (glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS){ //Z
            game->mainCam.pos += game->mainCam.orientation  * moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS){ //S
            game->mainCam.pos -= game->mainCam.orientation  * moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_UP) == GLFW_PRESS){ //UP
            game->mainCam.pos += game->mainCam.orientation  * moveSpeed * 0.1f;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_DOWN) == GLFW_PRESS){ //DOWN
            game->mainCam.pos -= game->mainCam.orientation  * moveSpeed * 0.1f;
        }
        
        /*if (glfwGetKey(game->window, GLFW_KEY_Q) == GLFW_PRESS){ //S
            printf("x:%f y:%f z:%f\n",game->mainCam.pos.x,game->mainCam.pos.y,game->mainCam.pos.z);
            //game->mainCam.pos = vec3(-8000,-8000,8000);
            //game->mainCam.up = vec3(0,0,1);
            //game->mainCam.horangle = 0.7;
            //game->mainCam.horangle = 2.1415;
            printf("horangle:%f vertangle:%f\n",game->mainCam.horangle,game->mainCam.vertangle);

        }*/
        
        // jitter mode, maybe for taa someday
        /*float jitterV = float(rand()%100) * 0.00005;
        float jitterH = float(rand()%100) * 0.00005;
        game->mainCam.orientation = vec3(cos(game->mainCam.horangle + jitterH)*sin(game->mainCam.vertangle ),sin(game->mainCam.horangle + jitterH)*sin(game->mainCam.vertangle + jitterV),cos(game->mainCam.vertangle + jitterV));*/
        
        game->mainCam.orientation = vec3(cos(game->mainCam.horangle)*sin(game->mainCam.vertangle ),sin(game->mainCam.horangle)*sin(game->mainCam.vertangle),cos(game->mainCam.vertangle));
        
        
        
        if (glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS){ //D
            game->mainCam.pos += normalize(cross(game->mainCam.orientation,game->mainCam.up))  * moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS){ //Q
            game->mainCam.pos -= normalize(cross(game->mainCam.orientation,game->mainCam.up))  * moveSpeed;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_RIGHT) == GLFW_PRESS){ //RIGHT
            game->mainCam.pos += normalize(cross(game->mainCam.orientation,game->mainCam.up))  * moveSpeed * 0.1f;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_LEFT) == GLFW_PRESS){ //LEFT
            game->mainCam.pos -= normalize(cross(game->mainCam.orientation,game->mainCam.up))  * moveSpeed * 0.1f;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_O) == GLFW_PRESS){ //D
            game->mainCam.FOV += 0.01;
        }
        
        if (glfwGetKey(game->window, GLFW_KEY_P) == GLFW_PRESS){ //Q
            game->mainCam.FOV -= 0.01;
        }
        
        game->mainCam.FOV = clamp(game->mainCam.FOV, 0.1f, 1.9f);
    }
    
    
}
