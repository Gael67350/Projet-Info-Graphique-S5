#version 130
precision mediump float;

attribute vec3 vPosition;
uniform mat4 uMvp;

//normals
attribute vec3 vNormals;

//uvValues
attribute vec2 vUV;

//varying related to the 
varying vec2 vary_uv;

void main()
{
   gl_Position = uMvp*vec4(vPosition, 1.0); 
   vary_uv = vUV;
}
