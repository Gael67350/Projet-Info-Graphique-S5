#version 130
precision mediump float;

//color information input
varying vec4 varyColor; //Sometimes we use "out" instead of "varying". "out" should be used in later version of GLSL.

void main()
{
    gl_FragColor = varyColor;
}
