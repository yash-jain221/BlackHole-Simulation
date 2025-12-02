#version 330 core
in vec2 uv;
in vec3 color;
in vec3 worldPos;

out vec4 FragColor;
uniform float time;

// Simple 3D noise function (Perlin-like)
float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);  // Smoothstep
    
    return mix(
        mix(mix(hash(p + vec3(0,0,0)), hash(p + vec3(1,0,0)), f.x),
            mix(hash(p + vec3(0,1,0)), hash(p + vec3(1,1,0)), f.x), f.y),
        mix(mix(hash(p + vec3(0,0,1)), hash(p + vec3(1,0,1)), f.x),
            mix(hash(p + vec3(0,1,1)), hash(p + vec3(1,1,1)), f.x), f.y),
        f.z
    );
}

void main()
{
    vec2 centered = uv - 0.5;
    float d = length(centered);
    
    // 3D noise for cloud-like density variation
    vec3 noisePos = worldPos * 0.05 + vec3(time * 0.02, time * 0.01, 0.0);
    float n = noise(noisePos);
    n = n * 0.5 + 0.5;  // Remap to [0, 1]
    
    // Layered noise for detail
    float detail = noise(noisePos * 3.0) * 0.3;
    float density = n + detail;
    
    // Soft radial falloff
    float radial = exp(-d * d * 4.0);
    
    // Combine noise with radial gradient
    float intensity = density * radial;
    
    // Smooth edge fadeout
    float alpha = intensity * smoothstep(0.5, 0.0, d);
    
    // Color variation based on noise
    vec3 finalColor = color * (1.0 + n * 0.5) * 1.5;
    
    FragColor = vec4(finalColor, alpha * 0.4);  // Semi-transparent fog
}
