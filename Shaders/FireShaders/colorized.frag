#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

//color information input
varying vec4 varyColor;

void main()
{
    gl_FragColor = varyColor;
}
