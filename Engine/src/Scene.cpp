#define NOMINMAX
#include <windows.h>

#include "Application.h"
#include "Render.h"

#include "Window.h"
#include "Scene.h"
#include "Model.h"
#include "Input.h"
#include <iostream> 

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <windows.h>
#include "Menus.h"

#include "LineSegment.h"
#include <cmath> 
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
    //Application::GetInstance().scene->LoadFBX("../FBX/BakerHouse.fbx");

    //Texture* tex = new Texture("../Images/Baker_house.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    //models[0].texturePath = "../Images/Baker_house.png";

    //models[0].Mmesh.texture = tex;
    //models[0].actualTexture = tex;
    //models[0].modelId = 0;

//   std::string parentDir = std::string("../Images/");
//imagesFiles.push_back(std::string("textura.png"));
//
//for (size_t i = 0; i < imagesFiles.size(); ++i)
//{
//    std::string fullPath = parentDir + imagesFiles[i];
//    // Crear la textura con tu clase Texture (usa DevIL internamente)
//    Texture tex(fullPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + i, GL_RGBA, GL_UNSIGNED_BYTE);
//    images.push_back(tex);
//}
    BuildOctree();
	return true;
}

void Scene::LoadFBX(const std::string& path) 
{
    Model model(path.c_str());
    model.modelId = models.size();
    models.push_back(model);
    BuildOctree();
    if (octreeRoot) {
        OctreeNode* root = octreeRoot.get();
        root->Insert(&models.back());
    }
}

void Scene::BuildOctree() {
    glm::vec3 globalMin(FLT_MAX), globalMax(-FLT_MAX);
    for (auto& m : models) {
        m.UpdateTransform();
        std::cout << "[AABB world] " << m.name
            << " min(" << m.minAABB.x << "," << m.minAABB.y << "," << m.minAABB.z << ")"
            << " max(" << m.maxAABB.x << "," << m.maxAABB.y << "," << m.maxAABB.z << ")\n";
        globalMin = glm::min(globalMin, m.minAABB);
        globalMax = glm::max(globalMax, m.maxAABB);
    }
    octreeRoot = std::make_unique<OctreeNode>(globalMin, globalMax, 0, 10, 5, this);
    for (auto& m : models) octreeRoot->Insert(&m);
}

void Scene::ApplyTextureToSelected(const std::string& path) 
{
    auto selected = Application::GetInstance().menus.get()->selectedObj;
    if (selected)
    {
        Texture* tex = new Texture(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

        for (auto& model : models)
        {
            if (model.modelId == Application::GetInstance().menus.get()->selectedObj->modelId) 
                Application::GetInstance().menus.get()->selectedObj->ApplTexture(tex, path);
        }
    }
    else
    {
        Application::GetInstance().menus->LogToConsole("ERROR APPLYING TEXTURE, NO OBJECT SELECTED");
    }

}
void Scene::SelectObject(Model* obj)
{
    auto* menus = Application::GetInstance().menus.get();

    if (menus->selectedObj != obj) {
        menus->selectedObj = obj;
        selected = true;
    }

    else {
        selected = false;
        menus->selectedObj = nullptr; // mismo comportamiento que en la jerarquía
    }


}

bool Scene::RayIntersectsTriangle(const LineSegment& ray, const glm::vec3& v0,const glm::vec3& v1,const glm::vec3& v2, float& t)
{
    const float EPSILON = 1e-8f;
    glm::vec3 dir = ray.Direction();
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);

    if (fabs(a) < EPSILON) return false; // paralelo

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

    // Evita división por cero
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

    // Si todo el AABB está detrás del rayo
    if (tFar < 0.0f) return false;
    // Si no hay solapamiento
    if (tNear > tFar) return false;

    // Si el rayo empieza dentro del AABB, usamos tFar
    t = (tNear >= 0.0f) ? tNear : tFar;
    return true;
}

void Scene::Raycast(const LineSegment& ray)
{
    std::vector<Model*> hitModels;
    if (octreeRoot) {
        octreeRoot->CollectObjectsHitByRay(ray, this, hitModels);

        std::cout << "Cantidad modelos entregados por octree: "
            << hitModels.size() << std::endl;

    }
    float closestT = FLT_MAX;
    Model* selected = nullptr;

    for (auto* model : hitModels) {
        for (auto& mesh : model->meshes) {
            for (size_t i = 0; i < mesh.indices.size(); i += 3) {
                glm::vec3 v0 = mesh.positionsWorld[mesh.indices[i + 0]];
                glm::vec3 v1 = mesh.positionsWorld[mesh.indices[i + 1]];
                glm::vec3 v2 = mesh.positionsWorld[mesh.indices[i + 2]];

                float t;
                if (RayIntersectsTriangle(ray, v0, v1, v2, t)) {
                    if (t < closestT) {
                        closestT = t;
                        selected = model;
                    }
                }
            }
        }
    }

    if (selected)
    {
        SelectObject(selected); // ahora usa la misma lógica que el menú
        Application::GetInstance().menus->LogToConsole(
            Application::GetInstance().menus->selectedObj ?
            "Selected: " + Application::GetInstance().menus->selectedObj->name :
            "Deselected");
    }

}
bool Scene::PreUpdate()
{
    return true;
}

void Scene::ImGuizmo() {
    if (selected && Application::GetInstance().input->click && Application::GetInstance().menus->selectedObj !=NULL) {
        auto* menus = Application::GetInstance().menus.get();

        ImGuizmo::BeginFrame();
        ImGuizmo::SetRect(0, 0, Application::GetInstance().window->width, Application::GetInstance().window->height);

        glm::mat4 view = Application::GetInstance().camera->GetViewMatrix();
        glm::mat4 proj = Application::GetInstance().camera->GetProjectionMatrix();
        glm::mat4 model = menus->selectedObj->transformMatrix;

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
                parentMatrix = Application::GetInstance().scene->models[menus->selectedObj->ParentID].transformMatrix;
            }

            // Convertir world a local
            glm::mat4 localModel = glm::inverse(parentMatrix) * model;

            // Descomponer la local
            float translation[3], rotationDeg[3], scaleArr[3];
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localModel), translation, rotationDeg, scaleArr);

            menus->selectedObj->position = glm::vec3(translation[0], translation[1], translation[2]);
            menus->selectedObj->rotation = glm::vec3(rotationDeg[0], rotationDeg[1], rotationDeg[2]);
            menus->selectedObj->scale = glm::vec3(scaleArr[0], scaleArr[1], scaleArr[2]);

            menus->selectedObj->UpdateTransform();
            BuildOctree();
        }
    }
}

bool Scene::Update(float dt)
{
    frustum.Update(Application::GetInstance().camera.get()->GetVPMatrix(100.0f, 0.1f, 100.0f));

    Application::GetInstance().render->OrderModels();
    Application::GetInstance().render->FrustumModels();

    //GLuint shaderProgram = Application::GetInstance().render->shaderProgram;
    // 
    ////Image 2D
    //for (int i = 0; i < images.size(); i++)
    //{
    //    images[i].texUnit(shaderProgram, "tex0", 0);
    //    images[i].Bind();

    //    GLfloat vertices2[] =
    //    {
    //        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // inferior izquierda
    //         0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // inferior derecha
    //         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // superior derecha
    //        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f  // superior izquierda
    //    };

    //    //// Indices para formar dos tri�ngulos
    //    GLuint indices2[] =
    //    {
    //        0, 1, 2,  // primer tri�ngulo
    //        2, 3, 0   // segundo tri�ngulo
    //    };

    //    int vertexCount = sizeof(vertices2) / sizeof(float);
    //    int indexCount = sizeof(indices2) / sizeof(unsigned int);
    //    Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 0.0f, &images[i]);

    //    images[i].Unbind();
    //}
    ImGuizmo();

	return true;
}
void Scene::SaveScene(std::string filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open()) return;

    file << "GameObjects:\n";
    for (const auto& model : models) {
        if (model.name != "Grid")
        {
            file << "{" << "\n";

            file << "UID: " << model.modelId << "\n";
            file << "Name: " << model.name << "\n";
            file << "ParentUID: " << model.ParentID << "\n";
            file << "Translation: " << model.position.x << ", " << model.position.y << ", " << model.position.z << "\n";
            file << "Scale: " << model.scale.x << ", " << model.scale.y << ", " << model.scale.z << "\n";
            file << "Rotation: " << model.rotation.x << ", " << model.rotation.y << ", " << model.rotation.z << "\n";
            file << "Texture: " << model.texturePath << "\n";

            file << "}" << "\n";
        }
    }
    file.close();
}
void Scene::LoadScene(std::string filePath)
{
    //"../Library/FBX/Scene.txt"
    std::ifstream file(filePath);
    if (!file.is_open()) return;
    std::string ModelName;
    std::string line;
    int UID = -1;

    bool insideObject = false;

    if (models.size() > 1) {
        deleteScene = true;
        for (int i = models.size() - 1; i >= 0; --i) {
            auto& model = models[i];

            if(model.name != "Grid")
            { 
                Application::GetInstance().scene.get()->octreeRoot.get()->Clear();
                if (model.isChild)Application::GetInstance().scene.get()->models[model.ParentID].eraseChild(model.modelId);
                models[model.modelId].CleanUpChilds();
                auto& sceneModels = Application::GetInstance().scene->models;
                sceneModels.erase(std::remove_if(sceneModels.begin(), sceneModels.end(),
                    [&](const Model& m) { return &m == &models[model.modelId]; }), sceneModels.end());

                Application::GetInstance().menus.get()->selectedObj = nullptr;
                if (models.size() <= 1) {
                    deleteScene = false;
                    break;
                }
            }
        }
    }

    if (!deleteScene)
    { 
        while (std::getline(file, line)) {
            if (line == "{") {
                insideObject = true;
            }
            else if (line == "}") {
                insideObject = false;
            }
            else if (insideObject) {
                // parsear clave: valor
                std::istringstream iss(line);
                std::string key;
                if (std::getline(iss, key, ':')) {
                    std::string value;
                    std::getline(iss, value);
                    // limpiar espacios
                    if (!value.empty() && value[0] == ' ') value.erase(0, 1);
                
                    if (key == "Name") {
                        if (value.size() >= 4 && value.substr(0, 4) == "Cube") {
                            Application::GetInstance().render.get()->CreateCube();
                        }
                        else if (value.size() >= 7 && value.substr(0, 7) == "Pyramid") {
                            Application::GetInstance().render.get()->CreatePyramid();
                        }
                        else if (value.size() >= 6 && value.substr(0, 6) == "Sphere") {
                            Application::GetInstance().render.get()->CreateSphere();
                        }
                        else if (value.size() >= 7 && value.substr(0, 7) == "Diamond") {
                            Application::GetInstance().render.get()->CreateDiamond();
                        }
                        else
                        {
                            ModelName = "../Library/FBX/" + value + ".fbx";
                            Application::GetInstance().scene->LoadFBX(ModelName);
                        }
                        UID = models.size() - 1;
                    }
                    if (key == "UID");
                    else if (key == "ParentUID")
                    {
                        models[UID].ParentID = std::stoi(value);
                    }
                    else if (key == "Translation") {
                        std::stringstream ss(value);
                        ss >> models[UID].position.x;
                        ss.ignore(1);
                        ss >> models[UID].position.y;
                        ss.ignore(1);
                        ss >> models[UID].position.z;
                        models[UID].UpdateTransform();

                    }
                    else if (key == "Scale") {
                        std::stringstream ss(value);
                        ss >> models[UID].scale.x;
                        ss.ignore(1);
                        ss >> models[UID].scale.y;
                        ss.ignore(1);
                        ss >> models[UID].scale.z;
                        models[UID].UpdateTransform();

                    }
                    else if (key == "Rotation") {
                        std::stringstream ss(value);
                        ss >> models[UID].rotation.x;
                        ss.ignore(1);
                        ss >> models[UID].rotation.y;
                        ss.ignore(1);
                        ss >> models[UID].rotation.z;
                        models[UID].UpdateTransform();

                    }
                    else if (key == "Texture") {

                        Texture* tex = new Texture(value.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
                        models[UID].ApplTexture(tex, value);
                        models[UID].actualTexture = tex;
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
    }
}
bool Scene::PostUpdate()
{

 	return true;
}

bool Scene::CleanUp() 
{
    Application::GetInstance().menus->LogToConsole("Scene::CleanUp started");

    models.clear();
    imagesFiles.clear();
    for (auto& tex : images) 
        tex.Delete();
    images.clear();

    Application::GetInstance().menus->LogToConsole("Scene::CleanUp completed");
	return true;
}