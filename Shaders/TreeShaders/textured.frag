#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

uniform sampler2D uTexture;

varying vec4 varyColor;
varying vec2 varyUV;

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main() {
	vec4 textureColor = texture2D(uTexture, varyUV);
	textureColor = vec4(textureColor.a, textureColor.b, textureColor.g, textureColor.r);

    gl_FragColor = textureColor * varyColor;
}
