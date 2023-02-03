//
//  GalaxyViewer.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 19/03/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GalaxyViewer_hpp
#define GalaxyViewer_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.hpp"

void ViewGalaxy(Game * game);
Galaxy LoadGalaxy(Game * game);
void sortStarsEBO(Galaxy * gal, Camera * cam);
int selectBody(float mouseX, float mouseY);

#endif /* GalaxyViewer_hpp */
