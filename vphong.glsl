#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;
uniform vec4 lightPosition;

out vec3 fN;
out vec3 fE;
out vec3 fL;

void main()
{
    vec4 eyePosition = view * model * vPosition;
    fN = mat3(view) * normalMatrix * vNormal;
    fL = lightPosition.xyz - eyePosition.xyz;
    fE = -eyePosition.xyz;
    gl_Position = projection * eyePosition;
}
