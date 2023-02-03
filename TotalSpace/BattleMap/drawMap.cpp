//
//  drawMap.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 18/03/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "drawMap.hpp"

void renderBranch(SquareBatch * batch, BattleMap * map, Camera cam)
{
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //finally lets render !
    glUniform2f(map->offsetLocFinalRender, batch->offset.x, batch->offset.y);
    glUniform1f(map->scaleLocFinalRender,batch->scale);
    //glUniform1f(glGetUniformLocation(map->terrain_program, "time"),map->time);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);

    glUniform1i(map->finalRenderShaderLocations.heightHorizontalDisplacementAOLoc, 0);
    glUniform1i(map->finalRenderShaderLocations.albedoRoughnessLoc, 1);
    glUniform1i(map->finalRenderShaderLocations.normalMapLoc, 2);
    glUniform1i(map->finalRenderShaderLocations.lightMapLoc, 3);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n", err);
    assert(err == 0);
    
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, batch->heightHorizontalDisplacementAO);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, batch->albedoRoughness);
    glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
    glBindTexture(GL_TEXTURE_2D, batch->normalMap);
    glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.lightMap);
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n", err);
    assert(err == 0);
    ///*
    glDrawElements(
                   GL_TRIANGLES,      // mode GL_TRIANGLES   GL_LINE_STRIP    GL_LINES
                   map->gridres*map->gridres*6,    // count
                   GL_UNSIGNED_INT,   // type
                   (void*)0           // element array buffer offset
                   );
     //*/
    
    glActiveTexture(GL_TEXTURE0 + 3); //maybe keep texture +3 bound as it is the global lightmap and doesnt change from batch to batch ?
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void drawMap(Game * game)
{
    GLenum err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    // Matrices projection et modelview
    mat4 projection;
    mat4 modelView;
    //mat4 modelPos;
    mat4 modProj;
    
    //prepare view matrices
    glViewport(0, 0, game->windowWidth, game->windowHeight);
    double fieldOfView = game->mainCam.FOV;
    projection = perspective(fieldOfView, (double) game->windowWidth / game->windowHeight, 1.0, 25000.0);
    modelView = lookAt(game->mainCam.pos, game->mainCam.pos+game->mainCam.orientation, game->mainCam.up);
    modProj = projection * modelView;
    
    if(game->mainCam.screenshotRequested)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.screenshotFbo);
        glViewport(0, 0, game->mainCam.screenshotWidth, game->mainCam.screenshotHeight);
        projection = perspective(fieldOfView, (double) game->mainCam.screenshotWidth / game->mainCam.screenshotHeight, 1.0, 25000.0);
        modelView = lookAt(game->mainCam.pos, game->mainCam.pos+game->mainCam.orientation, game->mainCam.up);
        modProj = projection * modelView;
    }
    
    if(game->mainCam.videoFramesRequiredLeft > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.videoFbo);
        glViewport(0, 0, game->mainCam.videoWidth, game->mainCam.videoHeight);
        projection = perspective(fieldOfView, (double) game->mainCam.videoWidth / game->mainCam.videoHeight, 1.0, 25000.0);
        modelView = lookAt(game->mainCam.pos, game->mainCam.pos+game->mainCam.orientation, game->mainCam.up);
        modProj = projection * modelView;
    }
    
    
    
    //prepare the other uniforms
    //float sunRotSpeed = 0.01f * 10;
    //sunRotSpeed = 0.0f;
    //float sunHeightInSky = 1.5f; //high sun : 1.2f
    //float time = game->battleMap.time;
    vec3 sunlightDirection = vec3(game->battleMap.recipe.sunPosX,game->battleMap.recipe.sunPosY,game->battleMap.recipe.sunPosZ); //rotation along vertical axis
    //vec3 sunlightDirection = normalize(vec3(sin(time * sunRotSpeed),0,cos(time * sunRotSpeed))); //rotation along horizontal axis
    mat4 inversemodproj = inverse(modProj); // for skybox shader
    
    //begin normal render
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    /*//performance measurement
    GLuint64 elapsedTime;
    GLuint queryID[1];
    glGenQueries(1, queryID);
    // issue the query
    glBeginQuery(GL_TIME_ELAPSED, queryID[0]);*/
    
    //draw the terrain
    
    glBindVertexArray(game->battleMap.vaoID);
    glUseProgram(game->battleMap.terrain_program); //final render shader
    
    glUniformMatrix4fv(game->battleMap.modprojLoc, 1, GL_FALSE, value_ptr(modProj));
    glUniform3fv(game->battleMap.camposLocFinalRender, 1, value_ptr(game->mainCam.pos));
    glUniform3fv(game->battleMap.sunlightDirectionLocFinalRender, 1, value_ptr(sunlightDirection));

    int count = 0;
    
    // actual terrain redenring loop
    for (int i = 0 ; i < game->battleMap.bmc; i++) { //render all batches on the display list
        if(game->battleMap.activeBatch[i]){//if the batch is active, render it
            renderBranch(game->battleMap.listOfSquareBatchesToDisplay[i], &game->battleMap, game->mainCam);
            count++;
        }
    }
    
    glBindVertexArray(0);
    
    if(count != game->battleMap.gridsToDisplayCount)
    {
        printf("???\nbmc : %i\n???\n",game->battleMap.bmc);
    }
    assert(count == game->battleMap.gridsToDisplayCount);
    //printf("terrain batch count this frame : %i\n\n",count);
    
    if(game->battleMap.gridsToDisplayCount >= game->battleMap.bmc)
    {
        printf("???\nbmc : %i\n???\n",game->battleMap.bmc);
    }
    assert(game->battleMap.gridsToDisplayCount < game->battleMap.bmc);
    
    if(game->battleMap.bmc < 0)
    {
        printf("???\nbmc : %i\n???\n",game->battleMap.bmc);
    }
    assert(game->battleMap.bmc > 0);
    
    
    //building render
    
    modProj = projection * modelView * glm::mat4(1.0f);
    
    glBindVertexArray(game->battleMap.buildings.vao);
    glUseProgram(game->battleMap.buildings.program); //buildings shaders
    //game->battleMap.recipe.heightAtMiddle = 150;
    //printf("building ofst from O meters : %f\n",game->battleMap.recipe.heightAtMiddle);
    glUniform1f(game->battleMap.buildings.zOffsetLoc, game->battleMap.recipe.heightAtMiddle);
    glUniformMatrix4fv(game->battleMap.buildings.MVPLoc, 1, GL_FALSE, value_ptr(modProj));
    
    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices in a cube
    
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // CLEAR EVERYTHING TO ONLY DRAW SKYQUAD

    
    
    
    //draw the skybox
    glDepthFunc(GL_LEQUAL); // make opengl draw the sky even when less than or equal to zFar (it's always equal for the skybox which is only a skyQuad
    glBindVertexArray(game->battleMap.skyboxVao);
    glUseProgram(game->battleMap.skybox_program);
    
    glUniformMatrix4fv(game->battleMap.inversemodprojLoc, 1, GL_FALSE, value_ptr(inversemodproj));
    glUniform3fv(game->battleMap.camposLocSkybox, 1, value_ptr(game->mainCam.pos));
    glUniform3fv(game->battleMap.sunlightDirectionLocSkybox, 1, value_ptr(sunlightDirection));
    
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
    if(game->mainCam.screenshotRequested)
    {
        glReadPixels(0, 0, game->mainCam.screenshotWidth, game->mainCam.screenshotHeight, GL_BGR, GL_UNSIGNED_BYTE, game->battleMap.screenshotPixels);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    if(game->mainCam.videoFramesRequiredLeft > 0)
    {
        glReadPixels(0, 0, game->mainCam.videoWidth, game->mainCam.videoHeight, GL_BGR, GL_UNSIGNED_BYTE, game->battleMap.videoPixels);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glUseProgram(0);
    glBindVertexArray(0);
    glDisable(GL_FRAMEBUFFER_SRGB);
    
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
