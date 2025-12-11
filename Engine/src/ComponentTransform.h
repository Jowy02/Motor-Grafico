#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameObject;

class ComponentTransform
{
public:
	ComponentTransform(const GameObject* owner);
	~ComponentTransform();
    void UpdateAABB();
    void GenerateLocalMatrix();
    void BoundingBox(float translation[3], float rotationDeg[3], float scaleArr[3]);

    // --- Transformations ---
    glm::mat4 transformMatrix;
    glm::mat4 localMatrix;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 worldPosition;
    glm::vec3 worldRotation;
    glm::vec3 worldScale;

    // --- Bounding box ---
    glm::vec3 center;
    glm::vec3 minAABB;
    glm::vec3 maxAABB;
    glm::vec3 size;

    glm::vec3 localMinAABB;
    glm::vec3 localMaxAABB;
};
