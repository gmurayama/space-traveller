#version 410

in vec4 vPos;
in vec3 vNormal;

uniform mat4 mModelView;
uniform mat3 mNormal;
uniform mat4 mProjection;

out vec4 P;
out vec3 N;

void main()
{
    P = mModelView * vPos;
    N = mNormal * vNormal;

    gl_Position = mProjection * P;
}
