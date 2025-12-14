#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "GameObject.h"
#include "Menus.h"
#include "ResourceManager.h"

#include <iostream> 

#include <windows.h>
#include <filesystem>

ResourceManager::ResourceManager() : Module()
{
}

// Destructor
ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Awake()
{
    return true;
}

bool ResourceManager::Start()
{
    LoadResource();
    //Once its all upload Street enviroment load using the meta file
    Application::GetInstance().scene.get()->LoadMeta("../Library/Meta/street2.meta");
    return true;
}

bool ResourceManager::PreUpdate()
{

    return true;
}

bool ResourceManager::Update(float dt)
{
    return true;
}

void ResourceManager::LoadResource()
{
    //Check every folder (MESH/TEXTURE/META)
    CheckMeshFiles();
    CheckTextureFiles();
    CheckMetaFiles();

    //Update menu reference 
    Application::GetInstance().menus.get()->metaFiles.clear();
    Application::GetInstance().menus.get()->metaFiles = metaFiles;

    Application::GetInstance().menus.get()->textures.clear();
    Application::GetInstance().menus.get()->textures = textures;

    Application::GetInstance().menus.get()->meshesFiles.clear();
    Application::GetInstance().menus.get()->meshesFiles = meshesFiles;

    LoadMeshResource();
}
void ResourceManager::CheckMeshFiles()
{
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA("..\\Library\\Meshes\\*", &data);
    std::string fileName = "";
    bool exist = false;
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
                continue;
            fileName = data.cFileName;

            if (fileName.substr(fileName.size() - 5) == ".mesh") {
                for (auto& files : meshesFiles)
                {
                    std::string TempfileName = "../Library/Meshes/" + fileName;
                    if (files == TempfileName) {
                        exist = true;
                    }
                }
                if (!exist)meshesFiles.push_back("../Library/Meshes/" + fileName);
                exist = false;

            }
        } while (FindNextFileA(h, &data));

        FindClose(h);
    }
}

void ResourceManager::CheckTextureFiles()
{
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA("..\\Library\\Images\\*", &data);
    std::string fileName;
    bool exist = false;

    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
                continue;

            // Filtrar por tipo de archivo
            fileName = data.cFileName;
            if (fileName.substr(fileName.size() - 4) == ".png") {
                fileName = "../Library/Images/" + fileName;
                for (auto& text : textures)
                {
                    if (text->textPath == fileName) exist = true;
                }
                Texture* tex = new Texture(fileName.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
                if (!exist)textures.push_back(tex);
                exist = false;
            }
        } while (FindNextFileA(h, &data));

        FindClose(h);
    }
}

void ResourceManager::CheckMetaFiles()
{
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA("..\\Library\\Meta\\*", &data);
    std::string fileName = "";
    bool exist = false;
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
                continue;
            fileName = data.cFileName;

            if (fileName.substr(fileName.size() - 5) == ".meta") {
                for (auto& files : metaFiles)
                {
                    std::string TempfileName = "../Library/Meta/" + fileName;
                    if (files == TempfileName) {
                        exist = true;
                    }
                }
                if (!exist)metaFiles.push_back("../Library/Meta/" + fileName);
                exist = false;

            }
        } while (FindNextFileA(h, &data));

        FindClose(h);
    }
}

void ResourceManager::LoadMeshResource()
{
    //Check all directions saved on meshesFiles and upload all component mesh
    for (int i = Meshes.size();i < meshesFiles.size();i++)
    {
        GameObject NewModel("NULL");
        ComponentMesh* tempMesh = new ComponentMesh(&NewModel);

        std::ifstream file(meshesFiles[i]);
        if (!file.is_open()) return;
        bool insideMesh = false;
        bool insideObject = false;

        std::string line;
        std::string prevLine;

        while (std::getline(file, line)) {
            if (line == "{") {
                if (prevLine == "Mesh:") insideMesh = true;
                else insideObject = true;
            }
            else if (line == "}") {
                insideMesh = false;
                insideObject = false;
            }
            else if (insideMesh) {
                std::istringstream iss(line);
                std::string key;
                if (std::getline(iss, key, ':')) {
                    std::string value;
                    std::getline(iss, value);
                    if (!value.empty() && value[0] == ' ') value.erase(0, 1);

                    if (key == "IndexCount") {                        
                        tempMesh->mesh.indexCount = std::stoi(value);
                        tempMesh->filenameMesh = meshesFiles[i];
                    }
                    else if (key == "minAABB") {
                        std::stringstream ss(value);
                        ss >> tempMesh->minAABB.x;
                        ss.ignore(1);
                        ss >> tempMesh->minAABB.y;
                        ss.ignore(1);
                        ss >> tempMesh->minAABB.z;
                    }
                    else if (key == "maxAABB") {
                        std::stringstream ss(value);
                        ss >> tempMesh->maxAABB.x;
                        ss.ignore(1);
                        ss >> tempMesh->maxAABB.y;
                        ss.ignore(1);
                        ss >> tempMesh->maxAABB.z;
                    }
                    else if (key == "Texture") {
                        tempMesh->mesh.texture = getTextureResource(value);
                    }
                    else if (key == "Indices") {
                        //Read entire line with indexes separated by '|'
                        std::stringstream ss(value);
                        std::string token;
                        tempMesh->mesh.indices.clear();
                        while (std::getline(ss, token, '|')) {
                            if (!token.empty())
                                tempMesh->mesh.indices.push_back(std::stoi(token));
                        }
                    }
                    else if (key == "Vertices") {
                        // Read entire line with vertex separated by '|'
                        std::stringstream ss(value);
                        std::string token;
                        tempMesh->mesh.vertices.clear();
                        while (std::getline(ss, token, '|')) {
                            if (!token.empty())
                                tempMesh->mesh.vertices.push_back(std::stof(token));
                        }
                    }
                    else if (key == "PositionsLocal") {
                        std::stringstream ss(value);
                        std::string token;
                        tempMesh->mesh.positionsLocal.clear();
                        while (std::getline(ss, token, '|')) {
                            if (!token.empty()) {
                                std::stringstream vecStream(token);
                                float x, y, z;
                                vecStream >> x;
                                vecStream.ignore(1);
                                vecStream >> y;
                                vecStream.ignore(1);
                                vecStream >> z;
                                tempMesh->mesh.positionsLocal.push_back(glm::vec3(x, y, z));
                            }
                        }
                        Meshes.push_back(tempMesh);
                    }
                }
            }
            prevLine = line; // Save last line to check the object type
        }
    }
}
ComponentMesh* ResourceManager::getMeshResource(std::string path)
{
    for (int i = 0; i < meshesFiles.size(); i++)
    {
        if (Meshes[i]->filenameMesh == path)
            return Meshes[i];
    }
    return NULL;
}
Texture* ResourceManager::getTextureResource(std::string path) {
    for (auto& text : textures)
    {
        if (path == text->textPath) return text;
    }
    return NULL;
}
std::string ResourceManager::getMetaResource(std::string path)
{
    for (auto& meta : metaFiles)
    {
        if (path == meta) return meta;
    }
    return "";
}
void ResourceManager::deleteResource(Type type, std::string path)
{
    if (type == Type::MESH)
    {
        Meshes.erase(std::remove(Meshes.begin(), Meshes.end(), getMeshResource(path)), Meshes.end());
        DeleteFileA(path.c_str());
        meshesFiles.clear();
        CheckMeshFiles();
        Application::GetInstance().menus.get()->meshesFiles.clear();
        Application::GetInstance().menus.get()->meshesFiles = meshesFiles;
    }
    else if (type == Type::TEXTURE)
    {
        textures.erase(std::remove(textures.begin(), textures.end(), getTextureResource(path)), textures.end());
        DeleteFileA(path.c_str());
        Application::GetInstance().menus.get()->textures.clear();
        Application::GetInstance().menus.get()->textures = textures;
    }
    else if (type == Type::META)
    {
        metaFiles.erase(std::remove(metaFiles.begin(), metaFiles.end(), getMetaResource(path)), metaFiles.end());
        DeleteFileA(path.c_str());
        Application::GetInstance().menus.get()->metaFiles.clear();
        Application::GetInstance().menus.get()->metaFiles = metaFiles;
    }
}


bool ResourceManager::PostUpdate()
{
    return true;
}

bool ResourceManager::CleanUp()
{

    return true;
}