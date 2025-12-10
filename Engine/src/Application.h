#pragma once

#include <memory>
#include <list>

#include "Module.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

// Forward declarations for modules
class Window;
class Input;
class Camera;
class Render;
class Scene;
class Menus;
class Mesh;
class SimulationController;
class ResourceManager;

class Application
{
public:
    // Singleton access
    static Application& GetInstance();

    // Module management
    void AddModule(std::shared_ptr<Module> module);

    // Lifecycle
    bool Awake();       // Called before render is available
    bool Start();       // Called before the first frame
    bool Update();      // Called each loop iteration
    bool CleanUp();     // Called before quitting

private:
    // Private constructor to enforce singleton
    Application();

    // Delete copy operations
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Internal update steps
    void PrepareUpdate(); // Before loop iteration
    void FinishUpdate();  // After loop iteration

    bool PreUpdate();     // Update modules before main update
    bool DoUpdate();      // Update modules
    bool PostUpdate();    // Update modules after main update

    std::list<std::shared_ptr<Module>> moduleList;

public:
    // Engine states
    enum EngineState
    {
        CREATE = 1,
        AWAKE,
        START,
        LOOP,
        CLEAN,
        FAIL,
        EXIT
    };

    // --- MODULES ---
    std::shared_ptr<Window> window;
    std::shared_ptr<Input> input;
    std::shared_ptr<Render> render;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Menus> menus;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<SimulationController> simulationController;
    std::shared_ptr<ResourceManager> resourceManager;

    bool requestExit = false;

private:
    // Timing
    float dt = 0.0f;       // Delta time
    int frames = 0;

    int frameCount = 0;
    int framesPerSecond = 0;
    int lastSecFrameCount = 0;

    bool showControls = false;

    float averageFps = 0.0f;
    int secondsSinceStartup = 0;

    const int maxFrameDuration = 16; // Max frame duration in milliseconds

    Uint32 frameStart = 0;
    Uint32 frameTime = 0;

    // FPS limiting
    const int targetFPS = 60;
    const int frameDelay = 1000 / targetFPS;

    uint64_t perfLastTime = 0;
};