#version 330 core
layout(location = 0) out vec4 outColor0;
layout(location = 1) out vec4 outBright;
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

    outColor0 = vec4(color, 1.0);
    outBright = vec4(color, 1.0);
}
