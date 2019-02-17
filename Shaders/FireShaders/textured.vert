#version 130
precision mediump float;

attribute vec3 vPosition;

//projected and unprojected version of the matrix
uniform mat4 uMvp;

//lightRelatedMatrix
uniform mat4 uWorldProjInv;
uniform mat4 uWorldProj;

//uvValues
attribute vec2 vUV;
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
   vary_normals = normalize(mat3(uWorldProjInv)*vNormals);

   vary_uv = vUV;

   vary_position = uWorldProj * vec4(vPosition,1.0);
   vary_position = vary_position/vary_position.w;
}
