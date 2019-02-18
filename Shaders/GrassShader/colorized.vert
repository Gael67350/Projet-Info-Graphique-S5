#version 130
precision mediump float;

attribute vec3 vPosition;
uniform mat4 uMvp;

//normal data as it is in the vbo data
attribute vec3 vNormals;

//Color information for fragment shader
uniform vec4 uColor;
varying vec4 varyColor;

void main()
{
    gl_Position = uMvp*vec4(vPosition, 1.0); 
    varyColor = uColor;
}
