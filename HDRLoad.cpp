#include "HDRLoad.h"
#include <stb/stb_image.h>
#include <iostream>
#include <vector>

const float M_PI = 3.14159265358979323846;
HDRLoad::HDRLoad(Shader &shader, const std::string& hdrPath)
{
    shader = shader;
    loadHDR(hdrPath);
    createSphere();
}

void HDRLoad::loadHDR(const std::string& hdrPath)
{
    stbi_set_flip_vertically_on_load(true);
    int w, h, n;
    float* data = stbi_loadf(hdrPath.c_str(), &w, &h, &n, 0);
    if (!data) {
        std::cerr << "Failed to load HDR: " << hdrPath << std::endl;
        return;
    }

    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

void HDRLoad::createSphere()
{
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    std::vector<float> data;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSeg = (float)x / X_SEGMENTS;
            float ySeg = (float)y / Y_SEGMENTS;
            float xPos = std::cos(xSeg * 2.0f * M_PI) * std::sin(ySeg * M_PI);
            float yPos = std::cos(ySeg * M_PI);
            float zPos = std::sin(xSeg * 2.0f * M_PI) * std::sin(ySeg * M_PI);

            data.push_back(xPos);
            data.push_back(yPos);
            data.push_back(zPos);
        }
    }

    std::vector<unsigned int> indices;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);

            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
        }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    unsigned int ebo;
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void HDRLoad::Draw()
{
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glDrawElements(GL_TRIANGLES, (64 * 64 * 6), GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void HDRLoad::Cleanup()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &hdrTexture);
}
