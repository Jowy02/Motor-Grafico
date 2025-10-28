#include <iostream>
#include "Texture.h"
#include <IL/il.h>
#include "Menus.h"
#include "Application.h"

Texture::Texture(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    // Inicializa DevIL (solo una vez en todo el programa)
    static bool ilInitialized = false;
    if (!ilInitialized) {
        ilInit();
        ilInitialized = true;
    }

    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);

    if (!ilLoadImage(imagePath)) {
        std::cout << "Error: No se pudo cargar la imagen: " << imagePath << std::endl;
        Application::GetInstance().menus->LogToConsole("ERROR: texture load failed: " + std::string(imagePath));
        Application::GetInstance().menus->LogToConsole("ERROR: DevIL load failed");

        ilDeleteImages(1, &img);
        return;
    }
    else {
        Application::GetInstance().menus->LogToConsole("Loaded texture: " + std::string(imagePath));
        Application::GetInstance().menus->LogToConsole("DevIL initialized");
    }

    ilConvertImage(format, pixelType);

    std::cout << "Loaded: " << imagePath
        << " (" << ilGetInteger(IL_IMAGE_WIDTH) << "x"
        << ilGetInteger(IL_IMAGE_HEIGHT) << ")\n";

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(texType, 0, format,
        ilGetInteger(IL_IMAGE_WIDTH),
        ilGetInteger(IL_IMAGE_HEIGHT),
        0, format, pixelType, ilGetData());

    glGenerateMipmap(texType);

    glBindTexture(texType, 0);
    ilDeleteImages(1, &img);
}

void Texture::texUnit(GLuint shaderProgram, const char* uniform, GLuint unit)
{
    glUseProgram(shaderProgram);
    GLuint texUni = glGetUniformLocation(shaderProgram, uniform);
    glUniform1i(texUni, unit);
}

void Texture::Bind()
{
    glBindTexture(type, ID);
}

void Texture::Unbind()
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}