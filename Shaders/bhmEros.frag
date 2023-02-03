
#version 150 core

out vec4 out_Color;

uniform sampler2D sampler;
uniform sampler2D roughnessMap;

float inflow(vec2 neighbor, vec2 neighborflow)
{
    return max(dot(neighbor * -1, neighborflow),0.001); //last parameter control the minimun slope at which roughness controlled erosion pattern appears (don't know why)
}

void main(void)
{
    vec2 normFragCoord = gl_FragCoord.xy * 0.00048828125; //   =  1 / 2048
    
    float roughness = texture(roughnessMap,normFragCoord).r;
    
    vec3 a = vec3(-1,0,1);
    
    vec3 us = texture(sampler,normFragCoord).rgb;
    
    float layerErosionFactor = 1;//fract(us.r * 5 + roughness * 0.05) + 0.5; //hacky terassing
    
    vec2 neighbourDirs[8]; //neighbour coordinates relative to us (i.e 0,1 1,0 -1,0 etc....)
    vec3 neigbourhs [8]; //neighbor xyz
    float inFlow [8]; //only positive values
    //float thermGain[8];
    
    neighbourDirs[0] = a.yz;
    neighbourDirs[1] = a.zy;
    neighbourDirs[2] = a.xy;
    neighbourDirs[3] = a.yx;
    
    neighbourDirs[4] = a.xz;
    neighbourDirs[5] = a.xx;
    neighbourDirs[6] = a.zx;
    neighbourDirs[7] = a.zz;
    
    for(int i = 0; i < 8; i++)
    {
        neigbourhs[i] = texture(sampler,normFragCoord + neighbourDirs[i] * 0.00048828125).rgb; //2048 because hardcoded fbo res is 2048x2048
        inFlow[i] = min(inflow(neighbourDirs[i],neigbourhs[i].yz),1);
        //thermGain[i] = (neigbourhs[i].x - us.x) / length(neighbourDirs[i]);
    }

    float gainGain = 0;
    float gainLoss = 0;
    float totalInflow = 0;
    float thermGain = 0;
    
    //us.yz *= 0.2; //set our flow to 0
    
    for(int i = 0; i < 8; i++)
    {
        thermGain = (neigbourhs[i].x - us.x) / length(neighbourDirs[i]);
        totalInflow += inFlow[i] * 5;
        us.yz += neighbourDirs[i] * thermGain * -0.2;
        gainGain += thermGain * 1.2;
    }
    
    gainGain += totalInflow * totalInflow * totalInflow;
    
    us.yz *= 0.5;// + totalInflow;
    
    gainLoss -= length(us.yz) * 1.5;
    
    float finalGain = (gainLoss + gainGain) * 0.1 * clamp(layerErosionFactor,0.6,1.1);

    if(finalGain < 0)
    {
        finalGain *= max(roughness, 0);
    }
    //float x = 1 when finalGain < 0 , 0 if finalGain > 0
    //finalGain *= max(roughness * (1 - max(ceil(finalGain),0)),1);// * (-gainLoss + 1);

    
    us.x += clamp(finalGain, -0.025, 0.025);
    
    out_Color = vec4(us.r,us.g,us.b,1);
}

