#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(const GameObject* owner){

}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::GenerateLocalMatrix()
{
    localMatrix = glm::mat4(1.0f);
    localMatrix = glm::translate(localMatrix, position);
    localMatrix = glm::rotate(localMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    localMatrix = glm::rotate(localMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    localMatrix = glm::rotate(localMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    localMatrix = glm::scale(localMatrix, scale);
}
void ComponentTransform::BoundingBox(float translation[3], float rotationDeg[3], float scaleArr[3])
{
    worldPosition = glm::vec3(translation[0], translation[1], translation[2]);

    worldRotation = glm::vec3(rotationDeg[0], rotationDeg[1], rotationDeg[2]);

    worldScale = glm::vec3(scaleArr[0], scaleArr[1], scaleArr[2]);

    // Usa los bounds LOCALES para calcular la caja en mundo
    glm::vec3 corners[8] =
    {

        {localMinAABB.x, localMinAABB.y, localMinAABB.z},
        {localMinAABB.x, localMinAABB.y, localMaxAABB.z},
        {localMinAABB.x, localMaxAABB.y, localMinAABB.z},
        {localMinAABB.x, localMaxAABB.y, localMaxAABB.z},
        {localMaxAABB.x, localMinAABB.y, localMinAABB.z},
        {localMaxAABB.x, localMinAABB.y, localMaxAABB.z},
        {localMaxAABB.x, localMaxAABB.y, localMinAABB.z},
        {localMaxAABB.x, localMaxAABB.y, localMaxAABB.z},
    };

    glm::vec3 newMin(FLT_MAX);
    glm::vec3 newMax(-FLT_MAX);

    for (int i = 0; i < 8; ++i)
    {
        glm::vec4 transformed = transformMatrix * glm::vec4(corners[i], 1.0f);
        glm::vec3 p = glm::vec3(transformed);
        newMin = glm::min(newMin, p);
        newMax = glm::max(newMax, p);
    }

    center = (newMin + newMax) * 0.5f;
    size = newMax - newMin;
}
void ComponentTransform::UpdateAABB()
{
    glm::vec3 worldMin(std::numeric_limits<float>::max());
    glm::vec3 worldMax(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; ++i)
    {
        glm::vec3 corner(
            (i & 1) ? localMaxAABB.x : localMinAABB.x,
            (i & 2) ? localMaxAABB.y : localMinAABB.y,
            (i & 4) ? localMaxAABB.z : localMinAABB.z
        );

        glm::vec4 transformed = transformMatrix * glm::vec4(corner, 1.0f);
        glm::vec3 p = glm::vec3(transformed);

        worldMin = glm::min(worldMin, p);
        worldMax = glm::max(worldMax, p);
    }

    minAABB = worldMin;
    maxAABB = worldMax;
    center = (worldMin + worldMax) * 0.5f;
    size = worldMax - worldMin;
}