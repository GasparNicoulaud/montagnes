
#version 150 core

out vec4 out_Color;

uniform sampler2D sampler;

float computeGainLoss(float mm, float neighbor, float dst)
{
    float talusRate = 10/2048.0 * max((pow(mm + 0.3,2)),0);// * 0.2;
    
    float retval = 0;
    
    if(abs(mm - neighbor) > talusRate * dst)
    {
        retval += (neighbor - mm) / 10.0;
    }
    
    return retval * 1.0f/dst;
}

void main(void)
{
    float tl, tm, tr, ml, mm, mr, bl, bm, br;
    
    float finalGain = 0;
    
    //check neigbourhs
    tl = texture(sampler,vec2((gl_FragCoord.x - 1)/2048.0,(gl_FragCoord.y + 1)/2048.0)).x; //2048 because hardcoded fbo res is 2048x2048
    tm = texture(sampler,vec2((gl_FragCoord.x    )/2048.0,(gl_FragCoord.y + 1)/2048.0)).x;
    tr = texture(sampler,vec2((gl_FragCoord.x + 1)/2048.0,(gl_FragCoord.y + 1)/2048.0)).x;
    
    ml = texture(sampler,vec2((gl_FragCoord.x - 1)/2048.0,(gl_FragCoord.y    )/2048.0)).x;
    mm = texture(sampler,vec2((gl_FragCoord.x    )/2048.0,(gl_FragCoord.y    )/2048.0)).x;
    mr = texture(sampler,vec2((gl_FragCoord.x + 1)/2048.0,(gl_FragCoord.y    )/2048.0)).x;
    
    bl = texture(sampler,vec2((gl_FragCoord.x - 1)/2048.0,(gl_FragCoord.y - 1)/2048.0)).x;
    bm = texture(sampler,vec2((gl_FragCoord.x    )/2048.0,(gl_FragCoord.y - 1)/2048.0)).x;
    br = texture(sampler,vec2((gl_FragCoord.x + 1)/2048.0,(gl_FragCoord.y - 1)/2048.0)).x;
    
    
    // for each that is higher than talus, compute gain
    // for each that is lower than talus, compute loss
    //corners first
    finalGain += computeGainLoss(mm,tl,1.4142);
    finalGain += computeGainLoss(mm,tr,1.4142);
    finalGain += computeGainLoss(mm,bl,1.4142);
    finalGain += computeGainLoss(mm,br,1.4142);

    // then the center cross
    finalGain += computeGainLoss(mm,tm,1);
    finalGain += computeGainLoss(mm,ml,1);
    finalGain += computeGainLoss(mm,mr,1);
    finalGain += computeGainLoss(mm,bm,1);
    //add gain and loss
    
    
    float outc = mm + finalGain;
    //outc += 0.1;
    out_Color = vec4(outc,0,0,1);
}

