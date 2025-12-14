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
    enum class Type : unsigned char
    {
        MESH,
        TEXTURE,
        META
    };

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
    void deleteResource(Type type, std::string path);

    ComponentMesh* getMeshResource(std::string path);
    Texture* getTextureResource(std::string path);
    std::string getMetaResource(std::string path);

    std::vector<ComponentMesh*> Meshes;           // Loaded Textures

private:
    std::vector<Texture*> textures;           // Loaded Textures
    std::vector<std::string> meshesFiles;
    std::vector<std::string> metaFiles;

    void LoadMeshResource();

    void CheckMeshFiles();
    void CheckTextureFiles();
    void CheckMetaFiles();
};