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
#include "Render.h"

// Base mesh structure used for OpenGL buffers
struct gemotryMesh;

struct ModelMesh {
    GLuint VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;
    Texture* texture = nullptr;
};

class Model
{
public:
    // --- General info ---
    std::string name;
    std::string texturePath;

    // --- Core functions ---
    Model(const std::string& path);
    void Draw();
    void UpdateTransform();
    void CleanUp();

    // --- Meshes ---
    ModelMesh Mmesh;
    gemotryMesh Normalmesh;
    gemotryMesh VertexNormalmesh;

    // --- Transformations ---
    glm::mat4 transformMatrix;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    // --- Bounding box ---
    glm::vec3 center;
    glm::vec3 minAABB;
    glm::vec3 maxAABB;
    glm::vec3 size;

    // --- Normals and tangents ---
    std::vector<float> normalLines;
    std::vector<float> vertexNormalLines;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    bool hasTangents = false;
    bool hasBitangents = false;

    // --- Textures and visibility ---
    Texture* actualTexture = nullptr;
    void switchTexture(bool checker, std::string type);
    bool isHidden = false;

    glm::mat4 GetModelMatrix() const;

private:
    // --- Internal data ---
    std::string directory;

    // --- Extra textures ---
    Texture* blackWhite = nullptr;
    Texture* normalMap = nullptr;

    // --- Model loading (Assimp) ---
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};