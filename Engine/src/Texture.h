#pragma once

#include <glad/glad.h>
#include <string>
#include <IL/il.h>

class Texture
{
public:
    // --- Texture data ---
    GLuint ID = 0;
    GLenum type = 0;
    bool hasAlpha = false;
    int width = 0;
    int height = 0;
    std::string textPath;

    // --- Constructor ---
    Texture(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

    // --- Texture operations ---
    void texUnit(GLuint shaderProgram, const char* uniform, GLuint unit);

    void Bind();
    void Unbind();
    void Delete();
};