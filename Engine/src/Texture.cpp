#include <iostream>
#include "Texture.h"
#include <IL/il.h>
#include "Menus.h"
#include "Application.h"

Texture::Texture(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    // Inicializar DevIL (solo una vez en todo el programa)
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

    /* ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);*/

 

    // 🔸 Detectar el formato real antes de convertir
    ILint imgFormat = ilGetInteger(IL_IMAGE_FORMAT);
    ILint imgType = ilGetInteger(IL_IMAGE_TYPE);

    // 🔸 Solo convertimos a RGBA si no tiene canal alfa
    if (imgFormat != IL_RGBA)
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    ILint bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    unsigned char* data = ilGetData();

    // --- 🔎 Detección precisa de transparencia ---
    hasAlpha = (bpp == 4);
    bool foundTransparent = false;

    if (hasAlpha)
    {
        unsigned char minAlpha = 255;
        unsigned char maxAlpha = 0;

        for (int i = 0; i < width * height; ++i)
        {
            unsigned char alpha = data[i * 4 + 3];
            if (alpha < 255) foundTransparent = true;
            if (alpha < minAlpha) minAlpha = alpha;
            if (alpha > maxAlpha) maxAlpha = alpha;
        }

        if (foundTransparent)
            std::cout << "[Texture] Transparencia real detectada. Alfa min=" << (int)minAlpha << ", max=" << (int)maxAlpha << "\n";
        else
            std::cout << "[Texture]  Tiene canal alfa pero todos los píxeles son opacos (alfa=" << (int)maxAlpha << ")\n";
    }
    else
    {
        std::cout << "[Texture] La imagen no tiene canal alfa.\n";
    }

    std::cout << "Loaded: " << imagePath
        << " (" << width << "x" << height << ")\n";


    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // ✅ evita bordes blancos/negros
    //glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(texType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

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