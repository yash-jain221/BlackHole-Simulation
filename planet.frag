#version 330 core

// MRT outputs
layout(location = 0) out vec4 outColor0;   // main scene color
layout(location = 1) out vec4 outBright;   // bright-pass color
layout(location = 2) out float bhMaskTex; 

in vec2 texCoord;

// planet texture
uniform sampler2D diffuse0;
uniform bool isStar;  // ? Add this to distinguish star from planets

void main()
{
    // sample texture
    vec3 color = texture(diffuse0, texCoord).rgb;

    // If this is the star, make it SUPER bright (HDR emission)
    if (isStar)
    {
        vec3 starEmission = color * 15.0;  // 15x brightness for strong bloom
        outColor0 = vec4(starEmission, 1.0);
        outBright = vec4(starEmission, 1.0);  // Force into bloom pass
    }
    else
    {
        // Normal planet rendering
        outColor0 = vec4(color, 1.0);
        
        // Brightness test for bloom (only bright parts of planets)
        float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
        
        if (brightness > 1.0)
            outBright = vec4(color, 1.0);
        else
            outBright = vec4(0.0, 0.0, 0.0, 1.0);
    }

    bhMaskTex = 0.0;
}

