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

    if (Application::GetInstance().input.get()->GetWindowEvent(WE_RESIZED)) {
        SDL_GetWindowSize(window,&width,&height);
        Application::GetInstance().input.get()->windowEvents[WE_RESIZED] = false;;
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

        if (firstClick)
        {
            SDL_WarpMouseInWindow(window, width / 2, height / 2);
            firstClick = false;
        }

        glm::vec3 target = selectedObj ? selectedObj->center : glm::vec3{ 0.0f };
        glm::vec3 size = selectedObj ? selectedObj->size : glm::vec3{ 0.0f };
        if (selectedObj) target.y -= size.y * 0.25f;

        glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
        glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(-rotY ), Up);
        glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX ), right);

        glm::vec3 offset = Position - target;
        offset = glm::vec3(yaw * pitch * glm::vec4(offset, 1.0f));

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
}

bool Camera::CleanUp()
{
    return true;
}