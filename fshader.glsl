#version 410

in vec4 P;
in vec3 N;

uniform vec3 Ia, Id, Is, lightPos;

uniform float shininess;

out vec4 fFragColor;

void main()
{
    vec3 fN = normalize(N);
    vec3 E = normalize(-P.xyz);
    vec3 L, R;
    float kd, ks;

    L = normalize(lightPos - P.xyz);
    R = 2*dot(L,fN)*fN-L;
    kd = max(dot(L,fN),0);
    ks = (kd == 0.0f) ? 0.0f : pow(max(dot(R,E),0),shininess);

    float d = clamp(-P.z/100.0f, 0, 1); // Simulate black fog

    fFragColor.rgb = (Ia + Id*kd + Is*ks)*(1.0f-d);
    fFragColor.a = 1.0f;
}
