#pragma once

#include "Module.h"
#include "Window.h"
#include "Render.h"
#include "Application.h"
#include "LineSegment.h"

#include <glad/glad.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera : public Module
{
public:
    // Constructors
    Camera(); // Default
    Camera(int width, int height, glm::vec3 position); // Initialize with window size and position

    // Destructor
    virtual ~Camera();

    // Lifecycle methods
    bool Awake() override;
    bool CleanUp() override;

    std::string CameraName;

    // Camera vectors
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f); // Default looking forward
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);           // Default up direction

    // First click flag to prevent camera jumping
    bool firstClick = true;

    // Window dimensions
    int width = 800;
    int height = 600;

    // Movement and sensitivity
    float MOVESPEED = 0.1f;   // Base speed
    float speed = MOVESPEED;  // Current speed (modifiable)
    float sensitivity = 0.1f; // Mouse sensitivity

    // Updates and exports the camera matrix to a shader
    void Matrix(float FOVdeg, float nearPlane, float farPlane, GLuint shaderID);

    // Handles input (keyboard and mouse)
    void Inputs(SDL_Window* window);

    LineSegment CreatePickingRay(int mouseX, int mouseY);
    LineSegment GenerateRayFromMouse(int mouseX, int mouseY, int screenWidth, int screenHeight, const glm::mat4& view, const glm::mat4& projection);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float FOVdeg = 45.0f, float nearPlane = 0.1f, float farPlane = 1000.0f) const;
    glm::mat4 GetVPMatrix(float FOVdeg, float nearPlane, float farPlane) const;

    void ChangeCamera(Camera*cam);

    // Frustum
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float FOV = 45.0f;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    void UpdateViewMatrix();

    void UpdateProjectionMatrix();

    void UpdateOrbit();
    glm::vec3 target = glm::vec3(0.0f);
    float yaw = -90.0f;  
    float pitch = 0.0f;   
    float distance = 10.0f;

};