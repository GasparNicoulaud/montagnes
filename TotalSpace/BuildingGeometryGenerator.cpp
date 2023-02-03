//
//  BuildingGeometryGenerator.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 9/13/17.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "BuildingGeometryGenerator.hpp"
#include "ShaderLoader.hpp"

// fills the buildings vao
void genBuildingGeometry(Game * game)
{
    //building shader
    game->battleMap.buildings.program = LoadAndLinkShader("building.vert", "building.frag");
    glUseProgram(game->battleMap.buildings.program);
    game->battleMap.buildings.zOffsetLoc = glGetUniformLocation(game->battleMap.buildings.program, "zOffset");
    game->battleMap.buildings.MVPLoc = glGetUniformLocation(game->battleMap.buildings.program, "MVP");
    
    //actual geometry generation
    //float vertices[36 * 3];
    
    static const GLfloat vertices[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    
    //then finally we fill the vao
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    //remplissage du vbo
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float) * 3, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 36 * sizeof(float) * 3, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //vbo done
    //now we do the vao
    glGenVertexArrays(1, &game->battleMap.buildings.vao);
    glBindVertexArray(game->battleMap.buildings.vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Déverrouillage du VAO
    glBindVertexArray(0);

}
