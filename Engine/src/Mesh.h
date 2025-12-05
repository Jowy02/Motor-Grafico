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
#include "Module.h"
#include "Application.h"

// Base mesh structure used for OpenGL buffers
struct gemotryMesh;

struct MMesh{
    GLuint VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;
    Texture* texture = nullptr;

    // Copias CPU para raycast
    std::vector<glm::vec3> positionsLocal;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positionsWorld;
};

class Mesh : public Module
{
public:
    // --- Core functions ---
    Mesh();
    virtual ~Mesh();

    bool CleanUp() override;

    // --- General info ---
    std::string name;
    std::string texturePath;
    int modelId;

    // --- Meshes ---
    MMesh Mmesh;
    gemotryMesh Normalmesh;
    gemotryMesh VertexNormalmesh;
    
    // --- Transformations ---
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
    bool hasTransparency = false;

    int componentID = -1;
    std::string modelPath;
    bool haveComponents = false;

    void loadModel(const std::string& path, GameObject* Obj);
private:
    // --- Internal data ---
    std::string directory;
    //GameObject* Obj = nullptr;
    
    // --- Extra textures ---
    Texture* blackWhite = nullptr;
    Texture* noTexture = nullptr;

    // --- Model loading (Assimp) ---
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);

    void processOthers(const aiScene* scene);
    std::vector<aiMesh*> otherMesh;
    int objNum = 0;
};