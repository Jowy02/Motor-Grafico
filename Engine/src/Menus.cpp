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

    // Intentar abrir el archivo en modo lectura para saber si ya se ha inicializado o no
    FILE* file = fopen(path.c_str(), "r");
    if (file) {
        fclose(file);  // cerrar el archivo si existe
        initialization_exist = true;
    }
    else {
        initialization_exist = false;
    }

    ImGui::StyleColorsDark();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Docking
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
    //Dock space
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

void Menus::MainMenu() {
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
        if (ImGui::BeginMenu("Create"))
        {
            selectedObj = NULL;
            if (ImGui::MenuItem("Triangulo"))
            {
                Application::GetInstance().render.get()->CreateTriangle();
            }
            if (ImGui::MenuItem("Cubo"))
            {
                Application::GetInstance().render.get()->CreateCube();
            }
            if (ImGui::MenuItem("Rombo"))
            {
                Application::GetInstance().render.get()->CreateDiamond();
            }
            if (ImGui::MenuItem("Sphere"))
            {
                Application::GetInstance().render.get()->CreateSphere();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Menus::BuildDockSpace() {
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
    style.WindowPadding = ImVec2(0.0f, 0.0f);      // Sin margen interno en ventanas
    //style.FramePadding = ImVec2(0.0f, 0.0f);       // Sin margen en botones y elementos
    //style.ItemSpacing = ImVec2(0.0f, 0.0f);        // Sin espacio entre elementos
    style.WindowBorderSize = 0.0f;                 // Sin borde de ventana
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

        // Dividir verticalmente el nodo raíz
        ImGuiID dock_top, dock_bottom, dock_center;
        dock_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.2f, nullptr, &dockspace_id);
        dock_bottom = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_id);
        dock_center = dockspace_id;

        // Dividir horizontalmente el centro
        ImGuiID dock_left, dock_right;
        dock_left = ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Left, 0.2f, nullptr, &dock_center);
        dock_right = ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Right, 0.2f, nullptr, &dock_center);

        // Asignar ventanas
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
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

    if (timeAccumulator >= 1.0f) // cada segundo
    {
        currentFPS = (float)framesCounter / timeAccumulator;
        fpsHistory.push_back(currentFPS);

        if (fpsHistory.size() > 100) // límite para el gráfico
            fpsHistory.erase(fpsHistory.begin());

        framesCounter = 0;
        timeAccumulator = 0.0f;
    }
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

        ImGui::Text("TRANSFORM");
        if(ImGui::DragFloat3("Position", &selectedObj->position.x, 0.1f))selectedObj->UpdateTransform();
        if (ImGui::DragFloat3("Rotation", &selectedObj->rotation.x, 0.1f))selectedObj->UpdateTransform();
        if (ImGui::DragFloat3("Scale", &selectedObj->scale.x, 0.1f))selectedObj->UpdateTransform();

        ImGui::Separator();

        ImGui::Text("MESH");
        ImGui::Text("Size: (%.2f, %.2f, %.2f)",
            selectedObj->size.x,
            selectedObj->size.y,
            selectedObj->size.z);
        //TO DO opción para visualizar la textura de los cuadrados blancos y negros

        ImGui::Separator();

        ImGui::Text("TEXTURE");
        ImGui::Text("Path: %s", selectedObj->texturePath.c_str());
        //TO DO TAMAÑO TEXTURE
        if (ImGui::Checkbox("Default texture", &checkbox)) selectedObj->switchTexture(checkbox, "BlackWhite");
        if (ImGui::Checkbox("Normal Map", &checkbox2)) selectedObj->switchTexture(checkbox2, "NormalMap");

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
void Menus::DrawSystemConfig()
{
    ImGui::Begin("System Config", &showSystemInfo);

    // Window Config
    ImGui::Text("Window");
    int with = Application::GetInstance().window.get()->width;
    int height = Application::GetInstance().window.get()->height;

    ImGui::DragInt("Width", &with, 0.1f);
    ImGui::DragInt("Height", &height, 0.1f);
    Application::GetInstance().window.get()->GetWindowSize(with,height);

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

    // Engine name
    ImGui::Text("Motor Gr�fico");
    ImGui::Separator();

    // Version
    ImGui::Text("Version: 1.0.0");

    // Team members
    ImGui::Text("Team:");
    ImGui::BulletText("Joel Vicente");
    ImGui::BulletText("Arthur Cordoba");
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

bool Menus::CleanUp() {
    LogToConsole("Shutting down ImGui...");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    LogToConsole("Scene::CleanUp completed");

    return true;
}