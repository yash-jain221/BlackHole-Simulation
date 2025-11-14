#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 WorldPos;
uniform mat4 camMatrix;

void main()
{
    WorldPos = aPos;
    gl_Position = mat4(mat3(camMatrix)) * vec4(aPos, 1.0);
}
