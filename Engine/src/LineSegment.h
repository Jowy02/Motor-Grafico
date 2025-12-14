#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <limits>

struct LineSegment
{
    glm::vec3 a;   // origen
    glm::vec3 b;   // extrem

    LineSegment() = default;
    LineSegment(const glm::vec3& _a, const glm::vec3& _b) : a(_a), b(_b) {}

    // Normalized address
    glm::vec3 Direction() const
    {
        glm::vec3 d = b - a;
        float len = glm::length(d);
        if (len == 0.0f)
            return glm::vec3(0.0f); // avoid NaN if a == b
        return d / len;
    }

    // Segment length
    float Length() const
    {
        return glm::length(b - a);
    }

    // Midpoint
    glm::vec3 MidPoint() const
    {
        return (a + b) * 0.5f;
    }

    // Interpolated point on the segment (t between 0 and 1)
    glm::vec3 PointAt(float t) const
    {
        return a + t * (b - a);
    }

    // Alternative constructor: origin + direction + length
    LineSegment(const glm::vec3& origin, const glm::vec3& dir, float length)
    {
        a = origin;
        b = origin + glm::normalize(dir) * length;
    }
};
