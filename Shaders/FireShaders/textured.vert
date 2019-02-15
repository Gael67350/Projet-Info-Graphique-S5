#version 130
precision mediump float;

attribute vec3 vPosition;

//projected and unprojected version of the matrix
uniform mat4 uMvp;
uniform mat3 uWorldProj33;
uniform mat4 uWorldProj;

//uvValues
attribute vec2 vUV;

//varying related to the 
varying vec2 vary_uv;

//varying related to lights

//normal data as it is in the vbo data
attribute vec3 vNormals;

//varying used to store converted normals
varying vec3 vary_normals;
varying vec4 vary_world_position;
varying vec4 vary_position;

void main()
{
   gl_Position = uMvp*vec4(vPosition, 1.0);
   vary_normals = vNormals;

   vary_world_position = uWorldProj * vec4(vPosition,1.0);
    vary_world_position = vary_world_position/vary_world_position.w;

   vary_position = gl_Position/gl_Position.w;

   vary_uv = vUV;
}
