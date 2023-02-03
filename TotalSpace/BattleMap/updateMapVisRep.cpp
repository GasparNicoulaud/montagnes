//
//  updateMapVisRep.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 18/03/2018.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//
#define GL_SILENCE_DEPRECATION

#include "updateMapVisRep.hpp"

void updateTree(SquareBatch * batch, BattleMap * map, Camera cam, Game * game, int divDepth);

void updateMapVisRep(Game * game)
{
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //first pass
    glBindVertexArray(game->battleMap.vaoIDHD);
    glUseProgram(game->battleMap.height_program); //heightmap shader
    glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.fboID);
    
    //prepare fbo rendering
    glViewport(0, 0, game->battleMap.fboWidth, game->battleMap.fboHeight);

    glActiveTexture(GL_TEXTURE0 + 0);
    updateTree(&game->battleMap.mainBatch, &game->battleMap, game->mainCam,game, 0);
    
    
    // Déverrouillage du Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    
    
    err = glGetError();
    if(err != 0)
        printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}

// generate a tile of the map, about 1.7 Mo at fbo_res of 256 for the three textures : albedoRoughness, heightHorizontalDisplacementAO and normals
void genDepthMapQuad(SquareBatch * batch, BattleMap * map, Game *game)
{
    GLenum err;
   /* //performance measurement
    GLuint64 elapsedTime;
    GLuint queryID[1];
    glGenQueries(1, queryID);
    // issue the query
    glBeginQuery(GL_TIME_ELAPSED, queryID[0]);*/
    
    //new textures
    glGenTextures(1, &batch->albedoRoughness);
    glBindTexture(GL_TEXTURE_2D, batch->albedoRoughness);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST | GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, map->fboWidth, map->fboWidth, 0, GL_RGBA, GL_BYTE, NULL); // rgb albedo, a roughness
    
    glGenTextures(1, &batch->heightHorizontalDisplacementAO);
    glBindTexture(GL_TEXTURE_2D, batch->heightHorizontalDisplacementAO);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST | GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, map->fboWidth, map->fboWidth, 0, GL_RGBA, GL_FLOAT, NULL); // r : height, gb : horizontal displace, a:ao
    
    glUseProgram(game->battleMap.height_program); //heightmap shader
    
    glUniform2f(map->offsetLocDynamicMap, batch->offset.x, batch->offset.y);
    glUniform1f(map->scaleLocDynamicMap, batch->scale);
    
    glUniform1i(map->dynamicMapShaderLocations.lightMapLoc, 0);
    glUniform1i(map->dynamicMapShaderLocations.geometryMapLoc, 1);
    glUniform1i(map->dynamicMapShaderLocations.materialMapLoc, 2);
    glUniform1i(map->dynamicMapShaderLocations.stoneMaterialAlbedoHeightLoc, 3);
    glUniform1i(map->dynamicMapShaderLocations.stoneMaterialRouhnessAOLoc, 4);
    glUniform1i(map->dynamicMapShaderLocations.rocksMaterialAlbedoHeightLoc, 5);
    glUniform1i(map->dynamicMapShaderLocations.rocksMaterialRouhnessAOLoc, 6);
    glUniform1i(map->dynamicMapShaderLocations.sandMaterialAlbedoHeightLoc, 7);
    glUniform1i(map->dynamicMapShaderLocations.sandMaterialRouhnessAOLoc, 8);
    glUniform1i(map->dynamicMapShaderLocations.flowMaterialAlbedoHeightLoc, 9);
    glUniform1i(map->dynamicMapShaderLocations.flowMaterialRouhnessAOLoc, 10);
    
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.lightMap);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.geometryMap);
    glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.materialMap);
    glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.stoneMaterialAlbedoHeight);
    glActiveTexture(GL_TEXTURE0 + 4); // Texture unit 4
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.stoneMaterialRouhnessAO);
    glActiveTexture(GL_TEXTURE0 + 5); // Texture unit 5
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.rocksMaterialAlbedoHeight);
    glActiveTexture(GL_TEXTURE0 + 6); // Texture unit 6
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.rocksMaterialRouhnessAO);
    glActiveTexture(GL_TEXTURE0 + 7); // Texture unit 7
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.sandMaterialAlbedoHeight);
    glActiveTexture(GL_TEXTURE0 + 8); // Texture unit 8
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.sandMaterialRouhnessAO);
    glActiveTexture(GL_TEXTURE0 + 9); // Texture unit 9
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.flowMaterialAlbedoHeight);
    glActiveTexture(GL_TEXTURE0 + 10); // Texture unit 10
    glBindTexture(GL_TEXTURE_2D, map->staticMapTextures.flowMaterialRouhnessAO);
    
    //attach the generated texture to the current framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, batch->heightHorizontalDisplacementAO, 0/*mipmap level*/);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, batch->albedoRoughness, 0/*mipmap level*/);
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(2, attachments);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //we should probably just disable depth testing and not bother clearing anything
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    glDrawArrays(GL_TRIANGLES, 0, 6); //draw call, geometry is only a quad, all the good buisness is in frag shader
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0/*mipmap level*/);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0/*mipmap level*/);
    
    glActiveTexture(GL_TEXTURE0 + 10); // Texture unit 10
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 9); // Texture unit 9
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 8); // Texture unit 8
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 7); // Texture unit 7
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 6); // Texture unit 6
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 5); // Texture unit 5
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 4); // Texture unit 4
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, 0);

    /*//end the query
    glEndQuery(GL_TIME_ELAPSED);
    // wait until the results are available
    GLint stopTimerAvailable = 0;
    while (!stopTimerAvailable) {
        glGetQueryObjectiv(queryID[0],GL_QUERY_RESULT_AVAILABLE,&stopTimerAvailable);
    }
    // get query results
    glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &elapsedTime);
    printf("Time spent on the GPU for terrain batch rendering: %f ms\n", elapsedTime / 1000000.0);*/
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    
    
    glUseProgram(game->battleMap.normalMap_program); //heightmap shader
    
    // normalmap text gen
    glGenTextures(1, &batch->normalMap);
    glBindTexture(GL_TEXTURE_2D, batch->normalMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST | GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, map->fboWidth, map->fboWidth, 0, GL_RGB, GL_HALF_FLOAT, NULL);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);

    
    //attach the generated texture to the current framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, batch->normalMap, 0/*mipmap level*/);
    GLuint OneAttachment[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, OneAttachment);
    
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1f(map->scaleLocNormalMap,batch->scale);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);

    glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, batch->heightHorizontalDisplacementAO);

    glDrawArrays(GL_TRIANGLES, 0, 6); //draw call, geometry is only a quad, all the good buisness is in frag shader
    glBindTexture(GL_TEXTURE_2D, 0);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);

}

//remove a batch from the display list and free its resources
void clearOneBatch(SquareBatch * batch, BattleMap * map)
{
    assert(batch->posInDisplayList >= 0);
    assert(batch->posInDisplayList < map->bmc);
    map->activeBatch[batch->posInDisplayList] = false;
    map->listOfSquareBatchesToDisplay[batch->posInDisplayList] = nullptr;
    map->gridsToDisplayCount--;
    batch->posInDisplayList = -1;
    //glDeleteTextures(1, &batch->texture);
    glDeleteTextures(1, &batch->normalMap);
    glDeleteTextures(1, &batch->heightHorizontalDisplacementAO);
    glDeleteTextures(1, &batch->albedoRoughness);
}

void clearSubBatches(SquareBatch * batch, BattleMap * map) //will free and destroy recursively the childrens of a batch but not the batch itself !
{
    for (int i = 0 ; i < 4; i++) // for each children
    {
        if(batch->subBatch[i].isDivided) //if this child is divided
        {
            clearSubBatches(&batch->subBatch[i],map);//clear it
        }
        else
        {
            //remove batch from display list and free its resources
            //assert(batch->subBatch[i].posInDisplayList >= 0);
            clearOneBatch(&batch->subBatch[i],map);
        }
    }
    delete(batch->subBatch);
    batch->isDivided = false;
}

void updateTree(SquareBatch * batch, BattleMap * map, Camera cam, Game * game, int divDepth)
{
    
    //center of the current batch relative to the world
    vec2 ccb = (batch->offset + vec2(0.5 * batch->scale * 10000.0));
    
    vec2 camxy = vec2(cam.pos);
    
    float dstfromCam = distance(ccb,camxy); // distance of the current batch's center to the camera
    
    dstfromCam *= 0.3; //good place to adjsut quality of terrain rendering, lower number gives higher resolution, higher gives better performance
    
    //if camera is close to the center of the current batch and the batch is not too small yet
    if(dstfromCam < 10000 * 1.41421356 * batch->scale && 10000 * 1.41421356 * batch->scale > map->minRad) //if should be divd
    {
        if(batch->isDivided) //if its already divided (as it should) we update its children
        {
            updateTree(&batch->subBatch[0], map, cam, game, divDepth+1);
            updateTree(&batch->subBatch[1], map, cam, game, divDepth+1);
            updateTree(&batch->subBatch[2], map, cam, game, divDepth+1);
            updateTree(&batch->subBatch[3], map, cam, game, divDepth+1);
        }
        else //else if the batch isnt divided yet, it should be
        {
            //assert we have space for them
            //assert(map->bmc - map->gridsToDisplayCount >= 4);
            
            //batch is not divided yet so we can assume its in displaylist
            //assert(batch->posInDisplayList >= 0);
            
            // first remove batch from display list and free its resources
            clearOneBatch(batch,map);
            
            //now create 4 subBatches
            batch->subBatch = new SquareBatch[4];
            
            // place their origin (offset from 0,0)
            batch->subBatch[0].offset = batch->offset;
            batch->subBatch[1].offset = batch->offset + vec2(1,0) * float(batch->scale * 0.5 * 10000.0);
            batch->subBatch[2].offset = batch->offset + vec2(0,1) * float(batch->scale * 0.5 * 10000.0);
            batch->subBatch[3].offset = batch->offset + vec2(1,1) * float(batch->scale * 0.5 * 10000.0);
            
            for (int i = 0 ; i < 4; i++) { // for each new children
                batch->subBatch[i].isDivided = false; //just created so its not divided
                batch->subBatch[i].scale = batch->scale * 0.5; //its scaled twice as small from its parent
                
                genDepthMapQuad(&batch->subBatch[i],map,game); //gen its heightmap and normalmap
                
                //displaylist insertion
                //bool debugFoundASpot = false;
                for (int j = 0 ; j < map->bmc; j++) { //find a free spot
                    if(!map->activeBatch[j]){//if the spot "j" is free, put the new batch inside
                        batch->subBatch[i].posInDisplayList = j;
                        map->activeBatch[j] = true;
                        map->listOfSquareBatchesToDisplay[j] = &batch->subBatch[i];
                        map->gridsToDisplayCount++;
                        //debugFoundASpot = true;
                        break;
                    }
                }
                //assert(debugFoundASpot);
            }
            batch->isDivided = true;
        }
    }
    else//not close enough to divide it should be displayed as is
    {
        if(batch->isDivided){ //the batch was divided but its not close enough to be so we undivide it
            //first let's clear its children
            clearSubBatches(batch,map);
            
            //as it was divided, it wasnt in the display list, so lets put it back in
            genDepthMapQuad(batch,map, game);
            
            //bool debugFoundASpot = false;
            for (int j = 0 ; j < map->bmc; j++) { //find a free spot
                if(!map->activeBatch[j]){//if the spot "j" is free, put the new batch inside
                    batch->posInDisplayList = j;
                    map->activeBatch[j] = true;
                    map->listOfSquareBatchesToDisplay[j] = batch;
                    map->gridsToDisplayCount++;
                    //debugFoundASpot = true;
                    break;
                }
            }
            //assert(debugFoundASpot);
        }
    }
}
