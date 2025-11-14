#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "shaderClass.h"

class HDRLoad {
public:
    unsigned int vao, vbo;
    unsigned int hdrTexture;
    Shader* shader;

    HDRLoad(Shader &shader, const std::string& hdrPath);
    void Draw();
    void Cleanup();

private:
    void loadHDR(const std::string& hdrPath);
    void createSphere();
};

