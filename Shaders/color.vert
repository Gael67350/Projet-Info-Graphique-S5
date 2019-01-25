#version 130
precision mediump float;

uniform mat4 uMVP;

attribute vec3 vPosition;
attribute vec3 vNormal;

varying vec3 varyNormal;

void main()
{
	  gl_Position = uMVP * vec4(vPosition, 1.0);
    varyNormal  = vNormal;
}
