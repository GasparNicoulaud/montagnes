//
//  BattleMapMaker.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#ifndef BattleMapMaker_hpp
#define BattleMapMaker_hpp
#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "GameObject.hpp"
#include "Camera.hpp"

// Macro utile au VBO

#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

void initBattleMap(Game * game);

#endif /* BattleMapMaker_hpp */
