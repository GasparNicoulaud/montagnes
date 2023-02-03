#version 150 core

uniform sampler2D sampler;
out uvec4 out_Color;

float julia(vec2 p)
{
    vec2 c = vec2(0.001,0.96); //vec2(-0.12,0.89);
    float seuil = 5;
    int imax = 50;
    vec2 tmp = p;
    
    for( int i=0; i<imax; i++ )
    {
        tmp = vec2(tmp.x * tmp.x - tmp.y * tmp.y + c.x, 2 * tmp.x * tmp.y + c.y);
        
        if(length(tmp) > seuil)
        {
            return i;
        }
    }
    
    return imax;
}

void main(void)
{
    vec4 terrain = texture(sampler,gl_FragCoord.xy/2048.0f).rgba;
    float height = texture(sampler,gl_FragCoord.xy/2048.0f).r;
    terrain.r += 2;
    terrain.r *= 10000;
    
    terrain.g = 500;// - max(0,terrain.r - 35000); //add of sediment to initalze it default : 800 // reset to 200 for jakoOld
    terrain.b = 100; // water, lets put 10 meters
    //terrain.a = julia((gl_FragCoord.xy - vec2(1024)) / 500.0) * 20; // julia set;
    //terrain.a = (sin(gl_FragCoord.x * 0.1) + sin(gl_FragCoord.y * 0.1) + 2)  * 20   + 80;
    terrain.a = 200 + height * 0.5; //hardness
    //terrain.a = gl_FragCoord.x * 0.2; //hardness
    out_Color = uvec4(terrain);
}

