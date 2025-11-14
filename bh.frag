#version 330 core
out vec4 FragColor;

in vec2 texCoord;

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

    FragColor = vec4(color, 1.0);
}