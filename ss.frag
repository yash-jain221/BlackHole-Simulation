#version 330 core
layout(location = 0) out vec4 outColor0;
layout(location = 1) out vec4 outBright;
in vec2 texCoord;
out float bhMask;
uniform sampler2D diffuse[16]; // up to 16 textures
uniform int diffuseCount;
uniform sampler2D specular[16]; // up to 16 textures
uniform int specularCount;

void main()
{
    vec3 d = vec3(0.0);
    for (int i = 0; i < diffuseCount; i++)
        d += texture(diffuse[i], texCoord).rgb;
    d /= diffuseCount;

    vec3 s = vec3(0.0);
    for (int i = 0; i < specularCount; i++)
        s += texture(specular[i], texCoord).rgb;
    s /= diffuseCount;

    vec3 color = d;
    color += d * s * 0.5;

    outColor0 = vec4(color, 1.0);
    outBright = vec4(color, 1.0);
    bhMask=0.0f;
}