//
//  RecipeGenerator.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 9/13/17.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#ifndef RecipeGenerator_hpp
#define RecipeGenerator_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "BattleMap.hpp"
#include "GameObject.hpp"

using namespace std;

Recipe genRecipe();
void saveRecipe(Recipe r);
void loapRecipe(Game * game);

#endif /* RecipeGenerator_hpp */
