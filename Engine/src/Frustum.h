#include "SDL3/SDL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

struct Frustum {
public:
    struct Plane {
        glm::vec3 normal;
        float distance;
    };

    Plane planes[6]; // left, right, top, bottom, near, far

    void Update(const glm::mat4& VP) {
        glm::mat4 m = VP;

        planes[0] = { glm::vec3(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0]), m[3][3] + m[3][0] }; // Left
        planes[1] = { glm::vec3(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0]), m[3][3] - m[3][0] }; // Right
        planes[2] = { glm::vec3(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1]), m[3][3] - m[3][1] }; // Top
        planes[3] = { glm::vec3(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1]), m[3][3] + m[3][1] }; // Bottom
        planes[4] = { glm::vec3(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2]), m[3][3] + m[3][2] }; // Near
        planes[5] = { glm::vec3(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2]), m[3][3] - m[3][2] }; // Far

        for (int i = 0; i < 6; i++) {
            float len = glm::length(planes[i].normal);
            planes[i].normal /= len;
            planes[i].distance /= len;
        }
    }

    bool IsBoxVisible(const glm::vec3& min, const glm::vec3& max) const {
        for (int i = 0; i < 6; i++) {
            glm::vec3 positiveVertex = min;
            if (planes[i].normal.x >= 0) positiveVertex.x = max.x;
            if (planes[i].normal.y >= 0) positiveVertex.y = max.y;
            if (planes[i].normal.z >= 0) positiveVertex.z = max.z;

            if (glm::dot(planes[i].normal, positiveVertex) + planes[i].distance < 0)
                return false;
        }
        return true;
    }
};
