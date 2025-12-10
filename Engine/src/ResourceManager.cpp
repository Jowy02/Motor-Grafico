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
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA("..\\Library\\Images\\*", &data);
    std::string fileName;
    bool exist = false;

    fbxFiles.clear();
    textures.clear();
    meshesFiles.clear();

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
                if(!exist)textures.push_back(tex);
                exist = false;

            }
        } while (FindNextFileA(h, &data));

        FindClose(h);
    }

    h = FindFirstFileA("..\\Library\\FBX\\*", &data);
    fileName = "";
    exist = false;
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
                continue;
            fileName = data.cFileName;

            if (fileName.substr(fileName.size() - 4) == ".fbx" || fileName.substr(fileName.size() - 4) == ".FBX") {
                for (auto& files : fbxFiles)
                {
                    std::string TempfileName = "../Library/FBX/" + fileName;
                    if (files == TempfileName) {
                        exist = true;
                    }
                }
                if(!exist)fbxFiles.push_back("../Library/FBX/" + fileName);
                exist = false;

            }
            if (fileName.substr(fileName.size() - 4) == ".txt") {
                txtFiles.push_back("../Library/FBX/" + fileName);
            }
        } while (FindNextFileA(h, &data));

        FindClose(h);
    }
    
    h = FindFirstFileA("..\\Library\\Meshes\\*", &data);
    fileName = "";
    exist = false;
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
                continue;
            fileName = data.cFileName;

            if (fileName.substr(fileName.size() - 4) == ".txt") {
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

    Application::GetInstance().menus.get()->fbxFiles.clear();
    Application::GetInstance().menus.get()->fbxFiles = fbxFiles;

    Application::GetInstance().menus.get()->textures.clear();
    Application::GetInstance().menus.get()->textures = textures;

    Application::GetInstance().menus.get()->meshesFiles.clear();
    Application::GetInstance().menus.get()->meshesFiles = meshesFiles;
}
bool ResourceManager::PostUpdate()
{


    return true;
}

bool ResourceManager::CleanUp()
{

    return true;
}