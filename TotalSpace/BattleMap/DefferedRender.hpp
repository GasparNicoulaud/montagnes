//
//  DefferedRender.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 22/11/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#ifndef DefferedRender_hpp
#define DefferedRender_hpp
#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include "GameObject.hpp"

void defferedRenderBranch(SquareBatch * batch, BattleMap * map, Camera cam);
void defferedDrawMap(Game * game);

#endif /* DefferedRender_hpp */
