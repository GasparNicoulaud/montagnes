//
//  GalaxyGenerator.hpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 17/03/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#ifndef GalaxyGenerator_hpp
#define GalaxyGenerator_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "GameObject.hpp"

void GenerateGalaxy();
System GenerateSystem(int i);
Astre GenerateAstre(int i);
Location GenerateLocation(int i);
void SaveGalaxy(Galaxy galaxy);

#endif /* GalaxyGenerator_hpp */
