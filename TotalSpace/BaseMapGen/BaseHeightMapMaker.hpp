//
//  BaseHeightMapMaker.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 18/04/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#ifndef BaseHeightMapMaker_hpp
#define BaseHeightMapMaker_hpp

#include <stdio.h>
#include <list>
#include <vector>
#include <math.h>
#include "GameObject.hpp"

void genBHMGPU(int bmhRes, Recipe recipe, BattleMap *battleMap);
//void errodeBHM(GLuint bhm, int bhmRes);

#endif /* BaseHeightMapMaker_hpp */
