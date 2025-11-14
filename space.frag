#version 330 core
out vec4 FragColor;
in vec3 WorldPos;
uniform sampler2D equirectangularMap;
const float PI = 3.14159265359;

vec2 sampleSphericalMap(vec3 v)
{
    float x = atan(v.z, v.x);
    float y = asin(v.y);
    return vec2(x / (2.0 * PI) + 0.5, y / PI + 0.5);
}

void main()
{
    vec2 uv = sampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
