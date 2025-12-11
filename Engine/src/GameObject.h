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

//Components
#include "ComponentMesh.h"
#include "ComponentTransform.h"

// Base mesh structure used for OpenGL buffers
struct gemotryMesh;

//struct ModelMesh {
//    GLuint VAO = 0, VBO = 0, EBO = 0;
//    unsigned int indexCount = 0;
//    Texture* texture = nullptr;
//
//    // Copias CPU para raycast
//    std::vector<glm::vec3> positionsLocal;
//    std::vector<unsigned int> indices;
//    std::vector<float> vertices;
//    std::vector<glm::vec3> positionsWorld;
//};

class GameObject
{
public:
    // --- General info ---
    std::string name;
    std::string texturePath;
    int modelId;

    // --- Core functions ---
    GameObject(const std::string& path);
    void Draw();
    //void UpdateAABB();
    void UpdateTransform();
    void CleanUpChilds();
    void CleanUp();
    void ApplTexture(Texture* tex, std::string path);

    //Play, Stop, Puase
    bool IsCreatedInPlay() const { return created_in_play; }
    void SetCreatedInPlay(bool value) { created_in_play = value; }
    void SaveInitialState();
    void LoadInitialState();

    glm::vec3 GetInitialPosition() const;
    glm::vec3 GetInitialRotation() const;
    glm::vec3 GetInitialScale() const;
    std::string GetInitialTexturePath() const;
    bool GetInitialIsHidden() const;
    int GetInitialParentID() const;
    void SetInitialParentID(int id);

    // --- Meshes ---
    //ModelMesh Mmesh;
    gemotryMesh Normalmesh;
    gemotryMesh VertexNormalmesh;

    ComponentMesh* myMesh;
    ComponentTransform* myTransform;

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
    void SetChild(GameObject* child);
    void eraseChild(int childId);

    bool isChild = false;
    int ParentID = -1;
    bool parentTransform = false;

    int componentID = -1;
    std::string modelPath;
    bool haveComponents = false;

    //void GameObject::RecreateBuffers();
private:
    // --- Internal data ---
    std::string directory;

    // --- Extra textures ---
    Texture* blackWhite = nullptr;
    Texture* noTexture = nullptr;

    // --- Model loading (Assimp) ---
    void loadModel(const std::string& path);

    //Play, Stop, Puase
    glm::vec3 initial_position;
    glm::vec3 initial_rotation;
    glm::vec3 initial_scale;
    bool initial_isHidden = false;
    std::string initial_texturePath;
    bool created_in_play = false;

    std::string initial_modelPath;
    int initial_ParentID = -1;


};