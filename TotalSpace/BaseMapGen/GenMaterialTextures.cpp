//
//  GenMaterialTextures.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 19/02/2020.
//  Copyright © 2020 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "GenMaterialTextures.hpp"
#include <assert.h>
#include "ShaderLoader.hpp"

vec3 vec3lerp(vec3 a, vec3 b, float fact){
    return vec3(
                    b.x * fact + a.x * (1.0-fact),
                    b.y * fact + a.y * (1.0-fact),
                    b.z * fact + a.z * (1.0-fact)
                );
}

void genMaterialTexture(GLuint* albedoHeightTexture, GLuint* roughnessAOTexture, MaterialRecipe materialRecipe, int textSize)
{
    glGenTextures(1, albedoHeightTexture);
    glBindTexture(GL_TEXTURE_2D, *albedoHeightTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST maybe for iquilez software interpolation ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST maybe for iquilez software interpolation ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textSize, textSize, 0, GL_RGBA, GL_FLOAT, NULL); //rgb albedo & a for height
    
    glGenTextures(1, roughnessAOTexture);
    glBindTexture(GL_TEXTURE_2D, *roughnessAOTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST maybe for iquilez software interpolation ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST maybe for iquilez software interpolation ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_CLAMP_TO_EDGE | GL_REPEAT | GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, textSize, textSize, 0, GL_RG, GL_BYTE, NULL);// R for roughness & G for ambient occlusion
    
    GLuint materialShader;
    GLuint aoShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/computeAO.frag");
    
    /*enum MaterialShaders{
        SAND,
        JAGGEDROCKS,
        CLIFFFACE,
        VORONOIROCKFIELD,
        SMOOTHSTONE,
        WHITESNOW,
        DIRTANDPEBBLES,
        POKEDVORONOI,
        ROCKSANDCRATERS,
        FINEGRAVEL,
        ELONGATEDBLOCKS,
        ROUGHROCK
    };*/
    
    int seed = materialRecipe.seed;
    float rng01a = ((float)(seed % 100)) * 0.012;
    float rng01b = abs(sin(rng01a + seed));
    float rng01c = abs(sin(rng01b + seed));
    float rng01d = abs(sin(rng01c + seed));
    
    if(materialRecipe.materialShader == ROUGHROCK)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/roughRock.frag");
        glUseProgram(materialShader);
        
        //random
        vec3 baseColor = vec3lerp(vec3(0.239,0.145,0.092), vec3(0.041,0.042,0.096), rng01a) * (1.0f - rng01c) * 3.0f;
        baseColor = vec3lerp(baseColor, vec3(0.541,0.192,0.176), (1.0f - rng01d) * rng01b);
        vec3 modulatedColor = vec3lerp(vec3(0.719,0.768,0.647), vec3(0.519,0.518,0.697), rng01b) * rng01d;
        
        glUniform3fv(glGetUniformLocation(materialShader, "baseColor"), 1, value_ptr(baseColor));
        glUniform3fv(glGetUniformLocation(materialShader, "modulatedColor"), 1, value_ptr(modulatedColor));

        printf("rough rock\n");
        printf("using rng01a : %f & rng01b %f & rng01c %f & rng01d %f \n", rng01a, rng01b, rng01c, rng01d);
    }
    else if(materialRecipe.materialShader == SAND)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/sandSmallDunes.frag");
        glUseProgram(materialShader);
        
        //blue gray
        //vec3 baseColor = vec3(0.041,0.042,0.066);
        //vec3 modulatedColor = vec3(0.519,0.518,0.697);
        
        //light sand
        //vec3 baseColor = vec3(0.210,0.145,0.092);
        //vec3 modulatedColor = vec3(0.719,0.768,0.647);
        
        //random
        vec3 baseColor = vec3lerp(vec3(0.210,0.145,0.092), vec3(0.041,0.042,0.066), rng01a);
        vec3 modulatedColor = vec3lerp(vec3(0.719,0.768,0.647), vec3(0.519,0.518,0.697), rng01b);
        
        //dark brown
        //vec3 baseColor = vec3(0.0081,0.0032,0.0026);
        //vec3 modulatedColor = vec3(0.0119,0.0118,0.0097);
        glUniform3fv(glGetUniformLocation(materialShader, "baseColor"), 1, value_ptr(baseColor));
        glUniform3fv(glGetUniformLocation(materialShader, "modulatedColor"), 1, value_ptr(modulatedColor));

        printf("sand\n");
        printf("using rng01a : %f & rng01b %f \n", rng01a, rng01b);
    }
    else if(materialRecipe.materialShader == JAGGEDROCKS)
    {
        vec3 baseColor = vec3lerp(vec3(0.569,0.519,0.507), vec3(0.041,0.042,0.066), rng01a);
        vec3 modulatedColor = vec3lerp(vec3(0.569,0.519,0.507), vec3(0.519,0.518,0.697), rng01b);
        
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/rockySand.frag");
        glUseProgram(materialShader);
        glUniform3fv(glGetUniformLocation(materialShader, "rockColor"), 1, value_ptr(baseColor));
        glUniform3fv(glGetUniformLocation(materialShader, "sandColor"), 1, value_ptr(modulatedColor));
        
        printf("rocky sand (jaggedrocks) (snow and black rocks)\n");
    }
    else if(materialRecipe.materialShader == VORONOIROCKFIELD)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/voronoiRockField.frag");
        glUseProgram(materialShader);
        
        //blue gray
        //vec3 baseColor = vec3(0.041,0.042,0.066);
        //vec3 modulatedColor = vec3(0.519,0.518,0.697);
        
        //light brown
        //vec3 baseColor = vec3(0.041,0.025,0.016);
        //vec3 modulatedColor = vec3(0.119,0.118,0.097);
        
        //random
        vec3 baseColor = vec3lerp(vec3(0.041,0.025,0.016), vec3(0.0081,0.0032,0.0026), rng01a);
        vec3 modulatedColor = vec3lerp(vec3(0.119,0.118,0.097), vec3(0.0119,0.0118,0.0097), rng01b);
        
        //dark brown
        //vec3 baseColor = vec3(0.0081,0.0032,0.0026);
        //vec3 modulatedColor = vec3(0.0119,0.0118,0.0097);
        glUniform3fv(glGetUniformLocation(materialShader, "baseColor"), 1, value_ptr(baseColor));
        glUniform3fv(glGetUniformLocation(materialShader, "modulatedColor"), 1, value_ptr(modulatedColor));
        
        printf("voronoi rock field\n");
        printf("using rng01a : %f & rng01b %f \n", rng01a, rng01b);
    }
    else if(materialRecipe.materialShader == SMOOTHSTONE)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/smoothStone.frag");
        glUseProgram(materialShader);
        
        //blue gray
        //vec3 baseColor = vec3(0.041,0.042,0.066);
        //vec3 modulatedColor = vec3(0.519,0.518,0.697);
        
        //light brown
        //vec3 baseColor = vec3(0.041,0.022,0.016);
        //vec3 modulatedColor = vec3(0.119,0.118,0.097);
        
        //dark brown
        //vec3 baseColor = vec3(0.0081,0.0062,0.0036);
        //vec3 modulatedColor = vec3(0.119,0.092,0.067);
        
        //random
        vec3 baseColor = vec3lerp(vec3(0.041,0.042,0.056), vec3(0.0081,0.0032,0.0026), rng01a);
        vec3 modulatedColor = vec3lerp(vec3(0.519,0.518,0.597), vec3(0.0119,0.0118,0.0097), rng01b);
        
        glUniform3fv(glGetUniformLocation(materialShader, "baseColor"), 1, value_ptr(baseColor));
        glUniform3fv(glGetUniformLocation(materialShader, "modulatedColor"), 1, value_ptr(modulatedColor));
        
        printf("smooth stone\n");
        printf("using rng01a : %f & rng01b %f \n", rng01a, rng01b);
    }
    else if(materialRecipe.materialShader == WHITESNOW)
    {
        //noiseColor
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/whiteSnow.frag");
        glUseProgram(materialShader);
        
        vec3 modulatedColor = vec3lerp(vec3(0.519,0.518,0.697), vec3(0.0149,0.0118,0.0097), rng01b);
        glUniform3fv(glGetUniformLocation(materialShader, "noiseColor"), 1, value_ptr(modulatedColor));
        
        printf("white snow\n");
    }
    else if(materialRecipe.materialShader == CLIFFFACE)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/cliffface.frag");
        glUseProgram(materialShader);
        
        printf("cliff face\n");
    }
    else if(materialRecipe.materialShader == DIRTANDPEBBLES)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/dirtAndPebbles.frag");
        glUseProgram(materialShader);
        
        //vec3 dirtColor = vec3(0.020,0.009,0.006); //dark brown
        //vec3 pebblesColor = vec3(0.233,0.058,0.033); //mars orange
        
        //dark brown
        //vec3 baseColor = vec3(0.0081,0.0032,0.0026);
        //vec3 modulatedColor = vec3(0.0119,0.0118,0.0097);
        
        
        //random
        vec3 baseColor = vec3lerp(vec3(0.0010,0.0009,0.0006), vec3(0.41,0.32,0.26), rng01a);
        vec3 modulatedColor = vec3lerp(baseColor, vec3(0.0319,0.0318,0.0397), rng01b);
        modulatedColor = vec3lerp(modulatedColor, vec3(0.319,0.318,0.397), (rng01b * rng01a) * 0.8);
        
        glUniform3fv(glGetUniformLocation(materialShader, "dirtColor"), 1, value_ptr(baseColor));
        glUniform3fv(glGetUniformLocation(materialShader, "pebblesColor"), 1, value_ptr(modulatedColor));
        
        printf("dirt and pebbles\n");
        printf("using rng01a : %f & rng01b %f \n", rng01a, rng01b);
    }
    else if(materialRecipe.materialShader == POKEDVORONOI)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/pokedVoronoi.frag");
        glUseProgram(materialShader);
        
        vec3 rockColor = vec3(0.00012,0.00013,0.00023); //near black
        vec3 modulatedColor = vec3lerp(rockColor, vec3(0.065,0.062,0.067), rng01b);

        glUniform3fv(glGetUniformLocation(materialShader, "rockColor"), 1, value_ptr(modulatedColor));
        printf("poked voronoi\n");
        printf("using rng01b %f \n", rng01b);
    }
    else if(materialRecipe.materialShader == ROCKSANDCRATERS)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/rocksAndCraters.frag");
        glUseProgram(materialShader);
        
        vec3 rockColor = vec3(0.131,0.137,0.139); //slightly blue gray
        rockColor = vec3lerp(vec3(0.151,0.157,0.159), vec3(0.48,0.43,0.49), rng01b);

        glUniform3fv(glGetUniformLocation(materialShader, "rockColor"), 1, value_ptr(rockColor));
        printf("rocks and craters\n");
        printf("using rng01b %f \n", rng01b);
    }
    else if(materialRecipe.materialShader == FINEGRAVEL)
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/fineGravel.frag");
        glUseProgram(materialShader);
        
        vec3 mainColor = vec3(2.8,2.4,1.4); //lightish yellow gray
        mainColor = vec3lerp(vec3(0.00012,0.00013,0.00023), mainColor, rng01b);


        glUniform3fv(glGetUniformLocation(materialShader, "mainColor"), 1, value_ptr(mainColor));
        printf("fine gravel\n");
        printf("using rng01b %f \n", rng01b);
    }
    else if(materialRecipe.materialShader == ELONGATEDBLOCKS )
    {
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/elongatedRocks.frag");
        glUseProgram(materialShader);
        
        vec3 mainColor = vec3(0.3,0.34,0.4); //light gray slightly blue
        mainColor = vec3lerp(mainColor, vec3(1.8,1.4,1.0), rng01b); //rng01b
//        printf("\n##############\n");
//        printf("mainColorR %f \n", mainColor.r);
//        printf("mainColorG %f \n", mainColor.g);
//        printf("mainColorB %f \n", mainColor.b);
//        printf("##############\n");

        glUniform3fv(glGetUniformLocation(materialShader, "mainColor"), 1, value_ptr(mainColor));
        printf("elongated rocks\n");
        printf("using rng01b %f \n", rng01b);
    }
    else
    {
        printf("UNKNOWN MATERIAL CODE : %i, DEFAULTING TO CLIFFACE ???\n", materialRecipe.materialShader);
        materialShader = LoadAndLinkShader("passThru.vert", "BaseMapGen/MaterialShaders/cliffface.frag");
    }
    
    
    glUseProgram(materialShader);
    glUniform1i(glGetUniformLocation(materialShader, "seed"),materialRecipe.seed);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *albedoHeightTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *roughnessAOTexture, 0);
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(2, attachments);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0/*mipmap level*/);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0/*mipmap level*/);
    GLuint onlyOneAttachments[1] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  onlyOneAttachments);
    
    glUseProgram(aoShader);
    //glUniform1i(glGetUniformLocation(materialShader, "seed"),materialRecipe.seed);
    glBindTexture(GL_TEXTURE_2D, *albedoHeightTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *roughnessAOTexture, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *roughnessAOTexture, 0);
    //GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(1, onlyOneAttachments);
    
    glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_FALSE); //only write to the ao channel, don't overwrite roughness data

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0/*mipmap level*/);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0/*mipmap level*/);
    //GLuint onlyOneAttachments[1] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  onlyOneAttachments);
    
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //reste mask to default
    
    GLenum err = glGetError();
    if(err != 0)
    printf("Error = %i\n %i", err, GL_INVALID_VALUE);
    assert(err == 0);
}
