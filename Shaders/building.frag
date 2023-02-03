#version 150 core

out vec4 out_Color;

in vec3 inoutpos;

void main()
{
    out_Color = vec4(0.98, 0.92, 0.84, 1.0); // beige because haussmanian building
    
    // slightly darker grid 1 x 1 meter grid
    if(abs(mod(inoutpos.x,1.0)) < 0.1)out_Color = vec4(0.88, 0.82, 0.74, 1.0);
    if(abs(mod(inoutpos.y,1.0)) < 0.1)out_Color = vec4(0.88, 0.82, 0.74, 1.0);
    if(abs(mod(inoutpos.z,1.0)) < 0.1)out_Color = vec4(0.88, 0.82, 0.74, 1.0);
    
    // gray grid 5 x 5 meter grid
    if(abs(mod(inoutpos.x,5.0)) < 0.2)out_Color = vec4(0.5, 0.5, 0.5, 1.0);
    if(abs(mod(inoutpos.y,5.0)) < 0.2)out_Color = vec4(0.5, 0.5, 0.5, 1.0);
    if(abs(mod(inoutpos.z,5.0)) < 0.2)out_Color = vec4(0.5, 0.5, 0.5, 1.0);
    
    //out_Color = vec4(abs(mod(inoutpos.x,1.0)),abs(mod(inoutpos.y,1.0)),abs(mod(inoutpos.z,1.0)), 1.0);
}