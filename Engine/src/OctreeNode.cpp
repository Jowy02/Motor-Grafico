#include "OctreeNode.h"
#include "Scene.h"
#include <iostream>
OctreeNode::OctreeNode(const glm::vec3& min, const glm::vec3& max,
    int depth, int maxObjects, int maxDepth, Scene* scene)
    : min(min), max(max), depth(depth), maxObjects(maxObjects), maxDepth(maxDepth), scene(scene) {}

void OctreeNode::Clear() {
    objects.clear();
    for (int i = 0; i < 8; i++) {
        if (children[i]) {
            children[i]->Clear();
            children[i].reset();
        }
    }
}

bool OctreeNode::FitsInNode(Model* m) const {
    return !(m->maxAABB.x < min.x || m->minAABB.x > max.x ||
        m->maxAABB.y < min.y || m->minAABB.y > max.y ||
        m->maxAABB.z < min.z || m->minAABB.z > max.z);
}


static bool IntersectsBox(const glm::vec3& nodeMin, const glm::vec3& nodeMax, const Model* m) {
    return !(m->maxAABB.x < nodeMin.x || m->minAABB.x > nodeMax.x ||
        m->maxAABB.y < nodeMin.y || m->minAABB.y > nodeMax.y ||
        m->maxAABB.z < nodeMin.z || m->minAABB.z > nodeMax.z);
}

void OctreeNode::Insert(Model* m) {
    // si el objeto no intersecta este nodo, ignorar
    if (!IntersectsBox(min, max, m)) return;

    if (depth >= maxDepth || (int)objects.size() < maxObjects) {
        objects.push_back(m->modelId); // guardamos id, no puntero
        return;
    }

    if (!children[0]) Subdivide();

    // insertar en hijos que intersecten
    for (int i = 0; i < 8; ++i) {
        children[i]->Insert(m);
    }
}



void OctreeNode::Subdivide() {
    glm::vec3 center = (min + max) * 0.5f;

        children[0] = std::make_unique<OctreeNode>(min, center, depth + 1, maxObjects, maxDepth, scene);
    children[1] = std::make_unique<OctreeNode>(glm::vec3(center.x, min.y, min.z),
        glm::vec3(max.x, center.y, center.z),
        depth + 1, maxObjects, maxDepth, scene);
    children[2] = std::make_unique<OctreeNode>(glm::vec3(min.x, center.y, min.z),
        glm::vec3(center.x, max.y, center.z),
        depth + 1, maxObjects, maxDepth, scene);
    children[3] = std::make_unique<OctreeNode>(glm::vec3(center.x, center.y, min.z),
        glm::vec3(max.x, max.y, center.z),
        depth + 1, maxObjects, maxDepth, scene);
    children[4] = std::make_unique<OctreeNode>(glm::vec3(min.x, min.y, center.z),
        glm::vec3(center.x, center.y, max.z),
        depth + 1, maxObjects, maxDepth, scene);
    children[5] = std::make_unique<OctreeNode>(glm::vec3(center.x, min.y, center.z),
        glm::vec3(max.x, center.y, max.z),
        depth + 1, maxObjects, maxDepth, scene);
    children[6] = std::make_unique<OctreeNode>(glm::vec3(min.x, center.y, center.z),
        glm::vec3(center.x, max.y, max.z),
        depth + 1, maxObjects, maxDepth, scene);
    children[7] = std::make_unique<OctreeNode>(center, max, depth + 1, maxObjects, maxDepth, scene);

    std::vector<int> keepHere;
    for (int objId : objects) {
        Model* obj = &scene->models[objId];
        bool moved = false;
        for (int i = 0; i < 8; ++i) {
            auto& c = children[i];
            if (!(obj->maxAABB.x < c->min.x || obj->minAABB.x > c->max.x ||
                obj->maxAABB.y < c->min.y || obj->minAABB.y > c->max.y ||
                obj->maxAABB.z < c->min.z || obj->minAABB.z > c->max.z))
            {
                c->Insert(obj);
                moved = true;
                break;
            }
        }
        if (!moved) keepHere.push_back(objId);
    }
    objects.swap(keepHere);

}



void OctreeNode::CollectObjectsInFrustum(const Frustum& frustum, std::vector<Model*>& result) const {
    if (!frustum.IsBoxVisible(min, max)) return;

    for (int objId : objects) {
        Model* obj = &scene->models[objId]; // necesitas pasar scene
        if (frustum.IsBoxVisible(obj->minAABB, obj->maxAABB))
            result.push_back(obj);
    }


    for (int i = 0; i < 8; i++)
        if (children[i]) children[i]->CollectObjectsInFrustum(frustum, result);
}

void OctreeNode::CollectObjectsHitByRay(const LineSegment& ray, Scene* scene, std::vector<Model*>& result) const {
    float t;
    if (!scene->RayIntersectsAABB(ray, min, max, t)) return;

    std::cout << "[Node hit] depth " << depth << " objs=" << objects.size() << "\n";

    for (int id : objects) {
        if (id < 0 || id >= (int)scene->models.size()) continue; // seguridad
        Model* obj = &scene->models[id];
        float tobj;
        if (scene->RayIntersectsAABB(ray, obj->minAABB, obj->maxAABB, tobj)) {
            std::cout << "  hit obj " << obj->name << " t=" << tobj << "\n";
            result.push_back(obj);
        }
    }

    for (int i = 0; i < 8; ++i)
        if (children[i]) children[i]->CollectObjectsHitByRay(ray, scene, result);
}


void OctreeNode::DebugDraw(Render* render) const {
    for (int objId : objects) {
        Model* obj = &scene->models[objId];
        render->DrawAABBOutline(*obj);
    }


    for (int i = 0; i < 8; i++)
        if (children[i]) children[i]->DebugDraw(render);
}
