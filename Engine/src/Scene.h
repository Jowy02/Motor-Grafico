#pragma once

#include "Module.h"
#include "Model.h"
#include "Texture.h"
#include "LineSegment.h"
#include "ImGuizmo.h"

#include <vector>
#include <string>

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

    void SelectObject(Model* obj);

    // --- Scene content ---
    std::vector<Model> models;
    GizmoOperation currentGizmo = GizmoOperation::TRANSLATE;

private:
    // --- Internal data ---
    SDL_Texture* mouseTileTex = nullptr;
    std::string tilePosDebug = "[0,0]";

    // --- Resources ---
    std::vector<std::string> imagesFiles;
    std::vector<Texture> images;


    bool selected = false;
};