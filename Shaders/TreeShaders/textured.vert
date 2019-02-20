#version 130
precision mediump float;

uniform mat4 uMVP;
uniform mat4 uModelViewMatrix;
uniform mat4 uInvModelViewMatrix;

attribute vec3 vPosition; //Depending who compiles, these variables are not "attribute" but "in". In this version (130) both are accepted. in should be used later
attribute vec3 vColor;
attribute vec2 vUV;
attribute vec3 vNormal;

varying vec4 varyPosition;
varying vec4 varyColor;
varying vec2 varyUV;
varying vec3 varyNormal;

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main() {
    gl_Position = uMVP * vec4(vPosition, 1.0);
	
	varyColor = vec4(vColor, 1.0);
	
	varyUV = vUV;
	
	varyNormal = normalize(mat3(uInvModelViewMatrix) * vNormal);
	
	varyPosition = uModelViewMatrix * vec4(vPosition, 1.0);
	varyPosition = varyPosition / varyPosition.w;
}
