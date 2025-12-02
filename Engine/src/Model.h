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

    // Copias CPU para raycast
    std::vector<glm::vec3> positionsLocal;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positionsWorld;
};


class Model
{
public:
    // --- General info ---
    std::string name;
    std::string texturePath;
    std::string fbxPath;
    int modelId;

    // --- Core functions ---
    void loadModel(const std::string& path);
    Model(const std::string& path);
    void Draw();
    void UpdateAABB();
    void UpdateTransform();
    void CleanUpChilds();
    void CleanUp();

    // --- Meshes ---
    ModelMesh Mmesh;
    gemotryMesh Normalmesh;
    gemotryMesh VertexNormalmesh;
    
    // --- Transformations ---
    glm::mat4 transformMatrix;
    glm::mat4 localMatrix;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 worldPosition ;
    glm::vec3 worldRotation ;
    glm::vec3 worldScale;

    // --- Bounding box ---
    glm::vec3 center;
    glm::vec3 minAABB;
    glm::vec3 maxAABB;
    glm::vec3 size;

    glm::vec3 localMinAABB;
    glm::vec3 localMaxAABB;

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
    bool hasTransparency = false;

    std::vector<int> childrenID;
    void SetChild(Model* child);
    void eraseChild(int childId);

    bool isChild = false;
    int ParentID = -1;
    bool parentTransform = false;

private:
    // --- Internal data ---
    std::string directory;

    // --- Extra textures ---
    Texture* blackWhite = nullptr;
    Texture* noTexture = nullptr;

    // --- Model loading (Assimp) ---
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);

};