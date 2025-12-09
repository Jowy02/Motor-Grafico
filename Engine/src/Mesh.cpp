#include "Mesh.h"
#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include <iostream>
#include <cstring> 
#include "Menus.h"
#include "Scene.h"
#include "OctreeNode.h" 

Mesh::Mesh() : Module()
{
    name = "Camera";
}
Mesh::~Mesh() {}

// Load a model using Assimp
void Mesh::loadModel(const std::string& path, GameObject* Obj)
{
    modelPath = path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        Application::GetInstance().menus->LogToConsole(std::string("ERROR: FBX load failed: ") + path);
        Application::GetInstance().menus->LogToConsole(std::string("ERROR: Assimp load failed: ") + importer.GetErrorString());
        return;
    }
    else 
    {
        Application::GetInstance().menus->LogToConsole("Loaded FBX: " + path);
        Application::GetInstance().menus->LogToConsole("ASSIMP initialized");
    }

    directory = path.substr(0, path.find_last_of('/'));

    name = path.substr(path.find_last_of("/\\") + 1);
    name = name.substr(0, name.find_last_of('.'));

    processNode(scene->mRootNode, scene);
    Obj->modelId = Application::GetInstance().scene.get()->models.size() + otherMesh.size();

    //Obj = &Application::GetInstance().scene->models.back();
    Obj->Mmesh.VAO = Mmesh.VAO;
    Obj->Mmesh.VBO = Mmesh.VBO;
    Obj->Mmesh.EBO = Mmesh.EBO;

    Obj->Mmesh.indexCount = Mmesh.indexCount;
    Obj->Mmesh.indices = Mmesh.indices;
    Obj->Mmesh.vertices = Mmesh.vertices;

    Obj->Mmesh.positionsLocal = Mmesh.positionsLocal;
    Obj->Mmesh.positionsWorld = Mmesh.positionsWorld;
    Obj->Mmesh.texture = Mmesh.texture; 

    Obj->hasTransparency = hasTransparency;

    Obj->minAABB = minAABB;
    Obj->maxAABB = maxAABB;
    Obj->name = name;

    Obj->modelPath = "../Library/Meshes/" + name + ".txt";

    maxAABB = { 0,0,0 };
    minAABB = {0,0,0};
    processOthers(scene);

    //Mmesh.positionsLocal.clear();
    //Mmesh.positionsWorld.clear();
    //Mmesh.indices.clear();
    //maxAABB = { 0,0,0 };
    //minAABB = {0,0,0};
    //Mmesh.indexCount = 0;
    //Mmesh.indices.clear();

    otherMesh.clear();
    objNum = 0;

}
void Mesh::processOthers(const aiScene* scene)
{
    for (auto& mesh : otherMesh)
    {
        Mmesh.positionsLocal.clear();
        Mmesh.positionsWorld.clear();
        Mmesh.indices.clear();

        GameObject newModel("Imported");

        processMesh(mesh, scene);

        newModel.Mmesh.VAO = Mmesh.VAO;
        newModel.Mmesh.VBO = Mmesh.VBO;
        newModel.Mmesh.EBO = Mmesh.EBO;

        newModel.Mmesh.indexCount = Mmesh.indexCount;
        newModel.Mmesh.indices = Mmesh.indices;
        newModel.Mmesh.vertices = Mmesh.vertices;

        newModel.Mmesh.positionsLocal = Mmesh.positionsLocal;
        newModel.Mmesh.positionsWorld = Mmesh.positionsWorld;
        newModel.Mmesh.texture = Mmesh.texture;

        newModel.minAABB = minAABB;
        newModel.maxAABB = maxAABB;

        newModel.name = mesh->mName.C_Str();
        newModel.modelId = Application::GetInstance().scene->models.size();
        newModel.center = (minAABB + maxAABB) * 0.5f;
        newModel.size = maxAABB - minAABB;

        newModel.localMinAABB = newModel.minAABB;
        newModel.localMaxAABB = newModel.maxAABB;

        newModel.componentID = modelId;

        newModel.position = { 0,0,0 };
        newModel.rotation = { 0,0,0 };
        newModel.scale = { 1,1,1 };

        if (!Application::GetInstance().scene->octreeRoot) {
            Application::GetInstance().scene->BuildOctree();
        }
        else {
            OctreeNode* root = Application::GetInstance().scene->octreeRoot.get();
            root->Insert(&Application::GetInstance().scene->models.back());
        }

        newModel.UpdateTransform();
        newModel.UpdateAABB();

        newModel.modelPath = "../Library/Meshes/" + newModel.name + ".txt";
        Application::GetInstance().scene->models.push_back(std::move(newModel));
    }
}
// Process all meshes in a node
void Mesh::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        if (objNum == 0)
        {
            processMesh(mesh, scene);
            name = mesh->mName.C_Str();
        }
        else
        {
            otherMesh.push_back(mesh);
            haveComponents = true;
        }
        objNum++;
    }

    // Recursively process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

// Convert an aiMesh from Assimp into a ModelMesh with VAO/VBO/EBO
void Mesh::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    ModelMesh newMesh;

    minAABB = glm::vec3(FLT_MAX);
    maxAABB = glm::vec3(-FLT_MAX);

    Mmesh.positionsLocal.clear();
    Mmesh.indices.clear();
    
    // Extract vertices: position, normal, and UV coordinates
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 vertex(mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);

        minAABB = glm::min(minAABB, vertex);
        maxAABB = glm::max(maxAABB, vertex);

        Mmesh.positionsLocal.push_back(vertex);

        // Position
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);

        // Normal (solo si existen)
        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }
        else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

        // Texture UV
        if (mesh->HasTextureCoords(0)) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    Mmesh.vertices = vertices;

    localMinAABB = minAABB;
    localMaxAABB = maxAABB;

    // Extract tangents if available
    if (mesh->mTangents != nullptr)
    {
        hasTangents = true;
        tangents.clear();
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            tangents.push_back(glm::vec3(mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z));
        }
    }

    // Extract bitangents if available
    if (mesh->mBitangents != nullptr) {
        hasBitangents = true;
        bitangents.clear();
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            bitangents.push_back(glm::vec3(mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z));
        }
    }

    // Extract indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices == 3) { // asegurarse de que es triángulo
            for (unsigned int j = 0; j < 3; j++) {
                indices.push_back(face.mIndices[j]);
                Mmesh.indices.push_back(face.mIndices[j]);
            }
        }
    }

    
    // Draw vertex and face normals for debugging
    VertexNormalmesh = Application::GetInstance().render.get()->DrawVertexNormalsFromMesh(vertices.data(), vertices.size(), tangents, bitangents, {}, vertexNormalLines);
    Normalmesh = Application::GetInstance().render.get()->DrawFaceNormals(vertices.data(), indices.data(), indices.size(), normalLines);

    // Create OpenGL buffers
    glGenVertexArrays(1, &Mmesh.VAO);
    glGenBuffers(1, &Mmesh.VBO);
    glGenBuffers(1, &Mmesh.EBO);

    glBindVertexArray(Mmesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Mmesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mmesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //// Alpha
    //glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    //glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    Mmesh.indexCount = indices.size();

    // Load texture from material if available
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
          
            std::string filename = std::string(path.C_Str());

            std::string materialName = filename.substr(filename.find_last_of("/\\") + 1);
            materialName = materialName.substr(0, name.find_last_of('.'));
            filename = "../Library/Images/" + materialName;

            //// Normalize backslashes
            //std::replace(filename.begin(), filename.end(), '\\', '/');

            //// If the texture path is not absolute, prepend the model directory
            //if (filename[0] != '/' && !(filename.length() > 1 && filename[1] == ':'))
            //    filename = directory + "/" + filename;

            std::cout << "Loading texture from material: " << filename << std::endl;

            Mmesh.texture = new Texture(filename.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
          
            if (Mmesh.texture)
                hasTransparency = Mmesh.texture->hasAlpha;

        }
        else
        {
            std::cout << "No diffuse texture found in the material." << std::endl;
            Mmesh.texture = nullptr;
            hasTransparency = false;
        }
    }
}
bool Mesh::CleanUp()
{
    if (Mmesh.VAO != 0)
        glDeleteVertexArrays(1, &Mmesh.VAO);
    if (Mmesh.VBO != 0)
        glDeleteBuffers(1, &Mmesh.VBO);
    if (Mmesh.EBO != 0)
        glDeleteBuffers(1, &Mmesh.EBO);
    if (Mmesh.texture)
        delete Mmesh.texture;
    if (blackWhite)
        delete blackWhite;

    Mmesh.VAO = 0;
    Mmesh.VBO = 0;
    Mmesh.EBO = 0;

    Mmesh.texture = nullptr;
    actualTexture = nullptr;
    blackWhite = nullptr;

    return true;
}