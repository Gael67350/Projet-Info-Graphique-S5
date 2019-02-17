#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

uniform bool uIsLight;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;
uniform vec4 uMaterials;
uniform vec3 uCameraPosition;

varying vec4 varyPosition;
varying vec4 varyColor; //Sometimes we use "out" instead of "varying". "out" should be used in later version of GLSL.
varying vec3 varyNormal;

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main() {
	vec4 color;
	
	if(uIsLight) {
		float ambientIntensity = uMaterials.x;
		float diffuseIntensity = uMaterials.y;
		float specIntensity = uMaterials.z;
		float shiny = uMaterials.w;
		
		vec3 lightDirection = normalize(uLightPosition - varyPosition.xyz);
		vec3 cameraDirection = normalize(uCameraPosition - varyPosition.xyz);
		
		vec3 ambient = ambientIntensity * uLightColor * varyColor.rgb;
		vec3 diffuse = diffuseIntensity * max(0.0, dot(varyNormal, lightDirection)) * varyColor.rgb * uLightColor;
		vec3 specular = specIntensity * pow(max(0.0, dot(reflect(-lightDirection, varyNormal), cameraDirection)), shiny) * uLightColor;
		
		color = vec4(ambient + diffuse + specular, 1.0);
	}else{
		color = varyColor;
	}
	
    gl_FragColor = color;
}