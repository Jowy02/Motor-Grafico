#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <limits>

struct LineSegment
{
    glm::vec3 a;   // origen
    glm::vec3 b;   // extremo

    LineSegment() = default;
    LineSegment(const glm::vec3& _a, const glm::vec3& _b) : a(_a), b(_b) {}

    // Dirección normalizada
    glm::vec3 Direction() const
    {
        glm::vec3 d = b - a;
        float len = glm::length(d);
        if (len == 0.0f)
            return glm::vec3(0.0f); // evita NaN si a == b
        return d / len;
    }

    // Longitud del segmento
    float Length() const
    {
        return glm::length(b - a);
    }

    // Punto medio
    glm::vec3 MidPoint() const
    {
        return (a + b) * 0.5f;
    }

    // Punto interpolado en el segmento (t entre 0 y 1)
    glm::vec3 PointAt(float t) const
    {
        return a + t * (b - a);
    }

    // Constructor alternativo: origen + dirección + longitud
    LineSegment(const glm::vec3& origin, const glm::vec3& dir, float length)
    {
        a = origin;
        b = origin + glm::normalize(dir) * length;
    }
};
