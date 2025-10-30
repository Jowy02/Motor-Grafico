#include "Model.h"
#include "Application.h"
#include "Camera.h"
#include "Texture.h"
#include <iostream>
#include <cstring> 
#include "Menus.h"

Model::Model(const std::string& path)
{
    if (path != "NULL")loadModel(path);
    else {
        position = { 0,0,0 };
        rotation = { 0,0,0 };
        scale = { 1,1,1 };

        minAABB = { -1,-1,-1 };
        maxAABB = { 1,1,1 };
        UpdateTransform();
    }
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

    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));    //Centrar

    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1, 0, 0));   //Rotar

    model = glm::scale(model, glm::vec3(0.5f)); //Escalar

    // Enviar la matriz al shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model_matrix");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));

    if (Mmesh.texture)
    {
        Mmesh.texture->texUnit(shaderProgram, "tex0", 0);
        Mmesh.texture->Bind();
        GLint useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");
        glUniform1i(useTexLoc, 1);
    }
    else
    {
        GLint useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");
        glUniform1i(useTexLoc, 0);
    }

    // Dibujar todos los meshes

    glBindVertexArray(Mmesh.VAO);
    glDrawElements(GL_TRIANGLES, Mmesh.indexCount, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);


    if (Mmesh.texture)
        Mmesh.texture->Unbind();
}
void Model::UpdateTransform()
{
    // Recalcular matriz de transformación
    transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, position);
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transformMatrix = glm::scale(transformMatrix, scale);

    // Los 8 vértices del AABB original
    glm::vec3 corners[8] = {
        {minAABB.x, minAABB.y, minAABB.z},
        {minAABB.x, minAABB.y, maxAABB.z},
        {minAABB.x, maxAABB.y, minAABB.z},
        {minAABB.x, maxAABB.y, maxAABB.z},
        {maxAABB.x, minAABB.y, minAABB.z},
        {maxAABB.x, minAABB.y, maxAABB.z},
        {maxAABB.x, maxAABB.y, minAABB.z},
        {maxAABB.x, maxAABB.y, maxAABB.z},
    };

    glm::vec3 newMin(FLT_MAX);
    glm::vec3 newMax(-FLT_MAX);

    // Aplicar la matriz de transformación a cada vértice
    for (int i = 0; i < 8; ++i)
    {
        glm::vec4 transformed = transformMatrix * glm::vec4(corners[i], 1.0f);
        glm::vec3 p = glm::vec3(transformed);

        newMin = glm::min(newMin, p);
        newMax = glm::max(newMax, p);
    }

    // Guardar nuevo centro y tamaño
    center = (newMin + newMax) * 0.5f;
    size = newMax - newMin;
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
        Application::GetInstance().menus->LogToConsole(std::string("ERROR: FBX load failed: ") + path);
        Application::GetInstance().menus->LogToConsole(std::string("ERROR: Assimp load failed: ") + importer.GetErrorString());
        return;
    }
    else {
        Application::GetInstance().menus->LogToConsole("Loaded FBX: " + path);

        Application::GetInstance().menus->LogToConsole("ASSIMP initialized");

    }

    directory = path.substr(0, path.find_last_of('/'));

    name = path.substr(path.find_last_of("/\\") + 1);
    name = name.substr(0, name.find_last_of('.'));

    processNode(scene->mRootNode, scene);
    center = (minAABB + maxAABB) * 0.5f;
    size = maxAABB - minAABB;

    position = { 0,0,0 };
    rotation = { 0,0,0 };
    scale = { 1,1,1 };

    UpdateTransform();
}

// Procesa recursivamente todos los nodos
void Model::processNode(aiNode* node, const aiScene* scene)
{
    // Procesa todas las mallas del nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    // Recurre en los hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

// Convierte un aiMesh de Assimp en ModelMesh con VAO/VBO/EBO
void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    minAABB = glm::vec3(FLT_MAX);
    maxAABB = glm::vec3(-FLT_MAX);

    // Extraer vértices: posición, normales y coordenadas UV
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 vertex(mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);

        // Actualizar AABB
        minAABB = glm::min(minAABB, vertex);
        maxAABB = glm::max(maxAABB, vertex);

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

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            std::string filename = std::string(path.C_Str());

            // Normalizar barras invertidas
            std::replace(filename.begin(), filename.end(), '\\', '/');

            // Si la textura NO es una ruta absoluta, agrégale el directorio del modelo
            if (filename[0] != '/' && !(filename.length() > 1 && filename[1] == ':'))
            {
                filename = directory + "/" + filename;
            }

            std::cout << "Cargando textura desde material: " << filename << std::endl;

            Mmesh.texture = new Texture(filename.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        }
        
        else
        {
            std::cout << "No se encontró textura difusa en el material." << std::endl;
            Mmesh.texture = nullptr;
        }
    }
}

void Model::CleanUp()
{
    if (Mmesh.texture)
    {
        Mmesh.texture->Delete();
        delete Mmesh.texture;
        Mmesh.texture = nullptr;
    }

    glDeleteVertexArrays(1, &Mmesh.VAO);
    glDeleteBuffers(1, &Mmesh.VBO);
    glDeleteBuffers(1, &Mmesh.EBO);
}
