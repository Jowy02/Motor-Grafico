#include "Model.h"
#include "Application.h"
#include "Camera.h"
#include <iostream>
#include <cstring> 

Model::Model(const std::string& path)
{
    loadModel(path);
}

// Dibuja todas las mallas del modelo
void Model::Draw()
{
    GLuint shaderProgram = Application::GetInstance().render->shaderProgram;
    //Modelo se vea desde la posicón de la cámara correcta
    Application::GetInstance().camera.get()->Inputs(Application::GetInstance().window.get()->window);
    Application::GetInstance().camera.get()->Matrix(45.0f, 0.1f, 100.0f, shaderProgram);

    // Matriz de modelo
    glm::mat4 model = glm::mat4(1.0f);

    //model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));    //Centrar

    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));   //Rotar

    model = glm::scale(model, glm::vec3(0.01f)); //Escalar

    // Enviar la matriz al shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model_matrix");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Dibujar todos los meshes

    glBindVertexArray(Mmesh.VAO);
    glDrawElements(GL_TRIANGLES, Mmesh.indexCount, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

// Carga el modelo con Assimp
void Model::loadModel(const std::string& path)
{
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
        return;
    }

    processNode(scene->mRootNode, scene);
}

// Procesa recursivamente todos los nodos
void Model::processNode(aiNode* node, const aiScene* scene)
{
    // Procesa todas las mallas del nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh);
    }

    // Recurre en los hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

// Convierte un aiMesh de Assimp en ModelMesh con VAO/VBO/EBO
void Model::processMesh(aiMesh* mesh)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Extraer vértices: posición, normales y coordenadas UV
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // Posición
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Normal
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        // Textura UV
        if (mesh->mTextureCoords[0])
        {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    // Extraer índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Crear buffers OpenGL

    glGenVertexArrays(1, &Mmesh.VAO);
    glGenBuffers(1, &Mmesh.VBO);
    glGenBuffers(1, &Mmesh.EBO);

    glBindVertexArray(Mmesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Mmesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mmesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    Mmesh.indexCount = indices.size();
}
