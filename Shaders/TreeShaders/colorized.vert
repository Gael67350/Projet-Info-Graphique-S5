#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

uniform mat4 uMVP;

attribute vec3 vPosition;
attribute vec3 vColor;

varying vec4 varyColor;

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main() {
    gl_Position = uMVP * vec4(vPosition, 1.0);

    varyColor = vec4(vColor, 1.0);
}