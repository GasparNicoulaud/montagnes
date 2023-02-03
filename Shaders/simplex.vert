
#version 140

in vec2 in_Vertex;

vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
    const vec4 C = vec4(0.211324865405187, 0.366025403784439,
                        -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod(i, 289.0);
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
                            dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

void main(void) {
    gl_Position = vec4(in_Vertex, snoise(in_Vertex) , 1.0); //transform to screen space and send out
}

//optimized by mesa :

#version 140
in vec2 in_Vertex;
void main ()
{
    vec3 g_1;
    vec4 x12_2;
    vec2 tmpvar_3;
    tmpvar_3 = floor((in_Vertex + dot (in_Vertex, vec2(0.3660254, 0.3660254))));
    vec2 tmpvar_4;
    tmpvar_4 = ((in_Vertex - tmpvar_3) + dot (tmpvar_3, vec2(0.2113249, 0.2113249)));
    vec2 tmpvar_5;
    if ((tmpvar_4.x > tmpvar_4.y)) {
        tmpvar_5 = vec2(1.0, 0.0);
    } else {
        tmpvar_5 = vec2(0.0, 1.0);
    };
    vec4 tmpvar_6;
    tmpvar_6 = (tmpvar_4.xyxy + vec4(0.2113249, 0.2113249, -0.5773503, -0.5773503));
    x12_2.zw = tmpvar_6.zw;
    x12_2.xy = (tmpvar_6.xy - tmpvar_5);
    vec2 tmpvar_7;
    tmpvar_7 = (vec2(mod (tmpvar_3, 289.0)));
    vec3 tmpvar_8;
    tmpvar_8.xz = vec2(0.0, 1.0);
    tmpvar_8.y = tmpvar_5.y;
    vec3 x_9;
    x_9 = (tmpvar_7.y + tmpvar_8);
    vec3 tmpvar_10;
    tmpvar_10.xz = vec2(0.0, 1.0);
    tmpvar_10.y = tmpvar_5.x;
    vec3 x_11;
    x_11 = (((vec3(mod (
                        (((x_9 * x_9) * 34.0) + x_9)
                        , 289.0))) + tmpvar_7.x) + tmpvar_10);
    vec3 tmpvar_12;
    tmpvar_12.x = dot (tmpvar_4, tmpvar_4);
    tmpvar_12.y = dot (x12_2.xy, x12_2.xy);
    tmpvar_12.z = dot (tmpvar_6.zw, tmpvar_6.zw);
    vec3 tmpvar_13;
    tmpvar_13 = max ((0.5 - tmpvar_12), 0.0);
    vec3 tmpvar_14;
    tmpvar_14 = ((2.0 * fract(
                              ((vec3(mod (((
                                            (x_11 * x_11)
                                            * 34.0) + x_11), 289.0))) * vec3(0.02439024, 0.02439024, 0.02439024))
                              )) - 1.0);
    vec3 tmpvar_15;
    tmpvar_15 = (abs(tmpvar_14) - 0.5);
    vec3 tmpvar_16;
    tmpvar_16 = (tmpvar_14 - floor((tmpvar_14 + 0.5)));
    vec3 tmpvar_17;
    tmpvar_17 = ((tmpvar_13 * tmpvar_13) * (tmpvar_13 * tmpvar_13));
    g_1.x = ((tmpvar_16.x * tmpvar_4.x) + (tmpvar_15.x * tmpvar_4.y));
    g_1.yz = ((tmpvar_16.yz * x12_2.xz) + (tmpvar_15.yz * x12_2.yw));
    vec4 tmpvar_18;
    tmpvar_18.w = 1.0;
    tmpvar_18.xy = in_Vertex;
    tmpvar_18.z = (130.0 * dot ((
                                 (tmpvar_17 * 1.792843)
                                 - 
                                 ((tmpvar_17 * 0.8537347) * ((tmpvar_16 * tmpvar_16) + (tmpvar_15 * tmpvar_15)))
                                 ), g_1));
    gl_Position = tmpvar_18;
}


float optnoise(vec2 v){
    vec3 g_1;
    vec4 x12_2;
    vec2 tmpvar_3  = floor((v + dot (v, vec2(0.3660254, 0.3660254))));
    vec2 tmpvar_4 = ((in_Vertex - tmpvar_3) + dot (tmpvar_3, vec2(0.2113249, 0.2113249)));
    vec2 tmpvar_5;
    if ((tmpvar_4.x > tmpvar_4.y)) {
        tmpvar_5 = vec2(1.0, 0.0);
    } else {
        tmpvar_5 = vec2(0.0, 1.0);
    };
    vec4 tmpvar_6 = (tmpvar_4.xyxy + vec4(0.2113249, 0.2113249, -0.5773503, -0.5773503));
    x12_2.zw = tmpvar_6.zw;
    x12_2.xy = (tmpvar_6.xy - tmpvar_5);
    vec2 tmpvar_7 = (vec2(mod (tmpvar_3, 289.0)));
    vec3 tmpvar_8;
    tmpvar_8.xz = vec2(0.0, 1.0);
    tmpvar_8.y = tmpvar_5.y;
    vec3 x_9 = (tmpvar_7.y + tmpvar_8);
    vec3 tmpvar_10;
    tmpvar_10.xz = vec2(0.0, 1.0);
    tmpvar_10.y = tmpvar_5.x;
    vec3 x_11 = (((vec3(mod ((((x_9 * x_9) * 34.0) + x_9), 289.0))) + tmpvar_7.x) + tmpvar_10);
    vec3 tmpvar_12;
    tmpvar_12.x = dot (tmpvar_4, tmpvar_4);
    tmpvar_12.y = dot (x12_2.xy, x12_2.xy);
    tmpvar_12.z = dot (tmpvar_6.zw, tmpvar_6.zw);
    vec3 tmpvar_13 = max ((0.5 - tmpvar_12), 0.0);
    vec3 tmpvar_14 = ((2.0 * fract(((vec3(mod ((((x_11 * x_11)* 34.0) + x_11), 289.0))) * vec3(0.02439024, 0.02439024, 0.02439024)))) - 1.0);
    vec3 tmpvar_15 = (abs(tmpvar_14) - 0.5);
    vec3 tmpvar_16 = (tmpvar_14 - floor((tmpvar_14 + 0.5)));
    vec3 tmpvar_17 = ((tmpvar_13 * tmpvar_13) * (tmpvar_13 * tmpvar_13));
    g_1.x = ((tmpvar_16.x * tmpvar_4.x) + (tmpvar_15.x * tmpvar_4.y));
    g_1.yz = ((tmpvar_16.yz * x12_2.xz) + (tmpvar_15.yz * x12_2.yw));
    return 130.0 * dot (((tmpvar_17 * 1.792843) - ((tmpvar_17 * 0.8537347) * ((tmpvar_16 * tmpvar_16) + (tmpvar_15 * tmpvar_15)))), g_1);
}


//again ?

float optnoise(vec2 v){
    vec3 g_1;
    vec3 m_2;
    vec4 x12_3;
    vec2 tmpvar_4;
    tmpvar_4 = floor((in_Vertex + dot (in_Vertex, vec2(0.3660254, 0.3660254))));
    vec2 tmpvar_5;
    tmpvar_5 = ((in_Vertex - tmpvar_4) + dot (tmpvar_4, vec2(0.2113249, 0.2113249)));
    vec2 tmpvar_6;
    if ((tmpvar_5.x > tmpvar_5.y)) {
        tmpvar_6 = vec2(1.0, 0.0);
    } else {
        tmpvar_6 = vec2(0.0, 1.0);
    };
    vec4 tmpvar_7;
    tmpvar_7 = (tmpvar_5.xyxy + vec4(0.2113249, 0.2113249, -0.5773503, -0.5773503));
    x12_3.zw = tmpvar_7.zw;
    x12_3.xy = (tmpvar_7.xy - tmpvar_6);
    vec2 tmpvar_8;
    tmpvar_8 = (vec2(mod (tmpvar_4, 289.0)));
    vec3 tmpvar_9;
    tmpvar_9.xz = vec2(0.0, 1.0);
    tmpvar_9.y = tmpvar_6.y;
    vec3 x_10;
    x_10 = (tmpvar_8.y + tmpvar_9);
    vec3 tmpvar_11;
    tmpvar_11.xz = vec2(0.0, 1.0);
    tmpvar_11.y = tmpvar_6.x;
    vec3 x_12;
    x_12 = (((vec3(mod (
                        (((x_10 * 34.0) + 1.0) * x_10)
                        , 289.0))) + tmpvar_8.x) + tmpvar_11);
    vec3 tmpvar_13;
    tmpvar_13.x = dot (tmpvar_5, tmpvar_5);
    tmpvar_13.y = dot (x12_3.xy, x12_3.xy);
    tmpvar_13.z = dot (tmpvar_7.zw, tmpvar_7.zw);
    vec3 tmpvar_14;
    tmpvar_14 = max ((0.5 - tmpvar_13), 0.0);
    m_2 = (tmpvar_14 * tmpvar_14);
    m_2 = (m_2 * m_2);
    vec3 tmpvar_15;
    tmpvar_15 = ((2.0 * fract(
                              ((vec3(mod (((
                                            (x_12 * 34.0)
                                            + 1.0) * x_12), 289.0))) * vec3(0.02439024, 0.02439024, 0.02439024))
                              )) - 1.0);
    vec3 tmpvar_16;
    tmpvar_16 = (abs(tmpvar_15) - 0.5);
    vec3 tmpvar_17;
    tmpvar_17 = (tmpvar_15 - floor((tmpvar_15 + 0.5)));
    m_2 = (m_2 * (1.792843 - (0.8537347 *
                              ((tmpvar_17 * tmpvar_17) + (tmpvar_16 * tmpvar_16))
                              )));
    g_1.x = ((tmpvar_17.x * tmpvar_5.x) + (tmpvar_16.x * tmpvar_5.y));
    g_1.yz = ((tmpvar_17.yz * x12_3.xz) + (tmpvar_16.yz * x12_3.yw));
    return 130.0 * dot (m_2, g_1);
}
