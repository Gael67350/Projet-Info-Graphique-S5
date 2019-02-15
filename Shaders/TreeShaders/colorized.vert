#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

uniform mat4 uMVP;
uniform mat4 uModelViewMatrix;
uniform mat4 uInvModelViewMatrix;

attribute vec3 vPosition;
attribute vec3 vColor;
attribute vec3 vNormal;

varying vec4 varyPosition;
varying vec4 varyColor;
varying vec3 varyNormal;

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main() {
    gl_Position = uMVP * vec4(vPosition, 1.0);

    varyColor = vec4(vColor, 1.0);
	
	varyNormal = normalize(mat3(uInvModelViewMatrix) * vNormal);
	
	varyPosition = uModelViewMatrix * vec4(vPosition, 1.0);
	varyPosition = varyPosition / varyPosition.w;
}