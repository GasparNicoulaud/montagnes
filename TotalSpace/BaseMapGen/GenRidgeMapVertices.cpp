//
//  GenRidgeMapVertices.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 10/06/2019.
//  Copyright © 2019 Gaspar Nicoulaud. All rights reserved.
//

#include "GenRidgeMapVertices.hpp"
#include <math.h>

#define BOUNDING_BOX_SIZE 1.2 //its really bounding box radius as the normal box is [-1 , 1]

float frandLocal(float max)
{
    return ((float)(rand()%100000) / 100000.0) * max;
}


struct Node
{
    int connection1 = -1;
    int connection2 = -1;
    int connection3 = -1;
    float x = 0,y = 0,z = 0;
    bool exists = false;
};

Node * nodes;

float sq(float val)
{
    return val*val;
}

float node2Ddistance(Node one, Node two)
{
    return sqrt(sq(one.x-two.x)+sq(one.y-two.y));
}

double dot_product(float x1,float y1,float x2,float y2)
{
    double result = 0.0;
    result += x1*x2;
    result += y1*y2;
    return result;
}

bool checkNodeAngles(Node root, Node candidate)
{
    float angleThreshold = 0.0;
    bool badAngle = false;
    if(root.connection1 != -1)
    {
        float dotProd = dot_product(candidate.x - root.x,candidate.y - root.y,nodes[root.connection1].x - root.x,nodes[root.connection1].y - root.y);
        if(dotProd > angleThreshold)
        {
            badAngle = true;
        }
    }
    if(root.connection2 != -1)
    {
        float dotProd = dot_product(candidate.x - root.x,candidate.y - root.y,nodes[root.connection2].x - root.x,nodes[root.connection2].y - root.y);
        if(dotProd > angleThreshold)
        {
            badAngle = true;
        }
    }
    if(root.connection3 != -1)
    {
        float dotProd = dot_product(candidate.x - root.x,candidate.y - root.y,nodes[root.connection3].x - root.x,nodes[root.connection3].y - root.y);
        if(dotProd > angleThreshold)
        {
            badAngle = true;
        }
    }
    return !badAngle;
}

void genRigdeMapVertices(float *ridgeVertices, int maxRidgeVertexCount, int generationIterationCount, int ridgeSeedCount, int seed, float scale, float exclusionZoneRadius, float seedStartingHeight)
{
    assert(ridgeSeedCount < maxRidgeVertexCount);
    nodes = (Node*)malloc((maxRidgeVertexCount)*sizeof(Node));
    //init the nodes
    for(int i = 0; i < maxRidgeVertexCount; i++)
    {
        nodes[i].connection1 = -1;
        nodes[i].connection2 = -1;
        nodes[i].connection3 = -1;
        nodes[i].x = 0;
        nodes[i].y = 0;
        nodes[i].z = 0;
        nodes[i].exists = false;
    }
    int nodesCarret = 0; //should always be the index to the first available node
    srand(seed);
    
    //add several starting point each at a minimum large dist of others, give them high but random height
    int seedCount = 0;
    int seedPlacementTryCount = 0;
    while(seedCount < ridgeSeedCount && seedPlacementTryCount < ridgeSeedCount *20)
    {
        Node candidate; //seed candidate, give it random xy value in [-BOUNDING_BOX_SIZE , BOUNDING_BOX_SIZE] range
        candidate.x = frandLocal(BOUNDING_BOX_SIZE * 2) - BOUNDING_BOX_SIZE;
        candidate.y = frandLocal(BOUNDING_BOX_SIZE * 2) - BOUNDING_BOX_SIZE;
        
        /*if(seedCount == 0) // harcoded but should be removed, only for test
        {
            candidate.x = 0;
            candidate.y = 0;
        }*/
        
        
        //check collsion with other seed
        bool collision = false;
        for(int i = 0; i < maxRidgeVertexCount; i++)
        {
            if(nodes[i].exists && node2Ddistance(candidate, nodes[i]) < 0.2)
            {
                collision = true;
                break;
            }
        }
        
        //try putting the seed systematically on the sides
        //float exclusionZoneRadius = 0.9;
        if(candidate.x > -exclusionZoneRadius && candidate.x < exclusionZoneRadius && candidate.y > -exclusionZoneRadius && candidate.y < exclusionZoneRadius)
        {
            collision = true;
        }
        
        //if no collsion, add it to the list
        if(!collision)
        {
            nodes[nodesCarret] = candidate;
            //constant seed height
            //nodes[nodesCarret].z = 1.0;

            //random seed height
            nodes[nodesCarret].z = frandLocal(0.2 * seedStartingHeight) + 0.8 * seedStartingHeight;
            nodes[nodesCarret].exists = true;
            nodes[nodesCarret].connection1 = -1;
            nodes[nodesCarret].connection2 = -1;
            nodes[nodesCarret].connection3 = -1;
            
            nodesCarret++;
            seedCount++;
        }
        
        seedPlacementTryCount++;
    }
    
    printf("seed node placed = %i, tried %i times\n",seedCount,seedPlacementTryCount);
    //assert(seedCount == ridgeSeedCount);
    
     /*
     for each node,
     if it has less than 2 conncetions
     try to add a node at a Radius dist and random angle,
     if new node is not too close to any other node &&
     if the ange it makes with other connections are never <90° (so if dot product is negative)
     then add it as connected to the parent node, and give it lower but random height
     */
    
    // now the adding of nodes
    int nodePlacementTryCount = 0;
    float argScale = scale;
    while(nodesCarret < maxRidgeVertexCount && nodePlacementTryCount < generationIterationCount)
    {
        float scaleRatio = 1.0;
        /*if(nodesCarret >= maxRidgeVertexCount * 0.8 || nodePlacementTryCount >= generationIterationCount * 0.8 )
        {
            scaleRatio = 0.6;
        }
        else
        {
            scaleRatio = 1.0;
        }*/
        scale = argScale * scaleRatio;
        //randomly choose a node to try and spawn a child node
        int i = rand()%nodesCarret;
        // check that it is suitable : at least one connection free, might be optimized by only checking connection3, as it is the last to be filled
        if((nodes[i].connection1 == -1 || nodes[i].connection2 == -1 || nodes[i].connection3 == -1) && nodes[i].z > 0)
        {
            assert(nodes[i].exists); //because i is chosen to be smaller than nodesCarret, node[i] should always exist
            Node candidate; //the candidate node child to add
            float rngAngle = frandLocal(2*3.1415);
            //float nodeDistance = 0.1;
            //float scale = 2.0;
            float nodeDistance = 0.05 * scale + frandLocal(0.02 * scale);
            candidate.x = sin(rngAngle) * nodeDistance + nodes[i].x;
            candidate.y = cos(rngAngle) * nodeDistance + nodes[i].y;
                
            //check if its too close with any other node or outside the box
            bool collision = false;
            if(candidate.x > BOUNDING_BOX_SIZE || candidate.x < -BOUNDING_BOX_SIZE || candidate.y > BOUNDING_BOX_SIZE || candidate.y < -BOUNDING_BOX_SIZE)
            {
                collision = true;
            }
            else
            {
                for(int j = 0; j < maxRidgeVertexCount; j++)
                {
                    if(i != j && nodes[j].exists && node2Ddistance(candidate, nodes[j]) < nodeDistance * 1.5)
                    {
                        collision = true;
                        break;
                    }
                }
            }
            
            //if the candidate is suitable, add it to the nodes[] list
            if(!collision && checkNodeAngles(nodes[i],candidate))
            {
                nodes[nodesCarret] = candidate;
                //random attenuation
                nodes[nodesCarret].z = nodes[i].z - (0.04 + (frandLocal(0.04) - 0.02)) * nodeDistance * 12;
                
                //no attenuation
                //nodes[nodesCarret].z = nodes[i].z;

                nodes[nodesCarret].exists = true;
                nodes[nodesCarret].connection1 = -1;
                nodes[nodesCarret].connection2 = -1;
                nodes[nodesCarret].connection3 = -1;
                
                nodes[nodesCarret].connection1 = i;
                    
                if(nodes[i].connection1 == -1)
                {
                    nodes[i].connection1 = nodesCarret;
                }
                else if(nodes[i].connection2 == -1)
                {
                    nodes[i].connection2 = nodesCarret;
                }
                else if(nodes[i].connection3 == -1)
                {
                    nodes[i].connection3 = nodesCarret;
                }
                
                nodesCarret ++;
            }
            
        }
        nodePlacementTryCount ++;
    }
    
    printf("nodes placed (not including seeds) = %i, tried %i times\n",nodesCarret - seedCount,nodePlacementTryCount);
    int ridgeVerticesCarret = 0;
    
    assert(nodesCarret <= maxRidgeVertexCount);
    
    //set nodes that have only one connection to 'height = 0'
    for(int i = 0; i < nodesCarret; i++)
    {
        if(nodes[i].exists)
        {
            int connectionCount = (nodes[i].connection1 != -1) + (nodes[i].connection2 != -1) + (nodes[i].connection3 != -1);
            if(connectionCount <= 1)
            {
                nodes[i].z = 0;
            }
        }
    }
    
    //convert from nodes to float array
    for(int i = 0; i < nodesCarret; i++)
    {
        //each node has 3 connections (max), each connection means 2 vertices, each vertex has 3 propertys (xyz)
        if(nodes[i].connection1 != -1)
        {
            ridgeVertices[ridgeVerticesCarret] = nodes[i].x;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[i].y;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[i].z;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection1].x;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection1].y;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection1].z;
            ridgeVerticesCarret++;
        }
        if(nodes[i].connection2 != -1)
        {
            ridgeVertices[ridgeVerticesCarret] = nodes[i].x;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[i].y;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[i].z;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection2].x;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection2].y;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection2].z;
            ridgeVerticesCarret++;
        }
        if(nodes[i].connection3 != -1)
        {
            ridgeVertices[ridgeVerticesCarret] = nodes[i].x;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[i].y;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[i].z;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection3].x;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection3].y;
            ridgeVerticesCarret++;
            ridgeVertices[ridgeVerticesCarret] = nodes[nodes[i].connection3].z;
            ridgeVerticesCarret++;
        }
        //each node has 3 connections (max), each connection means 2 vertices, each vertex has 3 propertys (xyz)
        assert(ridgeVerticesCarret <= maxRidgeVertexCount * 3 * 2 * 3);
    }
    
    free(nodes);
    
    //fill the rest of the array with vertex values that will put them out of frame
    for(int i = ridgeVerticesCarret; i < maxRidgeVertexCount * 3 * 2 * 3; i++)
    {
        ridgeVertices[i] = -10.0; //sets the rest to all -10 (x y and z) so when we render it into bhm it's out of view
    }
}
