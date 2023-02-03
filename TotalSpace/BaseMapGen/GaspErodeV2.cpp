//
//  GaspErodeV2.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 28/12/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "GaspErodeV2.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

void prepareTextures(GLuint* heightmap1, GLuint* waterPipe1, GLuint* sedimentPipe1, GLuint* heightmap2, GLuint* waterPipe2, GLuint* sedimentPipe2, GLuint bhm, int resolution)
{
    //allocate all textures
    glGenTextures(1, heightmap1);
    glBindTexture(GL_TEXTURE_2D, *heightmap1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, resolution, resolution, 0, GL_RG, GL_FLOAT, NULL);// R rocks, G sediments

    glGenTextures(1, heightmap2);
    glBindTexture(GL_TEXTURE_2D, *heightmap2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, resolution, resolution, 0, GL_RG, GL_FLOAT, NULL);// R rocks, G sediments
    
    glGenTextures(1, waterPipe1);
    glBindTexture(GL_TEXTURE_2D, *waterPipe1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution, resolution, 0, GL_RGBA, GL_HALF_FLOAT, NULL);

    glGenTextures(1, waterPipe2);
    glBindTexture(GL_TEXTURE_2D, *waterPipe2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution, resolution, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    
    glGenTextures(1, sedimentPipe1);
    glBindTexture(GL_TEXTURE_2D, *sedimentPipe1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution, resolution, 0, GL_RGBA, GL_HALF_FLOAT, NULL);

    glGenTextures(1, sedimentPipe2);
    glBindTexture(GL_TEXTURE_2D, *sedimentPipe2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution, resolution, 0, GL_RGBA, GL_HALF_FLOAT, NULL);

    /*
    glGenTextures(1, waterPipe1);
    glBindTexture(GL_TEXTURE_2D, *waterPipe1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);

    glGenTextures(1, waterPipe2);
    glBindTexture(GL_TEXTURE_2D, *waterPipe2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);
    
    glGenTextures(1, sedimentPipe1);
    glBindTexture(GL_TEXTURE_2D, *sedimentPipe1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);

    glGenTextures(1, sedimentPipe2);
    glBindTexture(GL_TEXTURE_2D, *sedimentPipe2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL);
    */
    
    //init necessary textures
    GLuint initShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/GaspErosionV2/initTextures.frag");
    glUseProgram(initShader);
    
    glBindTexture(GL_TEXTURE_2D, bhm);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *heightmap1, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *waterPipe1, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *sedimentPipe1, 0);
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    glDrawBuffers(3, attachments);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void erodeLoop(GLuint* heightmap1, GLuint* waterPipe1, GLuint* sedimentPipe1, GLuint* heightmap2, GLuint* waterPipe2, GLuint* sedimentPipe2, int iterations)
{
    GLuint program = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/GaspErosionV2/mainLoop.frag");
    glUseProgram(program);
    GLint hmLoc  = glGetUniformLocation(program, "heightRocksSediments");
    GLint watLoc = glGetUniformLocation(program, "waterPipes");
    GLint sedLoc = glGetUniformLocation(program, "sedimentPipes");
    GLint iterCountLoc = glGetUniformLocation(program, "iterCounter");
    glUniform1i(hmLoc, 0);
    glUniform1i(watLoc, 1);
    glUniform1i(sedLoc, 2);
    
    
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    for(int i = 0; i < iterations ; i++)
    {
        //ping
        glUniform1i(iterCountLoc,i * 2);
        glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
        glBindTexture(GL_TEXTURE_2D, *sedimentPipe1);
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, *waterPipe1);
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, *heightmap1);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *heightmap2, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *waterPipe2, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *sedimentPipe2, 0);
        
        glDrawBuffers(3,  attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        //pong
        glUniform1i(iterCountLoc,i * 2 + 1);
        glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
        glBindTexture(GL_TEXTURE_2D, *sedimentPipe2);
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, *waterPipe2);
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, *heightmap2);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *heightmap1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *waterPipe1, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *sedimentPipe1, 0);
        
        glDrawBuffers(3,  attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        if(i % 1000 == 0)
        {
            glFinish();
            printf("took a small break at iteration %i\n",i);
        }
    }
    
    
    attachments[1] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  attachments);
}

void putResultIntoBhm(GLuint* heightmap2, GLuint bhm)
{
    GLuint shader = LoadAndLinkShader("passThru.vert", "BaseMapGen/Erosion/GaspErosionV2/heightMapBackToBhm.frag");
    glUseProgram(shader);
    glBindTexture(GL_TEXTURE_2D, *heightmap2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bhm, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GaspErodeV2(GLuint bhm, int bhmRes, int iterations)
{
    GLuint heightmap1, heightmap2;
    GLuint waterPipe1,waterPipe2;
    GLuint sedimentPipe1, sedimentPipe2;
    
    prepareTextures(&heightmap1, &waterPipe1, &sedimentPipe1, &heightmap2, &waterPipe2, &sedimentPipe2, bhm, bhmRes);
    erodeLoop(&heightmap1, &waterPipe1, &sedimentPipe1, &heightmap2, &waterPipe2, &sedimentPipe2, iterations);
    putResultIntoBhm(&heightmap1,bhm);
}
