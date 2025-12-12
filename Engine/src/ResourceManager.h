#pragma once

#include "Module.h"
#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include <vector>
#include <string>

struct SDL_Texture;

class ResourceManager: public Module
{
public:
    ResourceManager();
    virtual ~ResourceManager();

    // Lifecycle methods
    bool Awake() override;      // Called before render is available
    bool Start() override;      // Called before the first frame
    bool Update(float dt);      // Called each frame
    bool PreUpdate() override;  // Called before all Updates
    bool PostUpdate() override; // Called after all Updates
    bool CleanUp() override;    // Called before quitting

    void LoadResource();
    bool init = true;

    std::vector<GameObject> models;           // Loaded models
    std::vector<Texture*> textures;           // Loaded Textures

    std::vector<std::string> meshesFiles;
    std::vector<ComponentMesh*> Meshes;           // Loaded Textures

private:
    void LoadMeshResource();

    std::vector<std::string> fbxFiles;
    std::vector<std::string> txtFiles;
};