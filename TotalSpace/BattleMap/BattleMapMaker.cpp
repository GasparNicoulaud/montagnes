//
//  BattleMapMaker.cpp
//  TotalSpace
//
//  Created by Gaspar Nicoulaud on 02/12/2016.
//  Copyright © 2016 Gaspar Nicoulaud. All rights reserved.
//

#include "BattleMapMaker.hpp"
#include "BaseHeightMapMaker.hpp"
#include "BuildingGeometryGenerator.hpp"
#include "ShaderLoader.hpp"
#define GL_SILENCE_DEPRECATION

using namespace glm;

void genDepthMapQuad(SquareBatch * batch, BattleMap * map, Game * game);

void initBattleMap(Game * game)
{
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    double currentTime = glfwGetTime();
    
    //game->battleMap.bmhTex = genBHMGPU(BASE_HM_RES, game->battleMap.recipe); // base offline layer generation
    genBHMGPU(BASE_HM_RES, game->battleMap.recipe,&game->battleMap);
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    
    printf("time of bhm generation : %.4fs\n",glfwGetTime() - currentTime);
    
    //currentTime = glfwGetTime();
    //printf("time of HERE : %.4fs\n",glfwGetTime() - currentTime);
    ///*
    GLfloat valueAtMiddle;
    glBindTexture(GL_TEXTURE_2D,game->battleMap.staticMapTextures.geometryMap);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2048, 2048, 0, GL_RGBA, GL_FLOAT, NULL); //r for height, g for concavity, b & a for normals

    GLsizeiptr bufferSize = BASE_HM_RES * BASE_HM_RES * 4 * 4; //geometryMap is RGBA32F, hence the 4 * 4
    
    GLfloat* pixelData = (float*)malloc(bufferSize);
    glGetTexImage(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  GL_FLOAT,
                  pixelData);
    
    valueAtMiddle = pixelData[((BASE_HM_RES/2) + (BASE_HM_RES/2) * BASE_HM_RES) * 4] + 140; //+140 because materials add about 140 meters
    free(pixelData);
    
    printf("value at middle of the map : %f\n",valueAtMiddle);
    //*/
    game->battleMap.recipe.heightAtMiddle = valueAtMiddle;
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    
    // Déverrouillage général
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    
    glUseProgram(0);
    glBindVertexArray(0);
    
    //Here : 6.0s
    
    
    //skybox vertices (only a quad actually, not a box)
    static const GLfloat skyboxVertices[] = {
        -1.0f,-1.0f, 1.0f, // triangle 1 : begin
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f, // triangle 1 : end
        
         1.0f,-1.0f, 1.0f, // triangle 2 : begin
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f // triangle 2 : end
    };
    
    //then finally we fill the vao
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    //remplissage du vbo
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 18 * sizeof(float), skyboxVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //vbo done
    //now we do the vao
    glGenVertexArrays(1, &game->battleMap.skyboxVao);
    glBindVertexArray(game->battleMap.skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Déverrouillage du VAO
    glBindVertexArray(0);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    
    
    //skybox shader
    game->battleMap.skybox_program = LoadAndLinkShader("skybox.vert", "skybox.frag");
    glUseProgram(game->battleMap.skybox_program);
    game->battleMap.inversemodprojLoc = glGetUniformLocation(game->battleMap.skybox_program, "inversemodproj");
    game->battleMap.camposLocSkybox = glGetUniformLocation(game->battleMap.skybox_program, "campos");
    game->battleMap.sunlightDirectionLocSkybox = glGetUniformLocation(game->battleMap.skybox_program, "sunlightDirection");

    
    //final terrain renderer shader program
    game->battleMap.terrain_program = LoadAndLinkShader("finalRender.vert","finalRender.frag");
    glUseProgram(game->battleMap.terrain_program);
    game->battleMap.modprojLoc = glGetUniformLocation(game->battleMap.terrain_program, "modproj");
    game->battleMap.camposLocFinalRender = glGetUniformLocation(game->battleMap.terrain_program, "campos");
    game->battleMap.sunlightDirectionLocFinalRender = glGetUniformLocation(game->battleMap.terrain_program, "sunlightDirection");
    game->battleMap.offsetLocFinalRender = glGetUniformLocation(game->battleMap.terrain_program, "offset");
    game->battleMap.scaleLocFinalRender = glGetUniformLocation(game->battleMap.terrain_program, "scale");
    game->battleMap.localHeightmapLoc = glGetUniformLocation(game->battleMap.terrain_program, "localHeightmap");
    game->battleMap.localNormalmapLoc = glGetUniformLocation(game->battleMap.terrain_program, "localNormalmap");
    game->battleMap.globalLightMapLoc = glGetUniformLocation(game->battleMap.terrain_program, "globalLightMap");
    
    game->battleMap.finalRenderShaderLocations.heightHorizontalDisplacementAOLoc = glGetUniformLocation(game->battleMap.terrain_program, "heightHorizontalDisplacementAO");
    game->battleMap.finalRenderShaderLocations.albedoRoughnessLoc = glGetUniformLocation(game->battleMap.terrain_program, "albedoRoughness");
    game->battleMap.finalRenderShaderLocations.normalMapLoc = glGetUniformLocation(game->battleMap.terrain_program, "localNormalmap");
    game->battleMap.finalRenderShaderLocations.lightMapLoc = glGetUniformLocation(game->battleMap.terrain_program, "globalLightMap");
    
    
    //normalmap shader
    game->battleMap.normalMap_program = LoadAndLinkShader("passThru.vert","DynamicMapGen/normalMap.frag");
    glUseProgram(game->battleMap.normalMap_program);
    game->battleMap.scaleLocNormalMap = glGetUniformLocation(game->battleMap.normalMap_program, "scale");
    
    
    //dynamic map shader
    game->battleMap.height_program = LoadAndLinkShader("dynamicMap.vert","dynamicMap.frag");
    glUseProgram(game->battleMap.height_program);
    game->battleMap.offsetLocDynamicMap = glGetUniformLocation(game->battleMap.height_program, "offset");
    game->battleMap.scaleLocDynamicMap = glGetUniformLocation(game->battleMap.height_program, "scale");
    //game->battleMap.flowmapCarvingStrengthLoc = glGetUniformLocation(game->battleMap.height_program, "flowmapCarvingStrength");
 
    //game->battleMap.dynamicMapShaderLocations.bhmLoc = glGetUniformLocation(game->battleMap.height_program, "baseHeightMap"); //vestigial
    game->battleMap.dynamicMapShaderLocations.lightMapLoc = glGetUniformLocation(game->battleMap.height_program, "lightMap");
    game->battleMap.dynamicMapShaderLocations.geometryMapLoc = glGetUniformLocation(game->battleMap.height_program, "geometryMap");
    game->battleMap.dynamicMapShaderLocations.materialMapLoc = glGetUniformLocation(game->battleMap.height_program, "materialMap");
    game->battleMap.dynamicMapShaderLocations.rocksMaterialRouhnessAOLoc = glGetUniformLocation(game->battleMap.height_program, "rocksMaterialRouhnessAO");
    game->battleMap.dynamicMapShaderLocations.rocksMaterialAlbedoHeightLoc = glGetUniformLocation(game->battleMap.height_program, "rocksMaterialAlbedoHeight");
    game->battleMap.dynamicMapShaderLocations.stoneMaterialRouhnessAOLoc = glGetUniformLocation(game->battleMap.height_program, "stoneMaterialRouhnessAO");
    game->battleMap.dynamicMapShaderLocations.stoneMaterialAlbedoHeightLoc = glGetUniformLocation(game->battleMap.height_program, "stoneMaterialAlbedoHeight");
    game->battleMap.dynamicMapShaderLocations.sandMaterialRouhnessAOLoc = glGetUniformLocation(game->battleMap.height_program, "sandMaterialRouhnessAO");
    game->battleMap.dynamicMapShaderLocations.sandMaterialAlbedoHeightLoc = glGetUniformLocation(game->battleMap.height_program, "sandMaterialAlbedoHeight");
    game->battleMap.dynamicMapShaderLocations.flowMaterialRouhnessAOLoc = glGetUniformLocation(game->battleMap.height_program, "flowMaterialRouhnessAO");
    game->battleMap.dynamicMapShaderLocations.flowMaterialAlbedoHeightLoc = glGetUniformLocation(game->battleMap.height_program, "flowMaterialAlbedoHeight");
    
    //horizontal Displacement Coefficient, 1 or -1
    glUniform1f(glGetUniformLocation(game->battleMap.height_program, "horizontalDisplacementCoeff"), game->battleMap.recipe.horizontalDisplacementCoeff);
    
    //final terrain deffered renderer shader programs
    game->battleMap.fillGbuffer = LoadAndLinkShader("Deferred/fillGbuffer.vert","Deferred/fillGbuffer.frag");
    glUseProgram(game->battleMap.fillGbuffer);
    game->battleMap.fillGbuffermodprojLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "modproj");
    //game->battleMap.fillGbuffercamposLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "campos");
    //game->battleMap.sunlightDirectionLocFinalRender = glGetUniformLocation(game->battleMap.terrain_program, "sunlightDirection");
    game->battleMap.fillGbufferoffsetLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "offset");
    game->battleMap.fillGbufferscaleLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "scale");
    //game->battleMap.fillGbufferlocalHeightmapLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "localHeightmap");
    //game->battleMap.fillGbufferlocalNormalmapLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "localNormalmap");
    //game->battleMap.globalLightMapLoc = glGetUniformLocation(game->battleMap.terrain_program, "globalLightMap");
    
    game->battleMap.fillGbufferShaderLocations.heightHorizontalDisplacementAOLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "heightHorizontalDisplacementAO");
    game->battleMap.fillGbufferShaderLocations.albedoRoughnessLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "albedoRoughnessMap");
    game->battleMap.fillGbufferShaderLocations.normalMapLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "localNormalmap");
//    game->battleMap.fillGbufferShaderLocations.lightMapLoc = glGetUniformLocation(game->battleMap.fillGbuffer, "globalLightMap");
    
    // draw g buffer
    game->battleMap.drawGbufferProgram = LoadAndLinkShader("passThru.vert","Deferred/drawFromGBuffer.frag");
    glUseProgram(game->battleMap.drawGbufferProgram);
    game->battleMap.drawGbufferPositionLoc = glGetUniformLocation(game->battleMap.drawGbufferProgram, "positionSampler");
    game->battleMap.drawGbufferAlbedoRoughnesLoc = glGetUniformLocation(game->battleMap.drawGbufferProgram, "albedoRoughnessSampler");
    game->battleMap.drawGbufferNormalSamplerLoc = glGetUniformLocation(game->battleMap.drawGbufferProgram, "normalSampler");
    game->battleMap.drawGbufferGlobalLightMapLoc = glGetUniformLocation(game->battleMap.drawGbufferProgram, "globalLightMap");
    game->battleMap.drawGbufferCampos = glGetUniformLocation(game->battleMap.drawGbufferProgram, "campos");
    game->battleMap.drawGbufferSunlightDirection = glGetUniformLocation(game->battleMap.drawGbufferProgram, "sunlightDirection");
    
    //skybox shader for g buffer
    game->battleMap.gBufferSkybox_program = LoadAndLinkShader("Deferred/skybox.vert", "Deferred/skybox.frag");
    glUseProgram(game->battleMap.gBufferSkybox_program);
    game->battleMap.gBufferSkybox_programInversemodprojLoc = glGetUniformLocation(game->battleMap.gBufferSkybox_program, "inversemodproj");
    game->battleMap.gBufferSkybox_programCamposLoc = glGetUniformLocation(game->battleMap.gBufferSkybox_program, "campos");
    
    
    //square grid gen
    float mapscale = 10000;
    const int gridres = game->battleMap.gridres;
    
    float verticesGrid[(gridres + 1) * (gridres + 1) * 2];
    for (int x = 0; x <= gridres ; x++){
        for (int y = 0; y <= gridres ; y++){
            
            verticesGrid[(x * (gridres + 1) + y) * 2] = mapscale * x/float(gridres);
            verticesGrid[(x * (gridres + 1) + y) * 2 + 1] = mapscale * y/float(gridres);
        }
    }
    
    //Here : 6.0s
    
    int indexGrid[gridres * gridres * 6];
    for (int x = 0; x < gridres; x++){ //
        for (int y = 0; y < gridres; y++ ){ //
            indexGrid[(x * gridres + y) * 6 + 0] = ((x + 0) * (gridres + 1) + y + 0);
            indexGrid[(x * gridres + y) * 6 + 1] = ((x + 1) * (gridres + 1) + y + 0);
            indexGrid[(x * gridres + y) * 6 + 2] = ((x + 0) * (gridres + 1) + y + 1);
            indexGrid[(x * gridres + y) * 6 + 3] = ((x + 1) * (gridres + 1) + y + 0);
            indexGrid[(x * gridres + y) * 6 + 4] = ((x + 1) * (gridres + 1) + y + 1);
            indexGrid[(x * gridres + y) * 6 + 5] = ((x + 0) * (gridres + 1) + y + 1);
        }
    }
    
    //Here : 6.1s
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n", err);
    assert(err == 0);
    
    //put previously generated vertex data into opengl memory
    
    glGenVertexArrays(1, &game->battleMap.vaoID);
    glBindVertexArray(game->battleMap.vaoID);
    
    glGenBuffers(1, &game->battleMap.vboID);
    glBindBuffer(GL_ARRAY_BUFFER, game->battleMap.vboID);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesGrid), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesGrid), verticesGrid);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    
    glGenBuffers(1, &game->battleMap.iboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game->battleMap.iboID);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexGrid), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indexGrid), indexGrid);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //done
    
    //Here : 6.1s
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //simple quad for the heightmap renderer
    float quadVertices[3*2*2] = {
        -1, -1,
         1, -1,
        -1,  1,
         1, -1,
         1,  1,
        -1,  1
    };
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    //here we prepare vao stuff for depthmap rendering
    glGenBuffers(1, &game->battleMap.vboIDHD); //contains a simple quad
    glBindBuffer(GL_ARRAY_BUFFER, game->battleMap.vboIDHD);
    
    //remplissage du vbo
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //vbo done
    //now we do the vao
    
    glGenVertexArrays(1, &game->battleMap.vaoIDHD);
    glBindVertexArray(game->battleMap.vaoIDHD);
    
    glBindBuffer(GL_ARRAY_BUFFER, game->battleMap.vboIDHD);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    // Déverrouillage du VAO
    glBindVertexArray(0);
    
    //MAP VIS REP BATCH TREE BUISNESS
    //prepare the display list
    for (int j = 0 ; j < game->battleMap.bmc; j++) { //init list to false
        game->battleMap.activeBatch[j] = false;
        game->battleMap.listOfSquareBatchesToDisplay[j] = nullptr;
    }
    game->battleMap.gridsToDisplayCount = 0;
    
    game->battleMap.radius = 5000;
    
    //first pass
    glBindVertexArray(game->battleMap.vaoIDHD); //  contains the simple quad
    glUseProgram(game->battleMap.height_program); //heightmap shaderbecause we will gen some heightmap
    
    glGenFramebuffers(1, &game->battleMap.fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.fboID);
    
    
    //mainbatch positioning
    
    game->battleMap.mainBatch.offset = vec2(-5000,-5000);
    game->battleMap.mainBatch.scale = 1;
    game->battleMap.mainBatch.isDivided = false;
    game->battleMap.mainBatch.posInDisplayList = 0;
    game->battleMap.activeBatch[0] = true;
    game->battleMap.listOfSquareBatchesToDisplay[0] = &game->battleMap.mainBatch;
    game->battleMap.gridsToDisplayCount = 1;
    
    glViewport(0, 0, game->battleMap.fboWidth, game->battleMap.fboHeight);
    genDepthMapQuad(&game->battleMap.mainBatch,&game->battleMap,game);
    
    //done
    
    //init camera
    initCam(game);
    
    // FBO and texture for g buffer  ____________________________________________________________________ BEGIN
    //fbo gen and bind
    glGenFramebuffers(1, &game->battleMap.gBufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.gBufferFbo);
    
    //Generate the texture for the g buffer
    glGenTextures(1,&game->battleMap.gBufferPosition);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,game->battleMap.gBufferPosition);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,game->windowWidth, game->windowHeight,0,GL_RGB,GL_FLOAT,NULL);
    glGenTextures(1,&game->battleMap.gBufferAlbedoRoughness);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,game->battleMap.gBufferAlbedoRoughness);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,game->windowWidth, game->windowHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glGenTextures(1,&game->battleMap.gBufferNormal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,game->battleMap.gBufferNormal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,game->windowWidth, game->windowHeight,0,GL_RGB,GL_FLOAT,NULL);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, game->battleMap.gBufferPosition,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, game->battleMap.gBufferAlbedoRoughness,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, game->battleMap.gBufferNormal,0);

    // Generate the depth attachment
    glGenRenderbuffers(1,&game->battleMap.gBufferDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, game->battleMap.gBufferDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, game->windowWidth, game->windowHeight);
    
    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, game->battleMap.gBufferDepthBuffer);
    
    // FBO and texture for g buffer  _______________________________________________________________ DONE
    
    // FBO and texture for screenshot  ____________________________________________________________________ BEGIN
    //fbo gen and bind
    glGenFramebuffers(1, &game->battleMap.screenshotFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.screenshotFbo);
    
    //Generate the texture for the screenshot
    glGenTextures(1,&game->battleMap.screenshotTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,game->battleMap.screenshotTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //probably unedeed as we will never sample from this texture ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //probably unedeed as we will never sample from this texture ?
    glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB8,game->mainCam.screenshotWidth,game->mainCam.screenshotHeight,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, game->battleMap.screenshotTexture,0);

    // Generate the depth attachment
    glGenRenderbuffers(1,&game->battleMap.screenshotDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, game->battleMap.screenshotDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, game->mainCam.screenshotWidth,game->mainCam.screenshotHeight);
    
    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, game->battleMap.screenshotDepthBuffer);
    
    typedef uint8_t BYTE;
    game->battleMap.screenshotPixels = new BYTE[3 * game->mainCam.screenshotWidth * game->mainCam.screenshotHeight];
    
    //debug code to print maximum screenshot size :
    //GLint dims[2];
    //glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dims[0]);
    //printf("Max viewport dimensions : %i x %i\n",dims[0],dims[1]);
    
    // FBO and texture for screenshot  _______________________________________________________________ DONE
    
    // FBO and texture for video  ____________________________________________________________________ BEGIN
    //fbo gen and bind
    glGenFramebuffers(1, &game->battleMap.videoFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, game->battleMap.videoFbo);
    
    //Generate the texture for the screenshot
    glGenTextures(1,&game->battleMap.videoTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,game->battleMap.videoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //probably unedeed as we will never sample from this texture ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //probably unedeed as we will never sample from this texture ?
    glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB8,game->mainCam.videoWidth,game->mainCam.videoHeight,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, game->battleMap.videoTexture,0);

    // Generate the depth attachment
    glGenRenderbuffers(1,&game->battleMap.videoDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, game->battleMap.videoDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, game->mainCam.videoWidth,game->mainCam.videoHeight);
    
    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, game->battleMap.videoDepthBuffer);
    
    typedef uint8_t BYTE;
    game->battleMap.videoPixels = new BYTE[3 * game->mainCam.videoWidth * game->mainCam.videoHeight];

    
    // FBO and texture for video  _______________________________________________________________ DONE
    
    
    game->battleLoaded = true;
    game->battleRequired = false;
       
    glUseProgram(0);
    glBindVertexArray(0);
       
    // Déverrouillage du Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
    
    printf("Battle Map Loaded\n");
}





