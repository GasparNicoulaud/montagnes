//
//  CPUParticleErode.hpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 27/12/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#ifndef CPUParticleErode_hpp
#define CPUParticleErode_hpp

#include <stdio.h>
#include <vector>
#define GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLCOREARB //to make glfw use opengle 3 and not 2 because 3 is etter than 2
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GLuint CPUParticleErode(GLuint bhm, int resolution, int iterations, float talusAngle, float erosionStrength);
GLuint CPUParticleErode128Steps(GLuint bhm, int resolution, double sedimentation);
GLuint CPUParticleMicroErode(GLuint bhm, int resolution);
GLuint CPUParticleErodeOnePass(GLuint bhm, int resolution, int iterations, float talusAngle, float erosionStrength);
float* PutBhmIntoArray(GLuint bhm, int resolution);
float* particleErode(float* heightMap, int resolution, int iterations);
GLuint PutErrodedMapIntoBhm(float* erodedMap, int resolution);
glm::vec3 surfaceNormal(float* heightMap, int i, int j, int resolution);
glm::vec2 valueAt(float* heightMap, int i, int j, int resolution);
void addValueAt(float* heightMap, int i, int j, int resolution, float value);



#endif /* CPUParticleErode_hpp */
