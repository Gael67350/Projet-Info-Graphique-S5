#version 130
precision mediump float;

attribute vec3 vPosition;
uniform mat4 uMvp;

//normal data as it is in the vbo data
attribute vec3 vNormals;

//Color information for fragment shader
uniform vec4 uColor;
varying vec4 varyColor;

//light related matrix
uniform mat4 uWorldProjInv;
uniform mat4 uWorldProj;

varying vec3 vary_normals;
varying vec4 vary_position;


void main()
{
    gl_Position = uMvp*vec4(vPosition, 1.0); 
    varyColor = uColor;

    vary_normals = normalize(mat3(uWorldProjInv)*vNormals);

    vary_position = uWorldProj * vec4(vPosition,1.0);
    vary_position = vary_position / vary_position.w;

}
