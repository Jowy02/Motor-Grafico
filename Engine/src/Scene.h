#pragma once

#include "Module.h"
#include "GameObject.h"
#include "Texture.h"
#include "LineSegment.h"
#include "ImGuizmo.h"
#include "Frustum.h"
#include "OctreeNode.h"
#include "SimulationController.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct SDL_Texture;

enum class GizmoOperation {
    TRANSLATE,
    ROTATE,
    SCALE
};

class Scene : public Module
{
public:
    // --- Constructors / Destructors ---
    Scene();
    virtual ~Scene();

    // --- Main lifecycle ---
    bool Awake();              // Called before render is available
    bool Start();              // Called before the first frame
    bool PreUpdate();          // Called before all updates
    bool Update(float dt);     // Called each loop iteration
    bool PostUpdate();         // Called after all updates
    bool CleanUp();            // Called before quitting

    // --- Scene management ---
    void LoadFBX(const std::string& path);
    void ApplyTextureToSelected(const std::string& path);
   
    //Raycast
    bool RayIntersectsTriangle(const LineSegment& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    bool RayIntersectsAABB(const LineSegment& ray, const glm::vec3& min, const glm::vec3& max, float& t);
    void Raycast(const LineSegment& ray);
    void RaycastFromMouse(int mouseX, int mouseY);

    void SelectObject(GameObject* obj);

    void SaveMesh(std::string filePath, GameObject model);
    void SaveMeta(int meshesId, std::string filePath);

    void LoadMeta(std::string filePath);
    void LoadMesh(std::string filePath);

    void SaveScene(std::string filePath);
    void LoadScene(std::string filePath);
    void ClearScene();
    // --- Scene content ---
    std::vector<GameObject> models;
    std::vector<Camera*> cameras;

    GizmoOperation currentGizmo = GizmoOperation::TRANSLATE;

    void ImGuizmo();
    Frustum frustum;
    bool selected = false;

    void BuildOctree();
    std::unique_ptr<OctreeNode> octreeRoot;

    void RecreateGameObject(const InitialGameObjectData& blueprint);
    Camera* GetActiveCamera();

private:
    // --- Internal data ---
    SDL_Texture* mouseTileTex = nullptr;
    std::string tilePosDebug = "[0,0]";

    // --- Resources ---
    std::vector<std::string> imagesFiles;
    std::vector<Texture> images;
    bool deleteScene = false;
};