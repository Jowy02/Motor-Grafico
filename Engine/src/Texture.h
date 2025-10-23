#pragma once
#include <glad/glad.h>
#include <string>
#include <IL/il.h>

class Texture
{
public:
    GLuint ID = 0;
    GLenum type = 0;

    Texture(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

    void texUnit(GLuint shaderProgram, const char* uniform, GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};
