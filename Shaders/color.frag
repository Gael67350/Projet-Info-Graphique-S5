#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

uniform vec3 color = vec3(0, 1.0, 1.0);

varying vec3 varyNormal;

void main()
{
    gl_FragColor = vec4(color, 1.0);
    
    //gl_FragColor = vec4((varyNormal+1.0)/2.0, 1.0);
}
