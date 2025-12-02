#version 330 core
in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform bool horizontal; // true -> horizontal pass, false -> vertical pass

const float weights[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162); //gaussian kernel

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, texCoords).rgb * weights[0];

    if (horizontal) {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(image, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    } else {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, texCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            result += texture(image, texCoords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
