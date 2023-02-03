//
//  DefferedRender.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 22/11/2022.
//  Copyright © 2022 Gaspar Nicoulaud. All rights reserved.
//

#include "DefferedRender.hpp"
#define GL_SILENCE_DEPRECATION

void defferedRenderBranch(SquareBatch * batch, BattleMap * map, Camera cam)
{
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    glUniform2f(map->fillGbufferoffsetLoc, batch->offset.x, batch->offset.y);
    glUniform1f(map->fillGbufferscaleLoc,batch->scale);
    //glUniform1f(glGetUniformLocation(map->terrain_program, "time"),map->time);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);

    glUniform1i(map->fillGbufferShaderLocations.heightHorizontalDisplacementAOLoc, 0);
    glUniform1i(map->fillGbufferShaderLocations.albedoRoughnessLoc, 1);
    glUniform1i(map->fillGbufferShaderLocations.normalMapLoc, 2);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n", err);
    assert(err == 0);
    
    glEnable(GL_FRAMEBUFFER_SRGB);
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, batch->heightHorizontalDisplacementAO);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, batch->albedoRoughness);
    glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
    glBindTexture(GL_TEXTURE_2D, batch->normalMap);
//    glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
//    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.lightMap);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n", err);
    assert(err == 0);
    ///*
    
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    glDrawElements(
                   GL_TRIANGLES,      // mode GL_TRIANGLES   GL_LINE_STRIP    GL_LINES
                   map->gridres*map->gridres*6,    // count
                   GL_UNSIGNED_INT,   // type
                   (void*)0           // element array buffer offset
                   );
     //*/
    
    
//    glActiveTexture(GL_TEXTURE0 + 3); //maybe keep texture +3 bound as it is the global lightmap and doesnt change from batch to batch ?
//    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_FRAMEBUFFER_SRGB);
}

void defferedRenderFromGBuffer(Game * game){
    
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(game->battleMap.vaoIDHD);
    glUseProgram(game->battleMap.drawGbufferProgram);
    glUniform1i(game->battleMap.drawGbufferPositionLoc, 0);
    glUniform1i(game->battleMap.drawGbufferAlbedoRoughnesLoc, 1);
    glUniform1i(game->battleMap.drawGbufferNormalSamplerLoc, 2);
    glUniform1i(game->battleMap.drawGbufferGlobalLightMapLoc, 3);
    
    glUniform3fv(game->battleMap.drawGbufferCampos, 1, value_ptr(game->mainCam.pos));
    vec3 sunlightDirection = vec3(game->battleMap.recipe.sunPosX,game->battleMap.recipe.sunPosY,game->battleMap.recipe.sunPosZ);
    glUniform3fv(game->battleMap.drawGbufferSunlightDirection, 1, value_ptr(sunlightDirection));
    
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, game->battleMap.gBufferPosition);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, game->battleMap.gBufferAlbedoRoughness);
    glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
    glBindTexture(GL_TEXTURE_2D, game->battleMap.gBufferNormal);
    glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
    glBindTexture(GL_TEXTURE_2D, game->battleMap.staticMapTextures.lightMap);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    err = glGetError();
    if(err != 0){
        printf("Error = %i\n", err);
        printf("Note : 1282 is 'Invalid Operation'");
    }
    assert(err == 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisable(GL_FRAMEBUFFER_SRGB);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
}

void defferedDrawMap(Game * game)
{
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    // Matrices projection et modelview
    mat4 projection;
    mat4 modelView;
    mat4 modProj;
    
    //prepare view matrices
    glViewport(0, 0, game->windowWidth, game->windowHeight);
    double fieldOfView = game->mainCam.FOV;
    projection = perspective(fieldOfView, (double) game->windowWidth / game->windowHeight, 1.0, 25000.0);
    modelView = lookAt(game->mainCam.pos, game->mainCam.pos+game->mainCam.orientation, game->mainCam.up);
    modProj = projection * modelView;
    
    glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.gBufferFbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    err = glGetError();
    if(err != 0){
        printf("Error = %i\n", err);
        printf("Note : 1282 is 'Invalid Operation'");
    }
    assert(err == 0);
    
    glBindVertexArray(game->battleMap.vaoID);
    glUseProgram(game->battleMap.fillGbuffer);
    glUniformMatrix4fv(game->battleMap.fillGbuffermodprojLoc, 1, GL_FALSE, value_ptr(modProj));

    int count = 0;
    
    // actual terrain redenring loop
    for (int i = 0 ; i < game->battleMap.bmc; i++) { //render all batches on the display list
        if(game->battleMap.activeBatch[i]){//if the batch is active, render it
            defferedRenderBranch(game->battleMap.listOfSquareBatchesToDisplay[i], &game->battleMap, game->mainCam);
            count++;
        }
    }
    
    //glBindVertexArray(0);
    
    //building render
    
//    modProj = projection * modelView * glm::mat4(1.0f);
//
//    glBindVertexArray(game->battleMap.buildings.vao);
//    glUseProgram(game->battleMap.buildings.program); //buildings shaders
//    //game->battleMap.recipe.heightAtMiddle = 150;
//    //printf("building ofst from O meters : %f\n",game->battleMap.recipe.heightAtMiddle);
//    glUniform1f(game->battleMap.buildings.zOffsetLoc, game->battleMap.recipe.heightAtMiddle);
//    glUniformMatrix4fv(game->battleMap.buildings.MVPLoc, 1, GL_FALSE, value_ptr(modProj));
//
//    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices in a cube
//

    
    //draw the skybox
    glDepthFunc(GL_LEQUAL); // make opengl draw the sky even when less than or equal to zFar (it's always equal for the skybox which is only a skyQuad
    glBindVertexArray(game->battleMap.skyboxVao); //just a quad
    glUseProgram(game->battleMap.gBufferSkybox_program);

    glUniformMatrix4fv(game->battleMap.gBufferSkybox_programInversemodprojLoc, 1, GL_FALSE, value_ptr(inverse(modProj)));
    glUniform3fv(game->battleMap.gBufferSkybox_programCamposLoc, 1, value_ptr(game->mainCam.pos));
    
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices in the skybox plane
    glDepthFunc(GL_LESS); // reset depth comparison function to its default state

    
    /*//end the query
    glEndQuery(GL_TIME_ELAPSED);
    // wait until the results are available
    GLint stopTimerAvailable = 0;
    while (!stopTimerAvailable) {
        glGetQueryObjectiv(queryID[0],GL_QUERY_RESULT_AVAILABLE,&stopTimerAvailable);
    }
    // get query results
    glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &elapsedTime);
    printf("Time spent on the GPU for frame rendering: %f ms\n", elapsedTime / 1000000.0);*/
    
    //cleanup
//    if(game->mainCam.screenshotRequested)
//    {
//        glReadPixels(0, 0, game->mainCam.screenshotWidth, game->mainCam.screenshotHeight, GL_BGR, GL_UNSIGNED_BYTE, game->battleMap.screenshotPixels);
//
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    }
//
//    if(game->mainCam.videoFramesRequiredLeft > 0)
//    {
//        glReadPixels(0, 0, game->mainCam.videoWidth, game->mainCam.videoHeight, GL_BGR, GL_UNSIGNED_BYTE, game->battleMap.videoPixels);
//
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    glBindVertexArray(0);
    
    defferedRenderFromGBuffer(game);
    
    //invalid value : 1281.   invalid operation : 1282
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n", err);
    assert(err == 0);
    
    /*if(game->mainCam.pos.z < 0)
    {
        GLuint shaderOne = LoadAndLinkShader("building.vert", "bhmEros.frag");
        glUseProgram(shaderOne);
    }
    if(game->mainCam.pos.z < -1000)
    {
        //exit(0);
    }*/
}
