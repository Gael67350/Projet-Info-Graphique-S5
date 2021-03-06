#version 130
precision mediump float;

attribute vec3 vPosition;
uniform mat4 uMvp;

//uvValues
attribute vec2 vUV;

//Color information for fragment shader
uniform vec4 uColor;
varying vec4 varyColor;

void main()
{
    gl_Position = uMvp*vec4(vPosition, 1.0); 
    varyColor = uColor;
}
