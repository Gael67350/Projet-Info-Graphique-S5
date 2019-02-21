#version 130
precision mediump float;

uniform mat4 uMVP;
uniform mat4 uModelViewMatrix;
uniform mat4 uInvModelViewMatrix;

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vUV;

//varying related to the 
varying vec4 varyPosition;
varying vec3 varyNormal;
varying vec2 varyUV;

//multiplication factor for texture replication
uniform float textureMultiplier ;

void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0); 
   
	varyNormal = normalize(mat3(uInvModelViewMatrix) * vNormal);
	
	varyPosition = uModelViewMatrix * vec4(vPosition, 1.0);
	varyPosition = varyPosition / varyPosition.w;
   
	varyUV = vec2(mat3(textureMultiplier,0.0,0.0,0.0,textureMultiplier,0.0,0.0,0.0,1.0)*vec3(vUV,1.0));
}