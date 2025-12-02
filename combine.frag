#version 330 core
out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D sceneTex;
uniform sampler2D bloomTex;
uniform sampler2D bhMaskTex;
uniform float exposure;
uniform float bloomIntensity;
uniform vec3 bhWorld;
uniform mat4 camMatrix;

vec2 worldToScreen(vec3 p)
{
    vec4 clip = camMatrix * vec4(p ,1.0);
    clip /= clip.w;
    return clip.xy * 0.5 + 0.5;
}

void main()
{
    vec2 bhUV = worldToScreen(bhWorld);
    vec2 uv = texCoords;
    
    // Vector from current pixel to black hole center
    vec2 toCenter = bhUV - texCoords;
    float r = length(toCenter);
    
    // Gravitational lensing parameters
    float schwarzschildRadius = 0.08;  // Size of the black hole
    float maxDistortRadius = 0.8;       // How far the effect reaches
    
    // DRAMATIC gravitational lensing
    if (r > schwarzschildRadius && r < maxDistortRadius)
    {
        // Schwarzschild-like lens equation
        // Light bends more as it gets closer to the event horizon
        float rSquared = r * r;
        float rsSquared = schwarzschildRadius * schwarzschildRadius;
        
        // inverse square law for the strength of gravity
        float bendStrength = rsSquared / rSquared;
        
        // Smooth falloff at the edges
        float falloff = smoothstep(maxDistortRadius, schwarzschildRadius * 1.5, r);
        
        // Apply the distortion - pull toward black hole
        float finalStrength = bendStrength * falloff * 2.0;  // Increase this for MORE distortion
        
        uv = texCoords + normalize(toCenter) * finalStrength;
    }

    // Very close to event horizon - einstein ring
    else if (r <= schwarzschildRadius && r > schwarzschildRadius * 0.5)
    {
        // Ring around the black hole gets MASSIVELY distorted
        float angle = atan(toCenter.y, toCenter.x);
        float spiral = r / schwarzschildRadius;
        
        // Create Einstein ring effect
        uv = bhUV + vec2(cos(angle), sin(angle)) * schwarzschildRadius * 1.2;
    }
    
    // Clamp UV to prevent sampling outside texture
    uv = clamp(uv, 0.0, 1.0);
    
    // Sample the scene with distorted UVs
    vec3 scene = texture(sceneTex, uv).rgb;
    vec3 bloom = texture(bloomTex, uv).rgb;
    
    // Sample mask at ORIGINAL texCoords
    float mask = texture(bhMaskTex, texCoords).r;
    
    // If we're inside the black hole area, use original scene
    if (mask > 0.5)
    {
        scene = texture(sceneTex, texCoords).rgb;
        bloom = texture(bloomTex, texCoords).rgb;
    }
    
    vec3 finalColor = scene + bloom;
    FragColor = vec4(finalColor, 1.0);
}