//
//  RecipeGenerator.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 9/13/17.
//  Copyright © 2017 Gaspar Nicoulaud. All rights reserved.
//

#include "RecipeGenerator.hpp"

float randFloatZeroToOne(){
    return (rand() % 10000) / 10000.0f;
}

MaterialShaders randomMaterialFrom(MaterialShaders choices[]){
    return choices[rand() % sizeof(choices) / sizeof(choices[0])];
}

Recipe genRecipe()
{
    Recipe recipe;
    
    /* initialize random seed: */
    srand ((int)time(NULL));
    
    recipe.seed = rand() % 1000000;
    
    /*
     enum MainShape{
         CANYONS,
         CLAWNOISE,
         RIDGENETWORK,
         SQRTPLAINS,
         BOULDERS,
         EXPDISTNOISE,
         SMALLPEAKS,
         PUREPERLIN,
         CLIFFSCALES,
         COMBINEDVORONOIS
     };
     */
    int numOfPossibleShapes = 10;
    recipe.mainShape = (MainShape)(rand() % numOfPossibleShapes);
    //recipe.mainShape = SQRTPLAINS;
    //recipe.seed = 12345; // seed fixing
//    recipe.baseNoiseFactor = 2;//(rand() % 10000) / 5000.0f;
//    recipe.erosionFactor = 0.5;
    
    //looks good from [0.2 , 10] but should be skew toward lower values
    recipe.clawNoiseParameters.frequency = std::min( 1.0f/randFloatZeroToOne() - 0.8f ,10.0f);
    recipe.clawNoiseParameters.amplitude = randFloatZeroToOne() * 1.3 + 0.2; //look good between [0.2 , 1.5]
    
    recipe.sqrtPlainsParameters.frequency = std::min( (1.0f/randFloatZeroToOne() - 0.5f) * 0.5f ,10.0f);
    recipe.sqrtPlainsParameters.amplitude = (randFloatZeroToOne() * 1.3 + 0.2 ) * (3.0f/recipe.sqrtPlainsParameters.frequency);
    recipe.sqrtPlainsParameters.amplitude = sqrt(recipe.sqrtPlainsParameters.amplitude);
    
    recipe.canyonsParameters.frequency = std::min( (1.0f/randFloatZeroToOne() - 0.8f) * 2.0f ,8.0f);
    recipe.canyonsParameters.amplitude = (randFloatZeroToOne() * 1.3 + 0.2 ) * (3.0f/(recipe.canyonsParameters.frequency + 1.0));
    
    recipe.bouldersParameters.frequency = std::min( 1.0f/randFloatZeroToOne() ,8.0f);
    recipe.bouldersParameters.amplitude = (randFloatZeroToOne() * 0.5 + 0.5 ) * (3.0f/(recipe.bouldersParameters.frequency + 1.0));
    
    recipe.smallPeaksParameters.frequency = std::min( 1.0f/randFloatZeroToOne() ,8.0f);
    recipe.smallPeaksParameters.amplitude = (randFloatZeroToOne() * 0.5 + 0.5 ) * (3.0f/(recipe.smallPeaksParameters.frequency + 1.0));
    
    recipe.purePerlinParameters.frequency = randFloatZeroToOne() + randFloatZeroToOne();
    recipe.purePerlinParameters.amplitude = (randFloatZeroToOne() * 0.5 + 0.75 ) * (3.0f/(recipe.purePerlinParameters.frequency + 1.0));
    
    recipe.cliffScalesParameters.frequency = randFloatZeroToOne() + randFloatZeroToOne();
    recipe.cliffScalesParameters.amplitude = (randFloatZeroToOne() * 0.5 + 0.75 ) * (1.0f/recipe.cliffScalesParameters.frequency);
    
    recipe.combinedVoronoisParameters.frequency = randFloatZeroToOne() + randFloatZeroToOne();
    recipe.combinedVoronoisParameters.amplitude = (randFloatZeroToOne() * 0.5 + 0.75 ) * (1.0f/recipe.combinedVoronoisParameters.frequency);
    
    recipe.quilezParameters.frequency = randFloatZeroToOne() + randFloatZeroToOne();
    recipe.quilezParameters.amplitude = (randFloatZeroToOne() * 0.5 + 0.75 ) * (1.0f/recipe.quilezParameters.frequency);
    
    recipe.applyTerracing = randFloatZeroToOne() > 0.5;
    recipe.teraccingAmount = 5.0 + randFloatZeroToOne() * randFloatZeroToOne() * 25.0;
    
    // Perlin Warping Factor -------
    recipe.perlinDisplaceStrenth = randFloatZeroToOne();
    if(recipe.mainShape == RIDGENETWORK || recipe.mainShape == SQRTPLAINS)
    {
        recipe.perlinDisplaceStrenth = 0.5 + randFloatZeroToOne() * 0.5;
    }
    recipe.perlinDisplaceStrenth *= 0.3;
    
    recipe.firstperlinDisplaceStrenth = randFloatZeroToOne();
    
    recipe.doClayJohnFilter = rand() % 2 == 0;
    
    recipe.boeschErosionIterations = floor((randFloatZeroToOne() * randFloatZeroToOne()) * 500.0) * floor(randFloatZeroToOne() + randFloatZeroToOne());
    
    // Flowmap Carving intensity ---
    recipe.flowmapCarvingStrength = randFloatZeroToOne() * randFloatZeroToOne();
    
    if(rand() % 2 == 0) //add craters to the recipe ? should tie into atomsphere presence
    {
        recipe.oldCratersParameters.amplitude = randFloatZeroToOne() * 4.0 + 1.0;
        recipe.oldCratersParameters.frequency = (1.0/recipe.oldCratersParameters.amplitude) * 2;
        
        recipe.newCratersParameters.amplitude = (randFloatZeroToOne() * recipe.oldCratersParameters.amplitude) * 0.5 + 1.0;
        recipe.newCratersParameters.frequency = (1.0/recipe.newCratersParameters.amplitude) * 2.5;
    }
    else
    {
        recipe.oldCratersParameters.amplitude = 0;
        recipe.oldCratersParameters.frequency = 0;
        recipe.newCratersParameters.amplitude = 0;
        recipe.newCratersParameters.frequency = 0;
    }
    
    recipe.ridgeMapRecipe.scale = 1.0 + randFloatZeroToOne() * 3.0; //scale looks good from 1.0 to 4.0
    recipe.ridgeMapRecipe.seedCount = (int)(randFloatZeroToOne() * 15) + 1;
    recipe.ridgeMapRecipe.seedStartingHeight = 0.5 + randFloatZeroToOne();
    recipe.ridgeMapRecipe.exclusionZoneRadius = 1.1 * randFloatZeroToOne() * randFloatZeroToOne();
    
    //higher values means sharper slopes
    recipe.pyramidizeFactor = randFloatZeroToOne() * 0.004 + 0.001;
    
    recipe.flipAfterMainShape = 0.0;
    if(rand() % 2 == 0)
    {
        recipe.flipAfterMainShape = 1.0;
    }
    
    recipe.flipAfterEverything = 0.0;
    if(rand() % 2 == 0)
    {
        recipe.flipAfterEverything = 1.0;
    }
    
    //between [0 , 5.0], high values lower the blur layer, around 2.0 it goes completely under the terrain
    recipe.firstBlurMaxStep = randFloatZeroToOne() * 5;
    
    //between [0.0 , 3.0], high values lower the blur layer, around 2.0 it goes completely under the terrain
    recipe.finalBlurMaxStep = randFloatZeroToOne() * 3;
    
    recipe.midBlurMaxTerraceFreq = randFloatZeroToOne() * 30.0;
    
    recipe.jakoV2Steps = (int)(randFloatZeroToOne() * randFloatZeroToOne() * 1500);
    if(randFloatZeroToOne() < 0.3)
    {
        recipe.jakoV2Steps = 0;
    }
    recipe.jako_rain_rate = (randFloatZeroToOne() * 9) + 1; // [1 - 10]
    
    recipe.PreWMTerraceFreq = randFloatZeroToOne() * 30.0;
    recipe.WMSpecialIterations = randFloatZeroToOne() * 64;
    recipe.WMSpecialSedimentation = randFloatZeroToOne();
    recipe.PostWMSpecialTalusAngle = 1.0 - randFloatZeroToOne() * randFloatZeroToOne();
    
    recipe.doWMerosion = randFloatZeroToOne() > 0.66;
    recipe.JakoTerraceFreq = recipe.doWMerosion ? randFloatZeroToOne() * 30.0 : 0;
    
    recipe.doGeoglicalEnhancement = recipe.doWMerosion && (randFloatZeroToOne() > 0.5);
    recipe.WMsedimentation = randFloatZeroToOne() * randFloatZeroToOne() * 2.0;
    recipe.doWMMicroErode = randFloatZeroToOne() > 0.5;
    recipe.WMTerraceFreq = randFloatZeroToOne() * 30.0;
    recipe.WMThermalIterations = randFloatZeroToOne() * 3000;
    recipe.postWMBlurMaxOffset = 0.05 + 0.2 * randFloatZeroToOne();
    
    recipe.finalTalusCarveSlope = 1 - (randFloatZeroToOne() * randFloatZeroToOne() * randFloatZeroToOne());
        
//    recipe.doWMerosion = false;
//    recipe.WMsedimentation = 0;
//    recipe.jakoV2Steps = 0;
//    recipe.doWMMicroErode = false;
//    recipe.WMSpecialIterations = 0;
//    recipe.doGeoglicalEnhancement = false;
//    recipe.WMThermalIterations = 0;
//    
////    recipe.WMsedimentation = 2.0;
////    recipe.doWMerosion = true;
//    
//    recipe.WMSpecialIterations = 64;
//    recipe.WMSpecialSedimentation = 0;
    
    /*
    enum MaterialShaders{
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
    };
    */
    int numOfPossibleMaterials = 12;
    
    recipe.stoneRecipe.seed = rand() % 1000000;
    recipe.stoneRecipe.materialShader = randomMaterialFrom((MaterialShaders[]){CLIFFFACE, SMOOTHSTONE, POKEDVORONOI, ROUGHROCK});
//    recipe.stoneRecipe.materialShader = (MaterialShaders)(rand() % numOfPossibleMaterials);
//    recipe.stoneRecipe.materialShader = ROUGHROCK;
    
    recipe.rocksRecipe.seed = rand() % 1000000;
    recipe.rocksRecipe.materialShader = (MaterialShaders)(rand() % numOfPossibleMaterials);
//    recipe.rocksRecipe.materialShader = ROUGHROCK;
    
    recipe.sandRecipe.seed = rand() % 1000000;
    recipe.sandRecipe.materialShader = randomMaterialFrom((MaterialShaders[]){JAGGEDROCKS, VORONOIROCKFIELD, WHITESNOW, DIRTANDPEBBLES, FINEGRAVEL});
//    recipe.sandRecipe.materialShader = ROUGHROCK;
    
    recipe.flowRecipe.seed = rand() % 1000000;
    recipe.flowRecipe.materialShader = (MaterialShaders)(rand() % numOfPossibleMaterials);
//    recipe.flowRecipe.materialShader = ROUGHROCK;
    
    
//    if(
//       recipe.stoneRecipe.materialShader != ROUGHROCK &&
//       recipe.rocksRecipe.materialShader != ROUGHROCK &&
//       recipe.sandRecipe.materialShader != ROUGHROCK &&
//       recipe.flowRecipe.materialShader != ROUGHROCK
//       ){
//           printf("exiting at recipe because didnt generate material we want.\n");
//           exit(0);
//    }
    
    //___________sun position in the sky___________ :
    
    //currently not ideal, as x and y are independant, it would be better to choose an ange from 0° to 360° and then compute x and y
    recipe.sunPosX = randFloatZeroToOne() * 2 - 1.0;
    recipe.sunPosY = randFloatZeroToOne() * 2 - 1.0;
    recipe.sunPosZ = 0.1 + randFloatZeroToOne() * 0.3; //make sun never too close to zenith
    //recipe.sunPosZ = 0;
    
    //recipe.sunPosZ = 0.8;
    
    //normalize sun position
    float sunPosLength = sqrt(recipe.sunPosX*recipe.sunPosX + recipe.sunPosY*recipe.sunPosY + recipe.sunPosZ*recipe.sunPosZ);
    recipe.sunPosX /= sunPosLength;
    recipe.sunPosY /= sunPosLength;
    recipe.sunPosZ /= sunPosLength;
    
    /*if(randFloatZeroToOne() > 0.5)
    {
        recipe.horizontalDisplacementCoeff = 1.0;
    }
    else
    {
        recipe.horizontalDisplacementCoeff = -1.0;
    }*/
    recipe.horizontalDisplacementCoeff = 1.0; // -1 looks weird so always 1
    
    /*
    recipe.mainShape = PUREPERLIN;
    recipe.flipAfterMainShape = 0;
    recipe.flipAfterEverything = 0;
    */
    
    /*
    recipe.stoneRecipe.materialShader = POKEDVORONOI;
    //recipe.rocksRecipe.materialShader = ELONGATEDBLOCKS;
    recipe.sandRecipe.materialShader = WHITESNOW;
    //recipe.flowRecipe.materialShader = FINEGRAVEL;
    recipe.doClayJohnFilter = true;
    recipe.mainShape = PUREPERLIN;
    */
    //recipe.mainShape = PUREPERLIN;
    
    //recipe.flowmapCarvingStrength = 0;
    //recipe.flipAfterMainShape = 0;
    //recipe.flipAfterEverything = 0;
    //recipe.jakoV2Steps = 0;
    //recipe.mainShape = BOULDERS;
    
    return recipe;
}

void saveRecipe(Recipe r)
{
    ofstream file;
    file.open ("/Users/gaspar/Prog/TotalSpace/maps/map.txt");
    file.write((char *)&r,sizeof(Recipe));
    file.close();
    printf("Saved map recipe\n");
}


void loapRecipe(Game * game)
{
    ifstream in("/Users/gaspar/Prog/TotalSpace/maps/map.txt",ios_base::binary);
    in.read((char *)&game->battleMap.recipe,sizeof(Recipe));
    in.close();
}
