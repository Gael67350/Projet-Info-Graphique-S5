#version 130
precision mediump float;

varying vec2 vary_uv;
uniform sampler2D uTexture;

void main()
{
    //definition of the texture mapping
    vec4 color = texture2D(uTexture,vary_uv);
    gl_FragColor = vec4(color.a,color.b,color.g,color.r);
}
