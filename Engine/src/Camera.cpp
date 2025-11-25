#include "Input.h"
#include "Camera.h"
#include "Scene.h"
#include "Menus.h"
#include "LineSegment.h"

Camera::Camera() : Module(), width(0), height(0), Position(0.0f, 0.0f, 0.0f)
{
    name = "Camera";
}

Camera::~Camera() {}

bool Camera::Awake()
{
    Position = { 0, 2, 10 }; // Default camera position
    return true;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, GLuint shaderID)
{
    // Calculate view and projection matrices
    glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    // Send matrices to shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

LineSegment Camera::CreatePickingRay(int mouseX, int mouseY, float FOVdeg, float nearPlane, float farPlane)
{
    float x = (2.0f * mouseX) / width - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / height;

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::mat4 proj = glm::perspective(glm::radians(FOVdeg), float(width) / height, nearPlane, farPlane);
    glm::vec4 rayEye = glm::inverse(proj) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
    glm::vec4 rayWorld4 = glm::inverse(view) * rayEye;
    glm::vec3 rayWorld = glm::normalize(glm::vec3(rayWorld4));

    return LineSegment(Position, Position + rayWorld * farPlane);
}


void Camera::Inputs(SDL_Window* window)
{

    ImGuiIO& io = ImGui::GetIO();
    auto& app = Application::GetInstance();
    auto* input = app.input.get();
    auto* menus = app.menus.get();
    auto* selectedObj = menus->selectedObj;

    if (Application::GetInstance().input.get()->GetWindowEvent(WE_RESIZED)) {
        SDL_GetWindowSize(window,&width,&height);
        glViewport(0, 0, width, height);
        Application::GetInstance().input.get()->windowEvents[WE_RESIZED] = false;
    }

    // Center the camera on the selected object
    if (input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && selectedObj)
    {
        glm::vec3 target = selectedObj->center;
        glm::vec3 size = selectedObj->size;
        target.y -= size.y * 0.25f;

        float idealDistance = glm::length(size) * 1.0f;
        glm::vec3 direction = glm::normalize(Position - target);

        Position = target + direction * idealDistance;
        Orientation = glm::normalize(target - Position);
    }

    // Increase camera speed with Shift
    speed = (input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) ? MOVESPEED * 2 : MOVESPEED;

    float dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    float rotX = sensitivity * dy;
    float rotY = sensitivity * dx;

    if (!io.WantCaptureMouse && input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
    {
        SDL_SetWindowRelativeMouseMode(window, true);

        glm::vec3 target = selectedObj ? selectedObj->center : glm::vec3(0.0f);
        glm::vec3 offset = Position - target;
        float radius = glm::length(offset);

        float pitch = glm::degrees(asin(offset.y / radius));
        float yaw = glm::degrees(atan2(offset.z, offset.x));

        float sensitivity = 0.2f;
        pitch += sensitivity * dy;
        yaw += sensitivity * dx;

        // Limita pitch
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Reconstruye posición de cámara
        float pitchRad = glm::radians(pitch);
        float yawRad = glm::radians(yaw);

        offset.x = radius * cos(pitchRad) * cos(yawRad);
        offset.y = radius * sin(pitchRad);
        offset.z = radius * cos(pitchRad) * sin(yawRad);

        Position = target + offset;
        Orientation = glm::normalize(target - Position);
    }

    else if (input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        SDL_SetWindowRelativeMouseMode(window, true);

        if (input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) Position += speed * Orientation;
        if (input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) Position += speed * -Orientation;
        if (input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) Position += speed * -glm::normalize(glm::cross(Orientation, Up));
        if (input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) Position += speed * glm::normalize(glm::cross(Orientation, Up));
        if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) Position += speed * Up;
        if (input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) Position += speed * -Up;

        //Free Look
        glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
        glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX), right);
        glm::vec3 newOrientation = glm::vec3(pitch * glm::vec4(Orientation, 0.0f));

        float angleBetween = glm::degrees(acos(glm::dot(newOrientation, Up) /
            (glm::length(newOrientation) * glm::length(Up))));
        if (fabs(angleBetween - 90.0f) <= 85.0f) Orientation = newOrientation;

        glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(-rotY), Up);
        Orientation = glm::vec3(yaw * glm::vec4(Orientation, 0.0f));
    }
    else
    {
       
       if(SDL_GetWindowRelativeMouseMode(window))
           SDL_SetWindowRelativeMouseMode(window, false);

       firstClick = true;
    }

    if (!io.WantCaptureMouse && input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && Application::GetInstance().input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
    {
        float mx, my;
        SDL_GetMouseState(&mx, &my);
        LineSegment ray = CreatePickingRay(mx, my, 45.0f, 0.1f, 1000.0f);
        Application::GetInstance().scene->Raycast(ray);
    }

}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Orientation, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane) const
{
    return glm::perspective(glm::radians(FOVdeg), float(width) / height, nearPlane, farPlane);
}

bool Camera::CleanUp()
{
    return true;
}