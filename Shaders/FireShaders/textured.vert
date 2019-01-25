#version 130
precision mediump float;

attribute vec3 vPosition;
uniform mat4 uTransfo;
uniform mat4 uMvp;

//normals
attribute vec3 vNormals;

//uvValues
attribute vec2 vUV;

//varying related to the 
varying vec2 vary_uv;
varying vec4 varyPosition;

void main()
{
   gl_Position = uMvp*uTransfo*vec4(vPosition, 1.0); 
   varyPosition = uTransfo*vec4(vPosition,1);
   vary_uv = vUV;
}
