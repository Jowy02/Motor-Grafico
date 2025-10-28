#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"


struct ModelMesh {
    GLuint VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;
    Texture* texture = nullptr;
};


class Model
{
public:
    std::string name;
    // Constructor: carga el modelo desde un archivo usando Assimp
    Model(const std::string& path);
    // Dibuja el modelo con un shader y una cámara
    void Draw();
    void CleanUp();
    ModelMesh Mmesh;

    glm::vec3 center;
    glm::vec3 minAABB;
    glm::vec3 maxAABB;
    glm::vec3 size;

private:
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};
