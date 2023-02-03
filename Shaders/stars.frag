#version 150 core

in vec3 inoutColor;
in vec2 inoutUV;
//in vec3 inoutPos;

out vec4 out_Color;

void main(void)
{
    float lengthUV = length(inoutUV);
    
    out_Color = vec4(inoutColor, //rgb star color
                     0.2/lengthUV - lengthUV*0.0002 //alpha value
                     );
}
















//more ?paddinggg
