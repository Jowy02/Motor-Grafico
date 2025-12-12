#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "Frustum.h"
#include "Render.h"
#include "LineSegment.h"

class Scene;

struct OctreeNode {
    glm::vec3 min;
    glm::vec3 max;
    std::vector<int> objects;
    std::unique_ptr<OctreeNode> children[8];
    Scene* scene;

    int maxObjects;
    int maxDepth;
    int depth;

    OctreeNode(const glm::vec3& min, const glm::vec3& max,
        int depth = 0, int maxObjects = 8, int maxDepth = 6, Scene* scene = nullptr);

    void Clear();
    bool FitsInNode(GameObject* m) const;
    void Insert(GameObject* m);
    void Subdivide();

    void CollectObjectsInFrustum(const Frustum& frustum, std::vector<GameObject*>& result) const;
    void CollectObjectsHitByRay(const LineSegment& ray, Scene* scene, std::vector<GameObject*>& result) const;
    void DebugDraw(Render* render) const;

};
