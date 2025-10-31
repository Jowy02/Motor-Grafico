#include "Input.h"
#include "Camera.h"
#include "Scene.h"
#include "Menus.h"

Camera::Camera() : Module(), width(0), height(0), Position(0.0f, 0.0f, 0.0f)
{
    name = "Camera";
}

Camera::~Camera() {}

bool Camera::Awake()
{
    width = Application::GetInstance().window.get()->width;
    height = Application::GetInstance().window.get()->height;
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

void Camera::Inputs(SDL_Window* window)
{
    ImGuiIO& io = ImGui::GetIO();
    auto& app = Application::GetInstance();
    auto* input = app.input.get();
    auto* menus = app.menus.get();
    auto* selectedObj = menus->selectedObj;

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

    bool leftMouse = input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT;
    bool rightMouse = input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT;

    if (!io.WantCaptureMouse && leftMouse)
    {
        SDL_HideCursor();

        if (firstClick)
        {
            SDL_WarpMouseInWindow(window, width / 2, height / 2);
            firstClick = false;
        }

        float rotX = sensitivity * (float)(input->GetMousePosition().y - height / 2) / height;
        float rotY = sensitivity * (float)(input->GetMousePosition().x - width / 2) / width;

        if (input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
        {
            glm::vec3 target = selectedObj ? selectedObj->center : glm::vec3{ 0.0f };
            glm::vec3 size = selectedObj ? selectedObj->size : glm::vec3{ 0.0f };
            if (selectedObj) target.y -= size.y * 0.25f;

            glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
            glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(-rotY * 180.0f), Up);
            glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX * 180.0f), right);

            glm::vec3 offset = Position - target;
            offset = glm::vec3(yaw * pitch * glm::vec4(offset, 1.0f));

            Position = target + offset;
            Orientation = glm::normalize(target - Position);
        }
        else
        {
            glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
            glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX * 180.0f), right);
            glm::vec3 newOrientation = glm::vec3(pitch * glm::vec4(Orientation, 0.0f));

            float angleBetween = glm::degrees(acos(glm::dot(newOrientation, Up) /
                (glm::length(newOrientation) * glm::length(Up))));
            if (fabs(angleBetween - 90.0f) <= 85.0f) Orientation = newOrientation;

            glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(-rotY * 180.0f), Up);
            Orientation = glm::vec3(yaw * glm::vec4(Orientation, 0.0f));
        }
    }
    else if (rightMouse)
    {
        if (input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) Position += speed * Orientation;
        if (input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) Position += speed * -Orientation;
        if (input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) Position += speed * -glm::normalize(glm::cross(Orientation, Up));
        if (input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) Position += speed * glm::normalize(glm::cross(Orientation, Up));
        if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) Position += speed * Up;
        if (input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) Position += speed * -Up;
    }
    else
    {
        SDL_ShowCursor();
        firstClick = true;
    }
}

bool Camera::CleanUp()
{
    return true;
}