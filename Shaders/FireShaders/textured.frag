#version 130
precision mediump float;

varying vec2 vary_uv;
uniform sampler2D uTexture;

varying vec3 vary_normals;
varying vec4 vary_world_position;
varying vec4 vary_position;

uniform vec4 uLightPosition;

uniform float uAmbientIntensity;
uniform float uLightIntensity;
uniform vec3 uLightColor;

void main()
{
    //definition of the texture mapping
    vec4 color = texture2D(uTexture,vary_uv);
    vec4 texColor = vec4(color.a,color.b,color.g,color.r);

    vec3 ambient = uAmbientIntensity * uLightColor * texColor.rgb ;
    vec3 diffuse = uLightIntensity * max(0.0,dot(vary_normals,vec3(uLightPosition-vary_world_position))) * texColor.rgb * uLightColor;
    //vec3 specular = vec3(0.0,0.0,0.0);

    gl_FragColor = vec4(ambient+diffuse,1.0);
}
