//
//  UIhandler.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 17/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#ifndef UIhandler_hpp
#define UIhandler_hpp

#include <stdio.h>
#include "GameObject.hpp"

void setWindowFPS (GLFWwindow* win);
void updateUI (Game * game);
void checkForScreenshotRequest(Game * game);
void checkForVideoRequest(Game * game);


#endif /* UIhandler_hpp */
