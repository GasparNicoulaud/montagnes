//
//  Camera.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 16/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

struct Camera{
    
    double vertangle, horangle;
    
    vec3 pos;
    vec3 orientation;
    vec3 up;
    
    bool fixedMode; // if true, behaves like in classic rts
    
    bool screenshotRequested;
    int screenshotWidth, screenshotHeight;
    
    int videoFramesRequiredLeft;
    int videoFramesRequiredTotal;
    int videoWidth, videoHeight;
    
    float FOV;
    
    bool movingFast;
};


#endif /* Camera_hpp */
#include "GameObject.hpp"

void initCam(Game * game);
void updateCam(Game * game);

