#version 330 core
layout (location = 0) in vec2 aPos;      // quad vertices
layout (location = 1) in vec3 instancePos;
layout (location = 2) in float instanceSize;
layout (location = 3) in vec3 instanceColor;

out vec2 uv;
out vec3 color;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 camMatrix;
uniform float time;

void main()
{
    // Animate particles: slow circular drift + radial pulsing
    float angle = time * 0.1 + instancePos.x * 0.01;  // Rotation speed
    vec3 offset = vec3(
        cos(angle) * 2.0,
        sin(time * 0.3 + instancePos.y * 0.05) * 1.5,  // Vertical wave
        sin(angle) * 2.0
    );
    
    vec3 animatedPos = instancePos + offset;
    
    // Billboard the quad to face camera
    mat4 modelView = camMatrix * model;
    vec3 camRight = vec3(modelView[0][0], modelView[1][0], modelView[2][0]);
    vec3 camUp = vec3(modelView[0][1], modelView[1][1], modelView[2][1]);
    
    vec3 vertexPos = animatedPos 
                   + camRight * aPos.x * instanceSize
                   + camUp * aPos.y * instanceSize;
    
    gl_Position = camMatrix * model * vec4(vertexPos, 1.0);
    uv = aPos + 0.5;  // Convert from [-0.5, 0.5] to [0, 1]
    color = instanceColor;
    worldPos = animatedPos;
}
