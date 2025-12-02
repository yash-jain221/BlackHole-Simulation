#version 330 core
in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sceneTex;
uniform float threshold; 

void main()
{
    vec3 color = texture(sceneTex, texCoords).rgb;

    // luminance
    float lum = dot(color, vec3(0.2126, 0.7152, 0.0722));
    // keep only bright parts
    if (lum > threshold)
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0);
}
