#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

//color information input
varying vec4 varyColor;

varying vec3 vary_normals;
varying vec4 vary_position;

uniform vec3 uLightPosition;

uniform float uAmbientIntensity;
uniform float uLightIntensity;
uniform vec3 uLightColor;

uniform vec3 specularConstant;

uniform float shine;

uniform vec3 uCameraPosition;

void main()
{
    vec3 lightVector = normalize(uLightPosition - vary_position.xyz);
    vec3 cameraVector = normalize(uCameraPosition - vary_position.xyz);

	float distance = distance(vary_position.xyz, uLightPosition);
	float lightAttenuation = clamp(18.0 / length(distance), 0.0, 1.0);

    vec4 color;

    vec3 ambient = uAmbientIntensity * uLightColor;
    vec3 diffuse = uLightIntensity * max(0.0,dot(vary_normals,lightVector)) * uLightColor;
    vec3 specular = specularConstant * pow(max(0.0,dot(reflect(-lightVector,vary_normals),cameraVector)),shine);

    color = vec4(lightAttenuation *(ambient+diffuse+specular),1.0);

    gl_FragColor = varyColor*color;
}
