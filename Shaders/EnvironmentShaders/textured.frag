#version 130
precision mediump float;

uniform bool uIsLight;
uniform bool uLightState;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;
uniform vec4 uMaterials;
uniform vec3 uCameraPosition;

uniform sampler2D uTexture;

varying vec4 varyPosition;
varying vec3 varyNormal;
varying vec2 varyUV;

void main()
{
	vec4 color;
	
	//definition of the texture mapping
	vec4 textureColor = texture2D(uTexture, varyUV);
	textureColor = vec4(textureColor.a, textureColor.b, textureColor.g, textureColor.r);

    if(uIsLight && uLightState) {
		float ambientIntensity = uMaterials.x;
		float diffuseIntensity = uMaterials.y;
		float specIntensity = uMaterials.z;
		float shiny = uMaterials.w;
		
		float distance = distance(varyPosition.xyz, uLightPosition);
		float lightAttenuation = clamp(18.0 / length(distance), 0.0, 1.0);
		
		vec3 lightDirection = normalize(uLightPosition - varyPosition.xyz);
		vec3 cameraDirection = normalize(uCameraPosition - varyPosition.xyz);
		
		vec3 ambient = ambientIntensity * uLightColor * textureColor.rgb;
		vec3 diffuse = diffuseIntensity * 0.6 * textureColor.rgb * uLightColor;
		vec3 specular = specIntensity * pow(max(0.0, dot(reflect(-lightDirection, varyNormal), cameraDirection)), shiny) * uLightColor;
		
		color = vec4(lightAttenuation * (ambient + diffuse + specular), 1.0);
	}else{
		color = textureColor;
	}
	
    gl_FragColor = color;
}
