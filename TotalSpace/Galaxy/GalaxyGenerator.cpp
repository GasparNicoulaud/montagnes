//
//  GalaxyGenerator.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 17/03/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#include "GalaxyGenerator.hpp"

#define STAR_COUNT 5000

void GenerateGalaxy()
{
    Galaxy galaxy;
    galaxy.pos = vec3(0,0,0);
    galaxy.systemCount = STAR_COUNT;
    galaxy.systems = new System[galaxy.systemCount];
    
    for (int i = 0; i < galaxy.systemCount; i++) {
        galaxy.systems[i] = GenerateSystem(i);
    }
    
    SaveGalaxy(galaxy);
}

float frand(float max)
{
    return ((float)(rand()%1000) / 1000.0) * max;
}

float starZdistrib(int i)
{
    float baseZ = frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0)+frand(10.0);
    baseZ -= 50.0;
    
    float retval = baseZ * (1 - (float)i/STAR_COUNT) * 20;
    
    if(i<STAR_COUNT/50)
    {
        retval *= 0.05 + (i/((float)STAR_COUNT/50.0)) * 0.95;
    }
    
    return retval;
}

System GenerateSystem(int i)
{
    System system;
    
    float distanceBetweenRingsFactor = 0.4 + ((float)max(i - (int)(STAR_COUNT * 0.9),0))/((float)STAR_COUNT) * 0.2;
    distanceBetweenRingsFactor *= 10000.0/(float)STAR_COUNT;
    
    float blackholeoffset = 0.0;
    if(i>0)
    {
        blackholeoffset = 5.0;
    }
        
    
    float theta = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/3.1416f)) * 2;
    system.pos.x = (i + blackholeoffset) * cos(theta) * distanceBetweenRingsFactor;
    system.pos.y = (i + blackholeoffset) * sin(theta) * distanceBetweenRingsFactor;
    system.pos.z = starZdistrib(i);
    if(i==0) system.pos.z = 0;
    
    system.color.r = 0.7 + frand(0.4);
    //system.color.g = 0.8 + frand(0.2);
    system.color.b = 0.6 + frand(0.4);
    system.color.g = min(system.color.b,system.color.r) + frand(abs(system.color.b-system.color.r) * 0.3);
    
    if(i==0)
    {
        system.color.r = 0.0;
        system.color.g = 0.0;
        system.color.b = 0.0;
    }
    
    system.astreCount = (rand()%10) + 1; //between 1 and 10 astres per system
    system.astres = new Astre[system.astreCount];
    
    for (int i = 0; i < system.astreCount; i++) {
        system.astres[i] = GenerateAstre(i);
    }
    
    return system;
}

Astre GenerateAstre(int i)
{
    Astre astre;
    astre.locationCount = (rand()%10) + 1; //between 1 and 10 location per astre
    astre.locations = new Location[astre.locationCount];
    
    for (int i = 0; i < astre.locationCount; i++) {
        astre.locations[i] = GenerateLocation(i);
    }
    
    return astre;
}

Location GenerateLocation(int i)
{
    Location location; //location, location...
    
    return location;
}

void SaveGalaxy(Galaxy galaxy)
{
    std::ofstream outfile ("galaxy.txt");
    
    outfile << galaxy.systemCount << std::endl;
    for (int i = 0; i < galaxy.systemCount; i++) {
        outfile << galaxy.systems[i].pos.x << " " << galaxy.systems[i].pos.y << " " << galaxy.systems[i].pos.z << " ";
        outfile << galaxy.systems[i].color.r << " " << galaxy.systems[i].color.g << " " << galaxy.systems[i].color.b << std::endl;
    }
    
    outfile.close();
    printf("Saved new galaxy.txt\n");
}



