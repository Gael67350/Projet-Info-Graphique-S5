#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

uniform bool uIsLight;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;
uniform vec3 uCameraPosition;

uniform sampler2D uTexture;

varying vec4 varyPosition;
varying vec4 varyColor;
varying vec2 varyUV;
varying vec3 varyNormal;

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main() {
	vec4 color;

	vec4 textureColor = texture2D(uTexture, varyUV);
	textureColor = vec4(textureColor.a, textureColor.b, textureColor.g, textureColor.r);
	
	if(uIsLight) {
		float ambientIntensity = 0.5;
		float diffuseIntensity = 1.0;
		float specIntensity = 0.1;
		float shiny = 64.0;
		
		vec3 lightDirection = normalize(uLightPosition - varyPosition.xyz);
		vec3 cameraDirection = normalize(uCameraPosition - varyPosition.xyz);
		
		vec3 ambient = ambientIntensity * uLightColor * varyColor.rgb;
		vec3 diffuse = diffuseIntensity * max(0.0, dot(varyNormal, uLightPosition)) * varyColor.rgb * uLightColor;
		vec3 specular = specIntensity * pow(max(0.0, dot(reflect(-lightDirection, varyNormal), cameraDirection)), shiny) * uLightColor;
		
		color = vec4(ambient + diffuse + specular, 1.0);
	}else{
		color = varyColor;
	}
	
	gl_FragColor = textureColor * color;
}
