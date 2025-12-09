#pragma once

#include "Module.h"
#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include <vector>
#include <string>

struct SDL_Texture;

class Menus : public Module
{
public:
    Menus();
    virtual ~Menus();

    // Lifecycle methods
    bool Awake() override;      // Called before render is available
    bool Start() override;      // Called before the first frame
    bool Update(float dt);      // Called each frame
    bool PreUpdate() override;  // Called before all Updates
    bool PostUpdate() override; // Called after all Updates
    bool CleanUp() override;    // Called before quitting

    // --- Console / Logging ---
    void DrawConsole();
    void LogToConsole(const std::string& msg) { consoleLog.push_back(msg); }
    std::vector<std::string> consoleLog;

    // --- FPS / Performance ---
    void CalculateFPS(float dt);
    void FPS_graph();
    float currentFPS = 0.0f;

    // --- Scene / Hierarchy ---
    void Hierarchy_Menu();
    void DrawGameObjectNode(GameObject* model);
    void DrawInspector();
    void DrawResourceManager();
    GameObject* selectedObj = nullptr;

    // --- System Info ---
    float GetRAMUsageMB();
    void DrawSystemInfo();
    void DrawSystemConfig();
    void DrawAboutWindow();

    // --- GUI / Docking ---
    void MainMenu();
    void BuildDockSpace();

    void LoadFbx();
    void LoadTxt();

    void LoadTextures();
    bool init = true;

    Texture* GetLoadedTexture(const std::string& path);

    void DrawSimulationToolbar();
private:
    std::vector<GameObject> models;           // Loaded models
    std::vector<Texture*> textures;           // Loaded models
    std::vector<float> fpsHistory;       // FPS history for graphs

    uint32_t lastFrameTime = 0;
    int framesCounter = 0;
    float timeAccumulator = 0.0f;

    // GUI visibility flags
    bool showConsole = true;
    bool showFPS = true;
    bool showHierarchy = true;
    bool showSystemInfo = true;
    bool showAbout = false;
    bool showInspector = true;
    bool showSystemConfig = true;

    bool isSave = false;
    bool isLoad = false;
    // Example checkbox states
    bool checkbox = false;
    bool checkbox2 = false;

    bool initialization_exist = false;

    bool draged = false;
    int dragTexture = 0;

    int dragFbx = 0;
    bool dragedFbx = false;

    int dragMesh = 0;
    bool dragedMesh = false;

    std::vector<std::string> fbxFiles;
    std::vector<std::string> txtFiles;
    std::vector<std::string> meshesFiles;

    Camera* selectedCamera = nullptr;

};