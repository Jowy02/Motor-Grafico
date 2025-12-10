#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "GameObject.h"
#include "Menus.h"
#include "Input.h"
#include "SimulationController.h"
#include "ResourceManager.h"
#include "Time.h"

#include <iostream> 

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <windows.h>

#include "imgui_internal.h"
#include "ImGuiFileDialog.h"
#include <filesystem>

IGFD::FileDialogConfig config;

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
    std::filesystem::create_directories("../Library/Images"); 
    std::filesystem::create_directories("../Library/FBX");
    Application::GetInstance().menus->LogToConsole("Initializing ImGui...");
    config.path = "../Library";

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

    Application::GetInstance().scene.get()->cameras.push_back(Application::GetInstance().camera.get());

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

    //DrawSimulationToolbar();

    CalculateFPS(dt);

    if (showConsole) DrawConsole();
    if (showFPS) FPS_graph();
    if (showHierarchy) Hierarchy_Menu();
    if (showSystemInfo) DrawSystemInfo();
    if (showAbout) DrawAboutWindow();
    if(showInspector)DrawInspector();
    if(showSystemConfig)DrawSystemConfig();
    if (isLoad || isSave)SaveLoad();

    DrawResourceManager();
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
            ImGui::MenuItem("System Config", nullptr, &showSystemConfig);
            ImGui::MenuItem("System Inspector", nullptr, &showInspector);

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
            if (Application::GetInstance().input->click) {
                Application::GetInstance().input->click = false;

            }
            selectedObj = NULL;
            if (ImGui::MenuItem("Pyramid"))
                Application::GetInstance().render.get()->CreatePyramid();
            if (ImGui::MenuItem("Cube"))
                Application::GetInstance().render.get()->CreateCube();
            if (ImGui::MenuItem("Diamond"))
                Application::GetInstance().render.get()->CreateDiamond();
            if (ImGui::MenuItem("Sphere"))
                Application::GetInstance().render.get()->CreateSphere();
            if (ImGui::MenuItem("Camera"))
            {
               
                Camera* newCamera = new Camera;
                newCamera->CameraName = "Camera" + std::to_string(Application::GetInstance().scene.get()->cameras.size());
                Application::GetInstance().scene.get()->cameras.push_back(newCamera);
                if (Application::GetInstance().simulationController->GetState() == GameState::RUNNING) {
                    Application::GetInstance().simulationController->OnCameraCreatedDuringPlay(newCamera);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene"))
        {
            selectedObj = NULL;
            if(ImGui::MenuItem("Save", nullptr, &isSave)) ImGuiFileDialog::Instance()->OpenDialog("ChooseScene", "Choose Scene File", ".txt,.json", config);
            if (ImGui::MenuItem("Load", nullptr, &isLoad))ImGuiFileDialog::Instance()->OpenDialog("ChooseScene", "Choose Scene File", ".txt,.json", config);


            ImGui::EndMenu();
        }

        DrawSimulationToolbar();
        ImGui::EndMainMenuBar();
    }
}

void Menus::DrawSimulationToolbar()
{
    SimulationController* simController = Application::GetInstance().simulationController.get();
    GameState currentState = simController->GetState();

    // PLAY / PAUSE
    if (currentState == GameState::STOPPED || currentState == GameState::PAUSED) {
        if (ImGui::Button("PLAY")) {
            simController->Play();
            Application::GetInstance().menus->LogToConsole("Simulation Started/Resumed");
        }
    }
    else if (currentState == GameState::RUNNING) {
        if (ImGui::Button("PAUSE")) {
            simController->Pause();
            Application::GetInstance().menus->LogToConsole("Simulation Paused/Resumed");
        }
    }

    ImGui::SameLine();

    // STOP
    bool stopDisabled = (currentState == GameState::STOPPED);
    if (stopDisabled) ImGui::BeginDisabled();
    if (ImGui::Button("STOP")) {
        simController->Stop();
        Application::GetInstance().menus->LogToConsole("Simulation Stopped and Reset");
    }
    if (stopDisabled) ImGui::EndDisabled();

    ImGui::SameLine();

    // STEP (One Frame)
    if (ImGui::Button("STEP")) {
        simController->Step();
        Application::GetInstance().menus->LogToConsole("Simulation Step requested (one frame).");
    }

    ImGui::SameLine();

    // Faster / Slower
    if (ImGui::Button("<< Slower")) {
        Time::SetTimeScale(Time::timeScale - 0.25f);
    }
    ImGui::SameLine();
    if (ImGui::Button("Faster >>")) {
        Time::SetTimeScale(Time::timeScale + 0.25f);
    }

    ImGui::SameLine();

    // TimeScale slider
    float ts = Time::timeScale;
    if (ImGui::SliderFloat("TimeScale", &ts, Time::minTimeScale, Time::maxTimeScale, "%.2f")) {
        Time::SetTimeScale(ts);
    }

    // Mostrar estados y relojes

    ImGui::Begin("Debug");
    ImGui::Text("State: %s", (currentState == GameState::STOPPED) ? "STOPPED" : (currentState == GameState::RUNNING ? "RUNNING" : "PAUSED"));
    ImGui::SameLine();
    ImGui::Text(" | Game Time: %.2f s | Real Time: %.2f s | dt (game): %.4f s", (float)Time::time, (float)Time::realTime, Time::deltaTime);
    ImGui::End();

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
        ImGui::DockBuilderDockWindow("System Config", dock_right);
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
    timeAccumulator += Time::realDeltaTime;

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
    if (selectedResourcePath != "" && (selectedObj != NULL || selectedCamera != NULL))
    {
        selectedObj = NULL;
        selectedCamera = NULL;
    }
    for (auto& camera : Application::GetInstance().scene.get()->cameras) {
        if (ImGui::TreeNodeEx(camera, ImGuiTreeNodeFlags_DefaultOpen, camera->CameraName.c_str()))
        {
            if (ImGui::IsItemClicked()) {
                if (selectedCamera != camera)
                {
                    selectedCamera = camera;
                    selectedObj = nullptr;
                    selectedResourcePath = "";
                }
                else selectedCamera = nullptr;
            }
            ImGui::TreePop();
        }
    }

    for (auto& Model : Application::GetInstance().scene.get()->models) {
        if(!Model.isChild)DrawGameObjectNode(&Model);
    }

    ImGui::End();
}

void Menus::DrawGameObjectNode(GameObject* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

    // Tree node
    bool nodeOpen = ImGui::TreeNodeEx((void*)obj, flags, "%s", obj->name.c_str());

    // Selección con click
    if (ImGui::IsItemClicked())
    {
        if (selectedObj != obj) { 
            selectedObj = obj;
        }
        else selectedObj = NULL;
    }

    if (ImGui::BeginDragDropSource())
    {
        //Ppuntero al objeto arrastrado
        ImGui::SetDragDropPayload("OBJECT_NODE", &obj, sizeof(obj));
        ImGui::Text("Mover %s", obj->name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_NODE"))
        {
            GameObject* draggedObj = *(GameObject**)payload->Data;
            if (draggedObj != obj && draggedObj->ParentID != obj->modelId)
            {
                obj->SetChild(draggedObj);
                selectedObj = NULL;
            }
            else {
                selectedObj = NULL;
            }
            
        }
        ImGui::EndDragDropTarget();
    }
    
    if (nodeOpen)
    {
        // Dibujar hijos del objeto
        for (auto& child : obj->childrenID)
            DrawGameObjectNode(&Application::GetInstance().scene.get()->models[child]);

        ImGui::TreePop();
    }
}

void Menus::DrawInspector()
{
    ImGui::Begin("Inspector");

    if (selectedObj != nullptr)
    {
        ImGui::Text("Selected: %s  id: %d", selectedObj->name.c_str(), selectedObj->modelId);

        if (ImGui::CollapsingHeader("TRANSFORM", ImGuiTreeNodeFlags_DefaultOpen)) {

            if (ImGui::DragFloat3("Position", &selectedObj->position.x, 0.1f)) {
                selectedObj->UpdateTransform();

            }
            if (ImGui::DragFloat3("Rotation", &selectedObj->rotation.x, 0.1f)) {
                selectedObj->UpdateTransform();

            }
            if (ImGui::DragFloat3("Scale", &selectedObj->scale.x, 0.1f)) {
                selectedObj->UpdateTransform();
            }
        }
        if (selectedObj->name != "Grid")
        {
            if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::Text("Size: (%.2f, %.2f, %.2f)", selectedObj->size.x, selectedObj->size.y, selectedObj->size.z);

                Render* render = Application::GetInstance().render.get();
                ImGui::Checkbox("Show Face Normals", &render->FaceNormals);
                ImGui::Checkbox("Show Vertex Normals", &render->VertexNormals);
            }

            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (selectedObj->actualTexture != NULL) {
                    ImGui::Text("Path: %s", selectedObj->texturePath.c_str());

                    ImGui::Text("Size: %d %d", selectedObj->actualTexture->width, selectedObj->actualTexture->height);
                    ImGui::Image(selectedObj->actualTexture->ID, ImVec2(150, 150));
                    if (ImGui::Checkbox("Default texture", &checkbox))
                        selectedObj->switchTexture(checkbox, "BlackWhite");
                    if (ImGui::Checkbox("Hide texture", &checkbox2))
                        selectedObj->switchTexture(checkbox2, "Hide");
                }
                else  ImGui::Text("No texture on this object");
                ImGui::Separator();
                ImGui::Checkbox("Hide Model", &selectedObj->isHidden);
            }
            if (ImGui::Button("Delete Model"))
            {
                if (Application::GetInstance().input->click) {
                    Application::GetInstance().input->click = false;
                }
                Application::GetInstance().scene.get()->octreeRoot.get()->Clear();
                if (selectedObj->isChild)Application::GetInstance().scene.get()->models[selectedObj->ParentID].eraseChild(selectedObj->modelId);
                selectedObj->CleanUpChilds();
                int id = selectedObj->modelId;
                auto& sceneModels = Application::GetInstance().scene->models;
                sceneModels.erase(std::remove_if(sceneModels.begin(), sceneModels.end(),
                    [&](const GameObject& m) { return &m == selectedObj; }), sceneModels.end());

                for (id; id < Application::GetInstance().scene.get()->models.size();id++)
                {
                    Application::GetInstance().scene.get()->models[id].modelId -= 1;
                }
                selectedObj = nullptr;
                Application::GetInstance().scene->BuildOctree();
                if (Application::GetInstance().simulationController->GetState() == GameState::STOPPED) {
                    Application::GetInstance().simulationController.get()->SaveInitialSceneState();

                }
            }
        }
    }
    else if (selectedCamera != nullptr)
    {
        Camera* cam = selectedCamera;
        ImGui::Text(cam->CameraName.c_str());
        ImGui::Separator();

        if (ImGui::DragFloat3("Position", &cam->Position.x, 0.1f))
        {
            cam->Inputs(Application::GetInstance().window->window);
        }

        glm::vec3 eulerAngles = glm::degrees(glm::vec3(
            atan2(cam->Orientation.y, sqrt(cam->Orientation.x * cam->Orientation.x + cam->Orientation.z * cam->Orientation.z)),
            atan2(cam->Orientation.x, cam->Orientation.z),
            0.0f
        ));

        if (ImGui::DragFloat3("Rotation", &eulerAngles.x, 0.5f))
        {
            // Convertir euler a orientación de la cámara
            float pitch = glm::radians(eulerAngles.x);
            float yaw = glm::radians(eulerAngles.y);

            cam->Orientation.x = cos(pitch) * sin(yaw);
            cam->Orientation.y = sin(pitch);
            cam->Orientation.z = cos(pitch) * cos(yaw);
            cam->Orientation = glm::normalize(cam->Orientation);

            cam->UpdateViewMatrix();
        }

        if (ImGui::DragFloat("FOV", &cam->FOV, 1.0f, 10.0f, 120.0f))
        {
            cam->UpdateProjectionMatrix();
        }

        if (ImGui::DragFloat("Near Plane", &cam->nearPlane, 0.01f, 0.01f, cam->farPlane - 0.01f))
        {
            cam->UpdateProjectionMatrix();
        }

        if (ImGui::DragFloat("Far Plane", &cam->farPlane, 0.1f, cam->nearPlane + 0.01f, 10000.0f))
        {
            cam->UpdateProjectionMatrix();
        }
        ImGui::DragFloat("Move Speed", &cam->MOVESPEED, 0.05f, 0.0f, 10.0f);
        ImGui::DragFloat("Sensitivity", &cam->sensitivity, 0.01f, 0.0f, 1.0f);

        if (selectedCamera->CameraName != "MainCamera")
        {
            if (ImGui::Button("Set us MainCamera"))
            {
                Application::GetInstance().camera.get()->ChangeCamera(selectedCamera);
                selectedCamera = Application::GetInstance().camera.get();

            }

            if (ImGui::Button("Delete Camera"))
            {
                auto& cams = Application::GetInstance().scene->cameras;

                cams.erase(std::remove(cams.begin(), cams.end(), cam), cams.end());

                if (Application::GetInstance().simulationController->GetState() == GameState::RUNNING) {
                    Application::GetInstance().simulationController->OnCameraRemovedDuringPlay(cam);
                }
                else {
                    delete cam;
                }
                selectedCamera = nullptr;

            }
        }
    }
    else if (selectedResourcePath != "")
    {
        ImGui::Text("Resource : %s", selectedResourcePath.c_str());
        Texture* selectedTexture = nullptr;

        if (ImGui::Button("Delete Selected")) {

            switch (selectedResourceType) 
            {
                case ResourceType::Texture:
                    // liberar memoria y borrar del vector
                    for (auto& text : textures)
                        if (text->textPath == selectedResourcePath) selectedTexture = text;

                    textures.erase(std::remove(textures.begin(), textures.end(), selectedTexture), textures.end());
                    selectedTexture = nullptr;
                    break;

                case ResourceType::Fbx:
                    fbxFiles.erase(std::remove(fbxFiles.begin(), fbxFiles.end(), selectedResourcePath), fbxFiles.end());
                    break;

                case ResourceType::Mesh:
                    meshesFiles.erase(std::remove(meshesFiles.begin(), meshesFiles.end(), selectedResourcePath), meshesFiles.end());
                    break;

                default: 
                    break;
            }
            DeleteFileA(selectedResourcePath.c_str());

            selectedResourceType = ResourceType::None;
            selectedResourcePath.clear();
            Application::GetInstance().resourceManager.get()->LoadResource();
        }
        ImGui::Separator();
    }
    else
    {
        ImGui::Text("No object selected");
    }
    ImGui::End();
}

Texture* Menus::GetLoadedTexture(const std::string& path)
{
    for (Texture* tex : textures)
    {
        if (tex->textPath == path)
        {
            return tex; 
        }
    }
    return nullptr; // No encontrada.
}
void Menus::SaveLoad()
{
    if (ImGuiFileDialog::Instance()->Display("ChooseScene")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

            if (isLoad)Application::GetInstance().scene.get()->LoadScene(filePath);
            else if (isSave){
                std::ofstream("filePath");
                Application::GetInstance().scene.get()->SaveScene(filePath);
            }
            isSave = false;
            isLoad = false;
            ImGuiFileDialog::Instance()->Close();
        }
        isSave = false;
        isLoad= false;
        ImGuiFileDialog::Instance()->Close();
    }
}
void Menus::DrawResourceManager() 
{
    ImGui::Begin("Resource Manager");

    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("TexturesTable", 8)) {
            for (int i = 0; i < textures.size(); i++) {
                ImGui::TableNextColumn();

                ImGui::BeginGroup();
                if (ImGui::Selectable(textures[i]->textPath.c_str(), selectedResourcePath == textures[i]->textPath)) {
                    selectedResourcePath = textures[i]->textPath;
                    selectedResourceType = ResourceType::Texture;
                }
                ImGui::Image(textures[i]->ID, ImVec2(150, 150));
                ImGui::EndGroup();

                // Drag & Drop 
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    dragTexture = i;
                    draged = true;
                    ImGui::Text("Arrastrando %s", textures[dragTexture]->textPath.c_str());
                    ImGui::SetDragDropPayload("TEXTURE_POINTER", &textures[dragTexture],
                        textures[dragTexture]->textPath.size() + 1);
                    ImGui::EndDragDropSource();
                }

                if (!ImGui::GetDragDropPayload() && draged) {
                    Application::GetInstance().scene->ApplyTextureToSelected(textures[dragTexture]->textPath.c_str());
                    draged = false;
                }
            }
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("MeshTable", 8)) {

            for (int i = 0; i < meshesFiles.size(); i++) {
                ImGui::TableNextColumn();

                ImGui::BeginGroup();
                if (ImGui::Selectable(meshesFiles[i].c_str(), selectedResourcePath == meshesFiles[i])) 
                {
                    selectedResourcePath = meshesFiles[i];
                    selectedResourceType = ResourceType::Mesh;
                }

                ImGui::EndGroup();

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

                    dragMesh = i;
                    dragedMesh = true;

                    ImGui::Text("Arrastrando %s", meshesFiles[dragMesh].c_str());
                    ImGui::SetDragDropPayload("TEXTURE_POINTER", &meshesFiles[dragMesh], meshesFiles[dragMesh].size() + 1);

                    ImGui::EndDragDropSource();

                }
                if (!ImGui::GetDragDropPayload() && dragedMesh)
                {
                    Application::GetInstance().menus.get()->selectedObj = NULL;
                    Application::GetInstance().scene->LoadMesh(meshesFiles[dragMesh]);
                    dragedMesh = false;
                }
            }
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Fbx", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("FbxTable", 8)) {

            for (int i = 0; i < fbxFiles.size(); i++) {
                ImGui::TableNextColumn();

                ImGui::BeginGroup();
                if (ImGui::Selectable(fbxFiles[i].c_str(), selectedResourcePath == fbxFiles[i]))
                {
                    selectedResourcePath = fbxFiles[i];
                    selectedResourceType = ResourceType::Fbx;
                }
                ImGui::EndGroup();

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

                    dragFbx = i;
                    dragedFbx = true;

                    ImGui::Text("Arrastrando %s", fbxFiles[dragFbx].c_str());
                    ImGui::SetDragDropPayload("TEXTURE_POINTER", &fbxFiles[dragFbx], fbxFiles[dragFbx].size() + 1);

                    ImGui::EndDragDropSource();

                }
                if (!ImGui::GetDragDropPayload() && dragedFbx)
                {
                    Application::GetInstance().menus.get()->selectedObj = NULL;
                    Application::GetInstance().scene->LoadFBX(fbxFiles[dragFbx]);
                    dragedFbx = false;
                }
            }
            ImGui::EndTable();
        }
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
    ImGui::Text("SuperNova_Engine");
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