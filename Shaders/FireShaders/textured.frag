#version 130
precision mediump float;

varying vec2 vary_uv;
uniform sampler2D uTexture;

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
    vec3 cameraVector = normalize(uCameraPosition-vary_position.xyz);

    vec4 color;

    //definition of the texture mapping
    vec4 textureColor = texture2D(uTexture,vary_uv);
    textureColor = vec4(textureColor.a,textureColor.b,textureColor.g,textureColor.r);

    vec3 ambient = uAmbientIntensity * uLightColor;
    vec3 diffuse = uLightIntensity * max(0.0,dot(vary_normals,lightVector)) * uLightColor;
    vec3 specular = specularConstant * pow(max(0.0,dot(reflect(-lightVector,vary_normals),cameraVector)),shine);

    color = vec4(ambient+diffuse+specular,1.0);

    gl_FragColor = textureColor*color;
}
