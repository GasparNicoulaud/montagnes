//
//  GalaxyViewer.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 19/03/2017.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "GalaxyViewer.hpp"
#include "ShaderLoader.hpp"

struct starToSort
{
    float disToCam;
    int id;
};

int compare (const void * a, const void * b)
{
    return ( (*(starToSort*)b).disToCam - (*(starToSort*)a).disToCam );
}

void sortStarsEBO(Galaxy * gal, Camera * cam)
{
    starToSort * stars = new starToSort[gal->systemCount];
    
    for (int i = 0; i < gal->systemCount; i++) {
        stars[i].disToCam = distance(gal->systems[i].pos, cam->pos);
        stars[i].id = i;
    }
    
    qsort (stars, gal->systemCount, sizeof(starToSort), compare);
    
    for (int i = 0; i < gal->systemCount; i++) {
        gal->indices[i * 6] = stars[i].id * 4;
        gal->indices[i * 6 + 1] = stars[i].id * 4 + 1;
        gal->indices[i * 6 + 2] = stars[i].id * 4 + 2;
        
        gal->indices[i * 6 + 3] = stars[i].id * 4;
        gal->indices[i * 6 + 4] = stars[i].id * 4 + 3;
        gal->indices[i * 6 + 5] = stars[i].id * 4 + 1;
    }
}

void ViewGalaxy(Game * game)
{
    //sort stars for transparency
    sortStarsEBO(&game->galaxy,&game->mainCam);
    
    // Matrices projection et modelview
    mat4 proj;
    mat4 view;
    
    //begin normal render
    glClearColor(0,0,0,1);// black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    //only one pass :
    glBindVertexArray(game->galaxy.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game->galaxy.elementbuffer);
    glUseProgram(game->galaxy.stars_program);
    
    // update stars EBO
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    0,
                    game->galaxy.systemCount * 6 * sizeof(unsigned int),
                    game->galaxy.indices);
    
    glViewport(0, 0, game->windowWidth, game->windowHeight);
    proj = perspective(70.0, (double) game->windowWidth / game->windowHeight, 1.0, 25000.0);
    view = lookAt(game->mainCam.pos, game->mainCam.pos+game->mainCam.orientation, game->mainCam.up);
    
    glUniformMatrix4fv(glGetUniformLocation(game->galaxy.stars_program, "proj"), 1, GL_FALSE, value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(game->galaxy.stars_program, "view"), 1, GL_FALSE, value_ptr(view));

    glDrawElements(GL_TRIANGLES, game->galaxy.systemCount * 6, GL_UNSIGNED_INT, (void*)0);
 
    //finish normal render
    glUseProgram(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_OPERATION);
    assert(err == 0);

}

Galaxy LoadGalaxy(Game * game)
{
    Galaxy galaxy;
    // galaxy loading
    std::ifstream infile("galaxy.txt"); // load file
    printf("galaxy.txt opened\n");
    int syscount;
    infile >> syscount;
    galaxy.systemCount = syscount;
    
    galaxy.pos = vec3(0,0,0);
    
    galaxy.systems = new System[galaxy.systemCount];
    
    float x,y,z,r,g,b;
    
    for (int i = 0; i < galaxy.systemCount; i++) {
        
        infile >> x >> y >> z >> r >> g >> b;
        
        galaxy.systems[i].pos.x = x;
        galaxy.systems[i].pos.y = y;
        galaxy.systems[i].pos.z = z;
        
        galaxy.systems[i].color.r = r;
        galaxy.systems[i].color.g = g;
        galaxy.systems[i].color.b = b;
    }
    
    infile.close(); // close file
    printf("galaxy.txt closed\n");
    
    float * starsVertices = new float[syscount * 4 * 3]; // * 3 coordinate per vertex * 4 points per star (because a quad)
    float * starsColors = new float[syscount * 4 * 3]; // * 3 because rgb
    float * starsUV = new float[syscount * 4 * 2]; // * 2 because xy
    galaxy.indices = new unsigned int[syscount * 6]; // * 6 vertex per star (because a quad)
    
    float starQuadSize = 50;
    
    for (int i = 0; i < galaxy.systemCount; i++) {
        
        starsVertices[i * 12] = galaxy.systems[i].pos.x - 0.5 * starQuadSize;
        starsVertices[i * 12 + 1] = galaxy.systems[i].pos.y - 0.5 * starQuadSize;
        starsVertices[i * 12 + 2] = galaxy.systems[i].pos.z;
        starsUV[i * 8] = - 0.5 * starQuadSize;
        starsUV[i * 8 + 1] = - 0.5 * starQuadSize;
        
        starsVertices[i * 12 + 3] = galaxy.systems[i].pos.x + 0.5 * starQuadSize;
        starsVertices[i * 12 + 4] = galaxy.systems[i].pos.y + 0.5 * starQuadSize;
        starsVertices[i * 12 + 5] = galaxy.systems[i].pos.z;
        starsUV[i * 8 + 2] = + 0.5 * starQuadSize;
        starsUV[i * 8 + 3] = + 0.5 * starQuadSize;
        
        starsVertices[i * 12 + 6] = galaxy.systems[i].pos.x - 0.5 * starQuadSize;
        starsVertices[i * 12 + 7] = galaxy.systems[i].pos.y + 0.5 * starQuadSize;
        starsVertices[i * 12 + 8] = galaxy.systems[i].pos.z;
        starsUV[i * 8 + 4] = - 0.5 * starQuadSize;
        starsUV[i * 8 + 5] = + 0.5 * starQuadSize;
        
        starsVertices[i * 12 + 9] = galaxy.systems[i].pos.x + 0.5 * starQuadSize;
        starsVertices[i * 12 + 10] = galaxy.systems[i].pos.y - 0.5 * starQuadSize;
        starsVertices[i * 12 + 11] = galaxy.systems[i].pos.z;
        starsUV[i * 8 + 6] = + 0.5 * starQuadSize;
        starsUV[i * 8 + 7] = - 0.5 * starQuadSize;
        
        for(int j = 0; j < 4; j++)
        {
            starsColors[i * 12 + j * 3] = galaxy.systems[i].color.r;
            starsColors[i * 12 + j * 3 + 1] = galaxy.systems[i].color.g;
            starsColors[i * 12 + j * 3 + 2] = galaxy.systems[i].color.b;
        }
        
        galaxy.indices[i * 6] = i * 4;
        galaxy.indices[i * 6 + 1] = i * 4 + 1;
        galaxy.indices[i * 6 + 2] = i * 4 + 2;
            
        galaxy.indices[i * 6 + 3] = i * 4;
        galaxy.indices[i * 6 + 4] = i * 4 + 3;
        galaxy.indices[i * 6 + 5] = i * 4 + 1;
    }
    
    // shader buisness
    galaxy.stars_program = LoadAndLinkShader("stars.vert", "stars.frag");
    
    glBindAttribLocation(galaxy.stars_program, 0, "in_Vertex");
    glBindAttribLocation(galaxy.stars_program, 1, "in_Colors");
    glBindAttribLocation(galaxy.stars_program, 2, "in_UV");
    
    glLinkProgram(galaxy.stars_program);
    
    
    glGenVertexArrays(1, &galaxy.VAO);
    glBindVertexArray(galaxy.VAO);
    
    //here we prepare vao stuff
    glGenBuffers(1, &galaxy.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, galaxy.VBO);
    
    //remplissage du vbo
    glBufferData(GL_ARRAY_BUFFER, (syscount * 4 * 3 + syscount * 4 * 3 + syscount * 4 * 2) * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    syscount * 4 * 3 * sizeof(float),
                    starsVertices);
    glBufferSubData(GL_ARRAY_BUFFER,
                    syscount * 4 * 3 * sizeof(float),
                    syscount * 4 * 3 * sizeof(float),
                    starsColors);
    glBufferSubData(GL_ARRAY_BUFFER,
                    (syscount * 4 * 3 + syscount * 4 * 3) * sizeof(float),
                    syscount * 4 * 2 * sizeof(float),
                    starsUV);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(syscount * 4 * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((syscount * 4 * 3 + syscount * 4 * 3) * sizeof(float)));
    glEnableVertexAttribArray(2);
    // vbo done
    
    //now ebo
    glGenBuffers(1, &galaxy.elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, galaxy.elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, syscount * 6 * sizeof(unsigned int), galaxy.indices, GL_DYNAMIC_DRAW);
    
    // Déverrouillage du VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    //placing camera
    game->mainCam.pos = vec3(500,100,100);
    game->mainCam.up = vec3(0,0,1);
    game->mainCam.horangle = 0;
    game->mainCam.vertangle = 3;
    
    game->galaxyLoaded = true;
    
    printf("Galaxy Loaded\n");
    
    return galaxy;
}

int selectBody(float mouseX, float mouseY)
{
    //TODO
    return - 1;
}












