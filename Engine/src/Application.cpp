#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Camera.h"
#include "Menus.h"

// Constructor
Application::Application() 
{
    window = std::make_shared<Window>();
    input = std::make_shared<Input>();
    render = std::make_shared<Render>();
    scene = std::make_shared<Scene>();
    camera = std::make_shared<Camera>();
    menus = std::make_shared<Menus>();
    // Ordered for awake / Start / Update // Reverse order of CleanUp
    AddModule(std::static_pointer_cast<Module>(window));
    AddModule(std::static_pointer_cast<Module>(input));
    AddModule(std::static_pointer_cast<Module>(menus));
    AddModule(std::static_pointer_cast<Module>(scene));
    AddModule(std::static_pointer_cast<Module>(render));
    AddModule(std::static_pointer_cast<Module>(camera));
}

// Static method to get the instance of the Engine class, following the singletn pattern
Application& Application::GetInstance() 
{
    static Application instance;
    return instance;
}

void Application::AddModule(std::shared_ptr<Module> module) 
{
    module->Init();
    moduleList.push_back(module);
}

// Called before render is available
bool Application::Awake() 
{
    bool result = true;
    for (const auto& module : moduleList) 
    {
        result = module.get()->Awake();
        if (!result)
            break;
    }
    return result;
}

// Called before the first frame
bool Application::Start() 
{
    bool result = true;
    for (const auto& module : moduleList) 
    {
        result = module.get()->Start();
        if (!result)
            break;
    }
    return result;
}

// Called each loop iteration
bool Application::Update() 
{
    bool ret = true;
    if (requestExit)
        return false;

    PrepareUpdate();
    if (input->GetWindowEvent(WE_QUIT) == true)
        ret = false;

    if (ret == true)
        ret = PreUpdate();

    if (ret == true)
        ret = DoUpdate();

    if (ret == true)
        ret = PostUpdate();
    FinishUpdate();
    return ret;
}

// Called before quitting
bool Application::CleanUp() 
{
    bool result = true;
    for (const auto& module : moduleList) 
    {
        result = module.get()->CleanUp();
        if (!result)
            break;
    }
    return result;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
    uint64_t now = SDL_GetPerformanceCounter();
    if (perfLastTime == 0)
        perfLastTime = now;

    dt = (float)(now - perfLastTime) / (float)SDL_GetPerformanceFrequency();
    perfLastTime = now;

    //frameTime.Start();
    frameStart = SDL_GetTicks();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
   window.get()->SetTitle("SuperNova_Engine");
}

// Call modules before each loop iteration
bool Application::PreUpdate()
{
    bool result = true;
    for (const auto& module : moduleList) 
    {
        result = module.get()->PreUpdate();
        if (!result)
            break;
    }
    return result;
}

// Call modules on each loop iteration
bool Application::DoUpdate()
{
    bool result = true;
    for (const auto& module : moduleList) 
    {
        result = module.get()->Update(dt);
        if (!result)
            break;
    }
    return result;
}

// Call modules after each loop iteration
bool Application::PostUpdate()
{
    bool result = true;
    for (const auto& module : moduleList) 
    {
        result = module.get()->PostUpdate();
        if (!result)
            break;
    }
    return result;
}
