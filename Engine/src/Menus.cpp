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
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                Application::GetInstance().requestExit = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::MenuItem("FPS Monitor", nullptr, &showFPS);
            ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
            ImGui::MenuItem("System Info", nullptr, &showSystemInfo);
            ImGui::EndMenu();
        }

        // Help menu
        if (ImGui::BeginMenu("Help"))
        {
            // Open GitHub documentation
            if (ImGui::MenuItem("GitHub Documentation"))
                ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico", 0, 0, SW_SHOWNORMAL);

            // Open GitHub issues page
            if (ImGui::MenuItem("Report a Bug"))
                ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico/issues", 0, 0, SW_SHOWNORMAL);

            // Open GitHub releases page
            if (ImGui::MenuItem("Download Latest"))
                ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico/releases", 0, 0, SW_SHOWNORMAL);

            ImGui::Separator();

            // About window toggle
            ImGui::MenuItem("About", nullptr, &showAbout);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

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

    if (showConsole) DrawConsole();
    if (showFPS) FPS_graph();
    if (showHierarchy) Hierarchy_Menu();
    if (showSystemInfo) DrawSystemInfo();
    if (showAbout) DrawAboutWindow();

    return true;
}

void Menus::FPS_graph()
{
    ImGui::Begin("FPS Monitor", &showFPS);
    ImGui::Text("FPS actual: %.1f", currentFPS);
    if (!fpsHistory.empty()) {
        ImGui::PlotLines("FPS", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 120.0f, ImVec2(100, 50));
    }
    ImGui::End();
}

void Menus::DrawConsole()
{
    ImGui::Begin("Console", &showConsole);

    for (const auto& line : consoleLog) {
        ImGui::TextUnformatted(line.c_str());
    }

    ImGui::End();
}
void Menus::Hierarchy_Menu()
{
    ImGui::Begin("Hierarchy", &showHierarchy);

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
void Menus::DrawInspector()
{
    ImGui::Begin("Inspector");

    if (!selectedObj == NULL) {
        ImGui::Text("Seleccionado: %s", selectedObj->name.c_str());
        ImGui::Separator();
        if(ImGui::DragFloat3("Position", &selectedObj->position.x, 0.1f))selectedObj->UpdateTransform();
        if (ImGui::DragFloat3("Rotation", &selectedObj->rotation.x, 0.1f))selectedObj->UpdateTransform();
        if (ImGui::DragFloat3("Scale", &selectedObj->scale.x, 0.1f))selectedObj->UpdateTransform();

        ImGui::Separator();

        ImGui::Text("Size: (%.2f, %.2f, %.2f)",
            selectedObj->size.x,
            selectedObj->size.y,
            selectedObj->size.z);

    }
    else {
        ImGui::Text("Ning�n objeto seleccionado");
    }

    ImGui::End();
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
    ImGui::Begin("System Info", &showSystemInfo);

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

void Menus::DrawAboutWindow()
{
    ImGui::Begin("About", &showAbout);

    // Engine name
    ImGui::Text("Motor Gr�fico");
    ImGui::Separator();

    // Version
    ImGui::Text("Version: 1.0.0");

    // Team members
    ImGui::Text("Team:");
    ImGui::BulletText("Joel Vicente");
    ImGui::BulletText("Arthur C�rdoba");
    ImGui::BulletText("Jana Puig");

    ImGui::Separator();

    // Libraries used
    ImGui::Text("Libraries used:");
    ImGui::BulletText("SDL3");
    ImGui::BulletText("OpenGL");
    ImGui::BulletText("Assimp");
    ImGui::BulletText("ImGui");
    ImGui::BulletText("DevIL");
    ImGui::BulletText("GLM");
    ImGui::BulletText("GLAD");

    ImGui::Separator();

    // License
    ImGui::Text("License...");
    ImGui::TextWrapped(
        "...."
    );

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

    
    return true;
}