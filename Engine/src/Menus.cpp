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
#include "imgui_internal.h"

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "imgui_layout.ini";
    std::string path = "imgui_layout.ini";

    // Try opening the file in read mode to check if ImGui was previously initialized
    FILE* file = fopen(path.c_str(), "r");
    if (file) {
        fclose(file);
        initialization_exist = true;
    }
    else {
        initialization_exist = false;
    }

    ImGui::StyleColorsDark();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    ImGui::StyleColorsDark();

    SDL_Window* window = Application::GetInstance().window->GetSDLWindow();
    SDL_GLContext gl_context = Application::GetInstance().window->GetGLContext();

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
    // Dock Space
    BuildDockSpace();
    MainMenu();

    CalculateFPS(dt);

    if (showConsole) DrawConsole();
    if (showFPS) FPS_graph();
    if (showHierarchy) Hierarchy_Menu();
    if (showSystemInfo) DrawSystemInfo();
    if (showAbout) DrawAboutWindow();
    DrawInspector();

    DrawSystemConfig();
    return true;
}

void Menus::MainMenu() 
{
    if (ImGui::BeginMainMenuBar())
    {   
        // --- FILE MENU ---
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
                Application::GetInstance().requestExit = true;
            ImGui::EndMenu();
        }

        // --- VIEW MENU ---
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::MenuItem("FPS Monitor", nullptr, &showFPS);
            ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
            ImGui::MenuItem("System Info", nullptr, &showSystemInfo);
            ImGui::EndMenu();
        }

        // --- HELP MENU ---
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("GitHub Documentation"))
                ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico", 0, 0, SW_SHOWNORMAL);

            if (ImGui::MenuItem("Report a Bug"))
                ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico/issues", 0, 0, SW_SHOWNORMAL);

            if (ImGui::MenuItem("Download Latest"))
                ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico/releases", 0, 0, SW_SHOWNORMAL);

            ImGui::Separator();
            ImGui::MenuItem("About", nullptr, &showAbout);
            ImGui::EndMenu();
        }

        // --- GEOMETRY CREATION MENU ---
        if (ImGui::BeginMenu("Create"))
        {
            selectedObj = NULL;
            if (ImGui::MenuItem("Pyramid"))
                Application::GetInstance().render.get()->CreatePyramid();
            if (ImGui::MenuItem("Cube"))
                Application::GetInstance().render.get()->CreateCube();
            if (ImGui::MenuItem("Diamond"))
                Application::GetInstance().render.get()->CreateDiamond();
            if (ImGui::MenuItem("Sphere"))
                Application::GetInstance().render.get()->CreateSphere();
            
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Menus::BuildDockSpace() 
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();


    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(0.0f, 0.0f);      // No internal padding for windows
    // style.FramePadding = ImVec2(0.0f, 0.0f);    // No padding for buttons or elements
    // style.ItemSpacing = ImVec2(0.0f, 0.0f);     // No spacing between items
    style.WindowBorderSize = 0.0f;                 // No border for windows
    style.IndentSpacing = 0.0f;

    ImGui::Begin("MainDockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    static bool layoutInitialized = false;

    if (!layoutInitialized && !initialization_exist)
    {
        ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        // Split the root node vertically
        ImGuiID dock_top, dock_bottom, dock_center;
        dock_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.2f, nullptr, &dockspace_id);
        dock_bottom = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_id);
        dock_center = dockspace_id;

        // Split the center area horizontally
        ImGuiID dock_left, dock_right;
        dock_left = ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Left, 0.2f, nullptr, &dock_center);
        dock_right = ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Right, 0.2f, nullptr, &dock_center);

        // Assign windows to each docking area
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
        ImGui::DockBuilderDockWindow("System Config", dock_right);
        ImGui::DockBuilderDockWindow("Console", dock_bottom);
        ImGui::DockBuilderDockWindow("FPS Monitor", dock_bottom);
        ImGui::DockBuilderDockWindow("System Info", dock_bottom);
        
        ImGui::DockBuilderFinish(dockspace_id);

        layoutInitialized = true;
    }

    ImGui::End();
}

void Menus::CalculateFPS(float dt)
{
    framesCounter++;
    timeAccumulator += dt;

    if (timeAccumulator >= 1.0f)
    {
        currentFPS = (float)framesCounter / timeAccumulator;
        fpsHistory.push_back(currentFPS);

        if (fpsHistory.size() > 100)
            fpsHistory.erase(fpsHistory.begin());

        framesCounter = 0;
        timeAccumulator = 0.0f;
    }
}

void Menus::FPS_graph()
{
    ImGui::Begin("FPS Monitor", &showFPS);
    ImGui::Text("FPS actual: %.1f", currentFPS);
    if (!fpsHistory.empty()) 
        ImGui::PlotLines("FPS", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 120.0f, ImVec2(100, 50));
    
    ImGui::End();
}

void Menus::DrawConsole()
{
    ImGui::Begin("Console", &showConsole);

    for (const auto& line : consoleLog) 
        ImGui::TextUnformatted(line.c_str());

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

    if (nodeOpen)
        ImGui::TreePop();
}

void Menus::DrawInspector()
{
    ImGui::Begin("Inspector");

    if (selectedObj != nullptr)
    {
        ImGui::Text("Selected: %s", selectedObj->name.c_str());
        ImGui::Separator();

        ImGui::Text("TRANSFORM");
        if (ImGui::DragFloat3("Position", &selectedObj->position.x, 0.1f)) 
            selectedObj->UpdateTransform();
        if (ImGui::DragFloat3("Rotation", &selectedObj->rotation.x, 0.1f)) 
            selectedObj->UpdateTransform();
        if (ImGui::DragFloat3("Scale", &selectedObj->scale.x, 0.1f)) 
            selectedObj->UpdateTransform();

        ImGui::Separator();
        ImGui::Text("MESH");
        ImGui::Text("Size: (%.2f, %.2f, %.2f)", selectedObj->size.x, selectedObj->size.y, selectedObj->size.z);

        Render* render = Application::GetInstance().render.get();
        ImGui::Checkbox("Show Face Normals", &render->FaceNormals);
        ImGui::Checkbox("Show Vertex Normals", &render->VertexNormals);

        ImGui::Separator();
        ImGui::Text("TEXTURE");
        ImGui::Text("Path: %s", selectedObj->texturePath.c_str());
        if (ImGui::Checkbox("Default texture", &checkbox)) 
            selectedObj->switchTexture(checkbox, "BlackWhite");
        if (ImGui::Checkbox("Hide texture", &checkbox2)) 
            selectedObj->switchTexture(checkbox2, "Hide");

        ImGui::Separator();
        ImGui::Checkbox("Hide Model", &selectedObj->isHidden);

        if (ImGui::Button("Delete Model")) 
        {
            auto& sceneModels = Application::GetInstance().scene->models;
            sceneModels.erase(
                std::remove_if(sceneModels.begin(), sceneModels.end(),
                    [&](const Model& m) { return &m == selectedObj; }),
                sceneModels.end()
            );
            selectedObj = nullptr;
        }
    }
    else
    {
        ImGui::Text("No object selected");
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

    // Memory Usage
    ImGui::Text("Memory Usage:");
    ImGui::Text("RAM: %.2f MB", GetRAMUsageMB());

    // Hardware Info
    ImGui::Separator();
    ImGui::Text("Hardware:");
    ImGui::Text("CPU: %s", SDL_GetPlatform());
    ImGui::Text("GPU: %s", (const char*)glGetString(GL_RENDERER));
    ImGui::Text("OpenGL Vendor: %s", (const char*)glGetString(GL_VENDOR));

    // Library Versions
    ImGui::Separator();
    ImGui::Text("Library Versions:");
    ImGui::Text("SDL Version: %s", SDL_GetRevision());
    ImGui::Text("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
    ImGui::Text("DevIL Version: %d", IL_VERSION);
    ImGui::End();
}

void Menus::DrawSystemConfig()
{
    ImGui::Begin("System Config", &showSystemInfo);

    // Window Config
    ImGui::Text("Window");
    int width = 0, height = 0;
    SDL_GetWindowSize(Application::GetInstance().window.get()->window, &width, &height);

    if (ImGui::DragInt("Width", &width, 0.1f) || ImGui::DragInt("Height", &height, 0.1f))
    {
        Application::GetInstance().window.get()->SetWindowSize(width, height);
        glViewport(0, 0, width, height);
    }
      
    // Camera Config
    ImGui::Separator();
    ImGui::Text("Camera");

    ImGui::DragFloat("Move Speed", &Application::GetInstance().camera.get()->MOVESPEED, 0.05f, 0.0f, 1.0f);
    ImGui::DragFloat("Sensitivity", &Application::GetInstance().camera.get()->sensitivity, 0.05f,0.0f,1.0f);

    glm::vec3 Position = Application::GetInstance().camera.get()->Position;
    ImGui::Text("Position: (%.2f, %.2f, %.2f)",
        Position.x,
        Position.y,
        Position.z);

    glm::vec3 Orientation = Application::GetInstance().camera.get()->Orientation;
    ImGui::Text("Orientation: (%.2f, %.2f, %.2f)",
        Orientation.x,
        Orientation.y,
        Orientation.z);

    ImGui::End();
}
void Menus::DrawAboutWindow()
{
    ImGui::Begin("About", &showAbout);

    // Engine Name
    ImGui::Text("Game Engine");
    ImGui::Separator();

    // Version
    ImGui::Text("Version: 1.0.0");

    // Team Members
    ImGui::Text("Team:");
    ImGui::BulletText("Joel Vicente");
    ImGui::BulletText("Arthur Cordoba");
    ImGui::BulletText("Jana Puig");
    ImGui::Separator();

    // Libraries Used
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
    if (ImGui::MenuItem("MIT Licence"))
        ShellExecuteA(0, "open", "https://github.com/Jowy02/Motor-Grafico/blob/main/LICENCE.md", 0, 0, SW_SHOWNORMAL);

    ImGui::End();
}

bool Menus::PostUpdate()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

bool Menus::CleanUp() 
{
    LogToConsole("Shutting down ImGui...");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    LogToConsole("Scene::CleanUp completed");

    return true;
}