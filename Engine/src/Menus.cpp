#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Model.h"
#include "Menus.h"
#include <iostream> 

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <windows.h>

Menus::Menus() : Module()
{
}
// Destructor
Menus::~Menus()
{
}

bool Menus::Awake()
{
    return true;
}


bool Menus::Start()
{

    Application::GetInstance().menus->LogToConsole("Initializing ImGui...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    SDL_Window* window = Application::GetInstance().window->GetSDLWindow(); // Asegúrate de tener este método
    SDL_GLContext gl_context = Application::GetInstance().window->GetGLContext(); // También necesitas esto

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    LogToConsole("ImGui initialized successfully");
    return true;
}


bool Menus::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    return true;
}

bool Menus::Update(float dt)
{
    
    framesCounter++;
    timeAccumulator += dt;

    if (timeAccumulator >= 1.0f) // cada segundo
    {
        currentFPS = (float)framesCounter / timeAccumulator;
        fpsHistory.push_back(currentFPS);

        if (fpsHistory.size() > 100) // límite para el gráfico
            fpsHistory.erase(fpsHistory.begin());

        framesCounter = 0;
        timeAccumulator = 0.0f;

        std::cout << "FPS: " << currentFPS << std::endl;

    }

    DrawConsole();
    FPS_graph();
    Hierarchy_Menu();
    DrawSystemInfo();
    return true;
}

void Menus::FPS_graph()
{
    ImGui::Begin("FPS Monitor");
    ImGui::Text("FPS actual: %.1f", currentFPS);
    if (!fpsHistory.empty()) {
        ImGui::PlotLines("FPS", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 120.0f, ImVec2(100, 50));
    }
    ImGui::End();
}

void Menus::DrawConsole()
{
    ImGui::Begin("Console");

    for (const auto& line : consoleLog) {
        ImGui::TextUnformatted(line.c_str());
    }

    ImGui::End();
}
void Menus::Hierarchy_Menu()
{
    ImGui::Begin("Hierarchy");

    for (auto& Model : Application::GetInstance().scene.get()->models) DrawGameObjectNode(&Model);

    ImGui::End();
}
void Menus::DrawGameObjectNode(Model* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (obj == selectedObj)
        flags |= ImGuiTreeNodeFlags_Selected;
    bool nodeOpen = ImGui::TreeNodeEx((void*)obj, flags, "%s", obj->name.c_str());

    if (ImGui::IsItemClicked())
        selectedObj = obj;

    if (nodeOpen) {
        ImGui::TreePop();
    }
}


float Menus::GetRAMUsageMB()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    return physMemUsed / (1024.0f * 1024.0f);
}

void Menus::DrawSystemInfo()
{
    ImGui::Begin("System Info");

    // Memory usage
    ImGui::Text("Memory Usage:");
    ImGui::Text("RAM: %.2f MB", GetRAMUsageMB());

    // Hardware info
    ImGui::Separator();
    ImGui::Text("Hardware:");
    ImGui::Text("CPU: %s", SDL_GetPlatform());
    ImGui::Text("GPU: %s", (const char*)glGetString(GL_RENDERER));
    ImGui::Text("OpenGL Vendor: %s", (const char*)glGetString(GL_VENDOR));

    // Library versions
    ImGui::Separator();
    ImGui::Text("Library Versions:");

    ImGui::Text("SDL Version: %s", SDL_GetRevision());

    ImGui::Text("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
    ImGui::Text("DevIL Version: %d", IL_VERSION);

    ImGui::End();
}

bool Menus::PostUpdate()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

bool Menus::CleanUp() {
    LogToConsole("Shutting down ImGui...");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    LogToConsole("Scene::CleanUp completed");

    Application::GetInstance().menus->LogToConsole("Menus::CleanUp started");
    
    return true;
}