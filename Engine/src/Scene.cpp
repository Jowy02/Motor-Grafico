#define NOMINMAX
#include <windows.h>

#include "Application.h"
#include "Render.h"

#include "Window.h"
#include "Scene.h"
#include "GameObject.h"
#include "ResourceManager.h"

#include "Input.h"
#include <iostream> 

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <windows.h>
#include "Menus.h"

#include "LineSegment.h"
#include <cmath> 
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene() : Module()
{
}

// Destructor
Scene::~Scene()
{
}

bool Scene::Awake()
{
    return true;
}

bool Scene::Start()
{
    BuildOctree();
    return true;
}

void Scene::LoadFBX(const std::string& path)
{
    std::string dest = "../Library/FBX/" + std::filesystem::path(path).filename().string();
    int cntModels = models.size();
    int cntMeshes = cntModels;

    if (!std::filesystem::exists(dest)) {
        std::filesystem::create_directories("../Library/FBX");
        std::filesystem::copy_file(path, dest,
            std::filesystem::copy_options::update_existing);
    }

    GameObject model(dest);
    model.modelId = models.size();
    models.push_back(model);
    BuildOctree();

    //Save mesh files
    for (cntModels; cntModels < models.size(); cntModels++)
    {
        dest = "../Library/Meshes/" + models[cntModels].name + ".mesh";
        SaveMesh(dest, models[cntModels]);
    }

    Application::GetInstance().resourceManager.get()->LoadResource();

    //Save meta files
    std::string temp = std::filesystem::path(path).filename().string();
    temp = temp.substr(0, temp.find_last_of('.'));
    dest = "../Library/Meta/" + temp + ".meta";
    
    if (dest != Application::GetInstance().resourceManager.get()->getMetaResource(dest))
    {
        SaveMeta(cntMeshes, dest);
        Application::GetInstance().resourceManager.get()->LoadResource();
    }
}

void Scene::BuildOctree() {
    if (models.empty()) {
        octreeRoot.reset();
        return;
    }

    glm::vec3 globalMin(FLT_MAX), globalMax(-FLT_MAX);
    for (auto& m : models) {
        m.myTransform->UpdateAABB();
        globalMin = glm::min(globalMin, m.myTransform->minAABB);
        globalMax = glm::max(globalMax, m.myTransform->maxAABB);
    }

    // Add a little padding to avoid degenerate boxes
    const float pad = 0.001f;
    globalMin -= glm::vec3(pad);
    globalMax += glm::vec3(pad);

    octreeRoot = std::make_unique<OctreeNode>(globalMin, globalMax, 0, 10, 5, this);

    for (auto& m : models) {
        octreeRoot->Insert(&m);
    }
}

void Scene::ApplyTextureToSelected(const std::string& path)
{
    auto selected = Application::GetInstance().menus.get()->selectedObj;
    bool exist = false;
    if (selected)
    {
        Texture* tempText = Application::GetInstance().resourceManager.get()->getTextureResource(path);
        if (path == tempText->textPath) {
            Application::GetInstance().menus.get()->selectedObj->ApplTexture(tempText, path);
            exist = true;
        }
        if (!exist)
        {
            std::string dest = "../Library/Images/" + std::filesystem::path(path).filename().string();
            if (!std::filesystem::exists(dest)) {
                std::filesystem::create_directories("../Library/Images");
                std::filesystem::copy_file(path, dest,
                    std::filesystem::copy_options::update_existing);
                Application::GetInstance().resourceManager.get()->LoadResource();
            }
            Texture* tex = new Texture(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
            Application::GetInstance().menus.get()->selectedObj->ApplTexture(tex, path);
        }
    }
    else
    {
        Application::GetInstance().menus->LogToConsole("ERROR APPLYING TEXTURE, NO OBJECT SELECTED");
    }
}
void Scene::SelectObject(GameObject* obj)
{
    auto* menus = Application::GetInstance().menus.get();

    if (menus->selectedObj != obj) {
        menus->selectedObj = obj;
        selected = true;
    }
    else {
        selected = false;
        menus->selectedObj = nullptr;
    }
}

void Scene::RaycastFromMouse(int mouseX, int mouseY)
{
    auto camera = Application::GetInstance().camera.get();
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 proj = camera->GetProjectionMatrix();

    LineSegment ray = Application::GetInstance().camera->GenerateRayFromMouse(mouseX, mouseY,
        Application::GetInstance().window->width,
        Application::GetInstance().window->height,
        view, proj);

    Raycast(ray); 
}

bool Scene::RayIntersectsTriangle(const LineSegment& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t)
{
    const float EPSILON = 1e-8f;
    glm::vec3 dir = ray.Direction();
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);

    if (fabs(a) < EPSILON) return false; // Parallel

    float f = 1.0f / a;
    glm::vec3 s = ray.a - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * glm::dot(edge2, q);
    return t > EPSILON;
}


bool Scene::RayIntersectsAABB(const LineSegment& ray, const glm::vec3& boxMin, const glm::vec3& boxMax, float& t)
{
    glm::vec3 dir = ray.Direction();

    // Avoid division by zero
    glm::vec3 invDir;
    invDir.x = (dir.x != 0.0f) ? 1.0f / dir.x : std::numeric_limits<float>::infinity();
    invDir.y = (dir.y != 0.0f) ? 1.0f / dir.y : std::numeric_limits<float>::infinity();
    invDir.z = (dir.z != 0.0f) ? 1.0f / dir.z : std::numeric_limits<float>::infinity();

    glm::vec3 t0s = (boxMin - ray.a) * invDir;
    glm::vec3 t1s = (boxMax - ray.a) * invDir;

    glm::vec3 tmin = glm::min(t0s, t1s);
    glm::vec3 tmax = glm::max(t0s, t1s);

    float tNear = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float tFar = glm::min(glm::min(tmax.x, tmax.y), tmax.z);

    // If all the AABB is behind the lightning
    if (tFar < 0.0f) return false;
    // If there is no overlap
    if (tNear > tFar) return false;

    // If the ray starts inside the AABB, we use tFar
    t = (tNear >= 0.0f) ? tNear : tFar;
    return true;
}

void Scene::Raycast(const LineSegment& ray)
{
    std::vector<GameObject*> hitModels;
    if (octreeRoot) {
        octreeRoot->CollectObjectsHitByRay(ray, this, hitModels);

        std::cout << "Cantidad modelos entregados por octree: "
            << hitModels.size() << std::endl;

    }
    float closestT = FLT_MAX;
    GameObject* selected = nullptr;

    for (auto* model : hitModels)
    {
        glm::mat4 modelMatrix = model->myTransform->transformMatrix;

        const auto& indices = model->myMesh->mesh.indices;
        const auto& positions = model->myMesh->mesh.positionsLocal;

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            size_t index0 = indices[i];
            size_t index1 = indices[i + 1];
            size_t index2 = indices[i + 2];

            glm::vec3 v0 = glm::vec3(
                modelMatrix * glm::vec4(positions[index0], 1.0f)
            );
            glm::vec3 v1 = glm::vec3(
                modelMatrix * glm::vec4(positions[index1], 1.0f)
            );
            glm::vec3 v2 = glm::vec3(
                modelMatrix * glm::vec4(positions[index2], 1.0f)
            );

            float t;
            if (RayIntersectsTriangle(ray, v0, v1, v2, t))
            {
                if (t < closestT)
                {
                    closestT = t;
                    selected = model;
                }
            }
        }
    }

    if (selected)
    {
        SelectObject(selected);
        Application::GetInstance().menus->LogToConsole(
            Application::GetInstance().menus->selectedObj ?
            "Selected: " + Application::GetInstance().menus->selectedObj->name :
            "Deselected");
        Application::GetInstance().menus.get()->selectedCamera = NULL;
        Application::GetInstance().menus.get()->selectedResourcePath = "";
    }
}
bool Scene::PreUpdate()
{
    BuildOctree();
    return true;
}

void Scene::ImGuizmo() {
    if (selected && Application::GetInstance().input->click && Application::GetInstance().menus->selectedObj != NULL) {
        auto* menus = Application::GetInstance().menus.get();

        ImGuizmo::BeginFrame();
        ImGuizmo::SetRect(0, 0, Application::GetInstance().window->width, Application::GetInstance().window->height);

        glm::mat4 view = Application::GetInstance().camera->GetViewMatrix();
        glm::mat4 proj = Application::GetInstance().camera->GetProjectionMatrix();
        glm::mat4 model = menus->selectedObj->myTransform->transformMatrix;

        ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
        switch (Application::GetInstance().scene->currentGizmo) {
        case GizmoOperation::TRANSLATE: op = ImGuizmo::TRANSLATE; break;
        case GizmoOperation::ROTATE:    op = ImGuizmo::ROTATE;    break;
        case GizmoOperation::SCALE:     op = ImGuizmo::SCALE;     break;
        }

        ImGuizmo::Manipulate(glm::value_ptr(view),
            glm::value_ptr(proj),
            op,
            ImGuizmo::LOCAL,
            glm::value_ptr(model));

        if (ImGuizmo::IsUsing()) {

            glm::mat4 parentMatrix = glm::mat4(1.0f);
            if (menus->selectedObj->isChild) {
                parentMatrix = Application::GetInstance().scene->models[menus->selectedObj->ParentID].myTransform->transformMatrix;
            }

            // Convert world to local
            glm::mat4 localModel = glm::inverse(parentMatrix) * model;

            // Break down local
            float translation[3], rotationDeg[3], scaleArr[3];
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localModel), translation, rotationDeg, scaleArr);

            menus->selectedObj-> myTransform->position = glm::vec3(translation[0], translation[1], translation[2]);
            menus->selectedObj-> myTransform->rotation = glm::vec3(rotationDeg[0], rotationDeg[1], rotationDeg[2]);
            menus->selectedObj-> myTransform->scale = glm::vec3(scaleArr[0], scaleArr[1], scaleArr[2]);

            menus->selectedObj->UpdateTransform();
            BuildOctree();
        }
    }
}

Camera* Scene::GetActiveCamera()
{
    auto& menus = Application::GetInstance().menus;
    if (menus->selectedCamera)
        return menus->selectedCamera;

    return Application::GetInstance().camera.get();
}

bool Scene::Update(float dt)
{
    Camera* activeCamera = Application::GetInstance().camera.get();
    if (activeCamera)
    {
        activeCamera->UpdateProjectionMatrix();

        frustum.Update(activeCamera->GetVPMatrix(
            activeCamera->FOV,
            activeCamera->nearPlane,
            activeCamera->farPlane
        ));
    }

    Application::GetInstance().render->OrderModels();
    Application::GetInstance().render->FrustumModels();

    ImGuizmo();

    return true;
}
void Scene::SaveMesh(std::string filePath, GameObject model)
{
    std::ofstream file(filePath);
    if (!file.is_open()) return;

    file << "Mesh:\n";

    file << "{" << "\n";
    file << "IndexCount: " << model.myMesh->mesh.indexCount << "\n";
    if(model.myMesh->mesh.texture !=NULL)file << "Texture: " << model.myMesh->mesh.texture->textPath.c_str() << "\n";
    else file << "Texture: " << "" << "\n";
    
    file << "minAABB: " << model.myTransform->minAABB.x << ", " << model.myTransform->minAABB.y << ", " << model.myTransform->minAABB.z << "\n";
    file << "maxAABB: " << model.myTransform->maxAABB.x << ", " << model.myTransform->maxAABB.y << ", " << model.myTransform->maxAABB.z << "\n";

    file << "Indices:";
    for (auto& indice : model.myMesh->mesh.indices)
        file << indice << "|";
    file << "\n";

    file << "Vertices:";
    for (auto& indice : model.myMesh->mesh.vertices)
        file << indice << "|";
    file << "\n";

    file << "PositionsLocal:";
    for (auto& positionsLocal : model.myMesh->mesh.positionsLocal)
        file << positionsLocal.x << ", " << positionsLocal.y << ", " << positionsLocal.z << "|";
    file << "\n";

    file << "}" << "\n";

    file.close();
}
void Scene::SaveMeta(int meshesId, std::string filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open()) return;
    for (meshesId; meshesId < models.size();meshesId++)
    {
        file << "Mesh:\n";

        file << "{" << "\n";
        file << "MeshRef: " << models[meshesId].modelPath.c_str() << "\n";
        file << "Texture: " << "" << "\n"; 

        file << "}" << "\n";
    }
    file.close();
}

void Scene::SaveScene(std::string filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open()) return;
    for (const auto& camera : cameras) {
        file << "Cameras:\n";
        file << "{" << "\n";

        file << "Name: " << camera->CameraName << "\n";
        file << "Orientation: " << camera->Orientation.x <<"," << camera->Orientation.y <<","<< camera->Orientation.z << "\n";
        file << "FOV: " << camera->FOV << "\n";
        file << "NearPlane: " << camera->nearPlane << "\n";
        file << "FarPlane: " << camera->farPlane << "\n";
        file << "MovesSpeed: " << camera->MOVESPEED << "\n";
        file << "Sensitivity: " << camera->sensitivity << "\n";
        file << "Position: " << camera->Position.x <<","<< camera->Position.y << "," << camera->Position.z << "\n";
        file << "}" << "\n";
        
    }
    for (const auto& model : models) {
        if (model.name != "Grid" && model.componentID < 0)
        {
            file << "GameObjects:\n";
            file << "{" << "\n";

            file << "UID: " << model.modelId << "\n";
            file << "Path: " << model.modelPath << "\n";
            file << "Component: " << model.componentID << "\n";
            file << "Name: " << model.name << "\n";
            file << "ParentUID: " << model.ParentID << "\n";
            file << "Translation: " << model.myTransform->position.x << ", " << model.myTransform->position.y << ", " << model.myTransform->position.z << "\n";
            file << "Scale: " << model.myTransform->scale.x << ", " << model.myTransform->scale.y << ", " << model.myTransform->scale.z << "\n";
            file << "Rotation: " << model.myTransform->rotation.x << ", " << model.myTransform->rotation.y << ", " << model.myTransform->rotation.z << "\n";
            file << "Texture: " << model.texturePath << "\n";
            file << "}" << "\n";
        }
    }
    file.close();
}
void Scene::ClearScene()
{
    if (!models.empty()) {
        GameObject grid = models.front(); //Save the Grid (assuming index 0)
        models.clear();                   // Delete all objects
        models.push_back(grid);           // Reinsert the Grid

        // Clean and reset Octree
        if (octreeRoot) {
            octreeRoot->Clear();
            octreeRoot.reset();
        }

        Application::GetInstance().menus.get()->selectedObj = nullptr;
    }
    if (cameras.size() > 1) {
        cameras.clear();
        cameras.push_back(Application::GetInstance().camera.get());
    }

    if (models.size() > 1) {
        for (int i = models.size() - 1; i >= 0; --i) {
            auto& model = models[i];

            if (model.name != "Grid")
            {
                Application::GetInstance().menus.get()->selectedObj = nullptr;

                if (model.isChild) models[model.ParentID].eraseChild(model.modelId);
                model.CleanUpChilds();
                auto& sceneModels = models;
                sceneModels.erase(std::remove_if(sceneModels.begin(), sceneModels.end(),
                    [&](const GameObject& m) { return &m == &models[model.modelId]; }), sceneModels.end());

                if (models.size() <= 1) {
                    break;
                }
                Application::GetInstance().scene.get()->octreeRoot.get()->Clear();
            }
        }
    }
}
void Scene::LoadScene(std::string filePath)
{
    if (!models.empty()) {
        GameObject grid = models.front(); // Save Grid (always on index 0)
        models.clear();                   // Delete all objects
        models.push_back(grid);           // Reinsert the Grid

        // Clean y reset Octree
        if (octreeRoot) {
            octreeRoot->Clear();
            octreeRoot.reset();
        }

        Application::GetInstance().menus.get()->selectedObj = nullptr;
    }
    if (cameras.size() > 1) {
        cameras.clear();
        cameras.push_back(Application::GetInstance().camera.get());
    }

    std::ifstream file(filePath);
    if (!file.is_open()) return;
    std::string ModelName;
    std::string line;
    std::string prevline = "";

    int C_UID = -1;
    int UID = -1;
    int tempId = -1;
    bool components = false;
    bool insideObject = false;
    bool insideCam = false;

    if (models.size() > 1) {
        deleteScene = true;
        for (int i = models.size() - 1; i >= 0; --i) {
            auto& model = models[i];

            if (model.name != "Grid")
            {
                Application::GetInstance().menus.get()->selectedObj = nullptr;

                if (model.isChild) models[model.ParentID].eraseChild(model.modelId);
                model.CleanUpChilds();
                auto& sceneModels = models;
                sceneModels.erase(std::remove_if(sceneModels.begin(), sceneModels.end(),
                    [&](const GameObject& m) { return &m == &models[model.modelId]; }), sceneModels.end());

                if (models.size() <= 1) {
                    deleteScene = false;
                    break;
                }
                Application::GetInstance().scene.get()->octreeRoot.get()->Clear();
            }
        }
    }

    if (!deleteScene)
    {
        while (std::getline(file, line)) {

            if (line == "{" && prevline == "GameObjects:") {
                insideObject = true;
            }
 
            else if (line == "{" && prevline == "Cameras:")
            {
                insideCam = true;
            }
            prevline = line;

            if (insideObject) {
                // Parse key : value
                std::istringstream iss(line);
                std::string key;
                if (std::getline(iss, key, ':')) {
                    std::string value;
                    std::getline(iss, value);
                    // Clean space between words
                    if (!value.empty() && value[0] == ' ') value.erase(0, 1);

                    if (key == "Name") {
                        if (value.size() >= 4 && value.substr(0, 4) == "Cube") {
                            Application::GetInstance().render.get()->CreateCube();
                            UID = models.size() - 1;

                        }
                        else if (value.size() >= 7 && value.substr(0, 7) == "Pyramid") {
                            Application::GetInstance().render.get()->CreatePyramid();
                            UID = models.size() - 1;

                        }
                        else if (value.size() >= 6 && value.substr(0, 6) == "Sphere") {
                            Application::GetInstance().render.get()->CreateSphere();
                            UID = models.size() - 1;

                        }
                        else if (value.size() >= 7 && value.substr(0, 7) == "Diamond") {
                            Application::GetInstance().render.get()->CreateDiamond();
                            UID = models.size() - 1;

                        }
                    }
                    if (key == "Path")
                    {
                        if (value != "")
                        {
                            //Check if mesh exist if no go to next object
                            if(LoadMesh(value))
                            {
                                insideObject = true;
                                UID = models.size() - 1;
                                models[UID].modelId = UID;
                            }
                            else insideObject = false;
                        }
                    }
                    else if (key == "ParentUID")
                    {
                        models[UID].ParentID = std::stoi(value);
                    }
                    else if (key == "Name")
                    {
                        models[UID].name = value;
                    }
                    else if (key == "Translation") {
                        std::stringstream ss(value);
                        ss >> models[UID].myTransform->position.x;
                        ss.ignore(1);
                        ss >> models[UID].myTransform->position.y;
                        ss.ignore(1);
                        ss >> models[UID].myTransform->position.z;
                        models[UID].UpdateTransform();

                    }
                    else if (key == "Scale") {
                        std::stringstream ss(value);
                        ss >> models[UID].myTransform->scale.x;
                        ss.ignore(1);
                        ss >> models[UID].myTransform->scale.y;
                        ss.ignore(1);
                        ss >> models[UID].myTransform->scale.z;
                        models[UID].UpdateTransform();

                    }
                    else if (key == "Rotation") {
                        std::stringstream ss(value);
                        ss >> models[UID].myTransform->rotation.x;
                        ss.ignore(1);
                        ss >> models[UID].myTransform->rotation.y;
                        ss.ignore(1);
                        ss >> models[UID].myTransform->rotation.z;
                        models[UID].UpdateTransform();
                    }
                    else if (key == "Texture") {
                        models[UID].ApplTexture(Application::GetInstance().resourceManager.get()->getTextureResource(value), value);
                    }
                }
            }
            else if (insideCam)
            {
                std::istringstream iss(line);
                std::string key;
                if (std::getline(iss, key, ':')) {
                    std::string value;
                    std::getline(iss, value);
                    // Clean space between words
                    if (!value.empty() && value[0] == ' ') value.erase(0, 1);
                    if (key == "Name"){
                        if (C_UID < 0) C_UID = 0;
                        else {
                            Camera* NewCamera = new Camera;
                            cameras.push_back(NewCamera);
                            C_UID += 1;
                        }
                        cameras[C_UID]->CameraName = value;
                    }
                    else if (key == "Orientation") {
                        std::stringstream ss(value);
                        ss >> cameras[C_UID]->Orientation.x;
                        ss.ignore(1);
                        ss >> cameras[C_UID]->Orientation.y;
                        ss.ignore(1);
                        ss >> cameras[C_UID]->Orientation.z;
                    }
                    else if (key == "FOV") {
                        cameras[C_UID]->FOV = std::stof(value);
                    }
                    else if (key == "NearPlane") {
                        cameras[C_UID]->nearPlane = std::stof(value);
                    }
                    else if (key == "FarPlane") {
                        cameras[C_UID]->farPlane = std::stof(value);
                    }
                    else if (key == "MovesSpeed") {
                        cameras[C_UID]->MOVESPEED = std::stof(value);
                    }
                    else if (key == "Sensitivity") {
                        cameras[C_UID]->sensitivity = std::stof(value);
                    }
                    else if (key == "Position") {
                        std::stringstream ss(value);
                        ss >> cameras[C_UID]->Position.x;
                        ss.ignore(1);
                        ss >> cameras[C_UID]->Position.y;
                        ss.ignore(1);
                        ss >> cameras[C_UID]->Position.z;
                    }
                }
            }
        }

        for (const auto& model : models)
        {
            if (model.ParentID != -1) {
                int parent = model.ParentID;
                models[model.modelId].ParentID = 0;
                models[parent].SetChild(&models[model.modelId]);
            }
        }
        BuildOctree();
    }
    cameras[0]->UpdateViewMatrix();
    cameras[0]->UpdateProjectionMatrix();
}
void Scene::LoadMeta(std::string filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) return;

    std::string line;
    std::string prevline = "";
    bool insideMesh = false;

    while (std::getline(file, line)) {

        if (line == "{" && prevline == "Mesh:") {
            insideMesh = true;
        }
        else if (line == "}") {
            insideMesh = false;
        }
        prevline = line;
        if (insideMesh) {
            // Parse key : value
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, ':')) {
                std::string value;
                std::getline(iss, value);
                // Clean space between words
                if (!value.empty() && value[0] == ' ') value.erase(0, 1);

                if (key == "MeshRef") {
                    LoadMesh(value);
                }
            }
        }
    }
}
bool Scene::LoadMesh(std::string filePath)
{
    GameObject NewModel(filePath);
    ComponentMesh* mesh = Application::GetInstance().resourceManager.get()->getMeshResource(filePath);
    if (mesh != NULL)
    {
        NewModel.myMesh = mesh;
        NewModel.myTransform->minAABB = NewModel.myMesh->minAABB;
        NewModel.myTransform->maxAABB = NewModel.myMesh->maxAABB;
        NewModel.modelPath = filePath;
        NewModel.actualTexture = NewModel.myMesh->mesh.texture;
        NewModel.saveTexture = NewModel.myMesh->mesh.texture;

        NewModel.myTransform->center = (NewModel.myTransform->minAABB + NewModel.myTransform->maxAABB) * 0.5f;
        NewModel.myTransform->size = NewModel.myTransform->maxAABB - NewModel.myTransform->minAABB;
        NewModel.myTransform->localMinAABB = NewModel.myTransform->minAABB;
        NewModel.myTransform->localMaxAABB = NewModel.myTransform->maxAABB;

        NewModel.myTransform->position = { 0,0,0 };
        NewModel.myTransform->rotation = { 0,0,0 };
        NewModel.myTransform->scale = { 1,1,1 };
        NewModel.myTransform->scale *= NewModel.myMesh->initialScale;

        if (!NewModel.myMesh->importTexture)
            NewModel.actualTexture = NULL;
        if(NewModel.actualTexture != NULL)NewModel.texturePath = NewModel.actualTexture->textPath;
        NewModel.modelId = (int)models.size();

        NewModel.UpdateTransform();
        NewModel.myTransform->UpdateAABB();

        models.push_back(NewModel);
        models.back().myMesh->RecreateBuffers();

        if (!octreeRoot) {
            Application::GetInstance().scene->BuildOctree();
        }
        else {
            OctreeNode* root = octreeRoot.get();
            root->Insert(&models.back());
        }
        return true;
    }
    return false;
}

bool Scene::PostUpdate()
{
    return true;
}

bool Scene::CleanUp()
{
    Application::GetInstance().menus->LogToConsole("Scene::CleanUp started");
    ClearScene();
    imagesFiles.clear();
    for (auto& tex : images)
        tex.Delete();
    images.clear();

    for (auto& model : models)
    {
        model.CleanUpChilds();
        model.CleanUp();     
    }

    Application::GetInstance().menus->LogToConsole("Scene::CleanUp completed");
    return true;
}

void Scene::RecreateGameObject(const InitialGameObjectData& blueprint)
{
    GameObject* newObjPtr = nullptr;

    if (blueprint.modelPath.empty() || blueprint.modelPath == "")
    {
        if (blueprint.name.find("Cube") != std::string::npos) {
            Application::GetInstance().render.get()->CreateCube();
        }
        else if (blueprint.name.find("Pyramid") != std::string::npos) {
            Application::GetInstance().render.get()->CreatePyramid();
        }
        else if (blueprint.name.find("Sphere") != std::string::npos) {
            Application::GetInstance().render.get()->CreateSphere();
        }
        else if (blueprint.name.find("Diamond") != std::string::npos) {
            Application::GetInstance().render.get()->CreateDiamond();
        }
        else if (blueprint.name.find("Grid") != std::string::npos) {
        }

        if (!models.empty()) {
            newObjPtr = &models.back();
        }
    }
    else
    {
        LoadMesh(blueprint.modelPath);
        if (!models.empty())
            newObjPtr = &models.back();
    }

    if (!newObjPtr) {
        Application::GetInstance().menus->LogToConsole("ERROR: Failed to recreate GameObject: " + blueprint.name);
        return;
    }

    GameObject& newObj = *newObjPtr;

    // Textura
    if (!blueprint.texturePath.empty())
    {
        Texture* tex = Application::GetInstance().menus->GetLoadedTexture(blueprint.texturePath);
        if (!tex)
        {
            Application::GetInstance().menus->LogToConsole("Advertencia: Recargando textura: " + blueprint.texturePath);
            tex = new Texture(blueprint.texturePath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        }
        newObj.ApplTexture(tex, blueprint.texturePath);
    }
    else
        newObj.ApplTexture(nullptr, "");

    newObj.name = blueprint.name;
    newObj.myTransform->position = blueprint.pos;
    newObj.myTransform->rotation = blueprint.rot;
    newObj.myTransform->scale = blueprint.scale;
    newObj.isHidden = blueprint.isHidden;
    newObj.modelId = models.size() - 1;
    newObj.SetInitialParentID(blueprint.parentID);
    newObj.UpdateTransform();
}