#include "Input.h"
#include "Camera.h"
#include "Scene.h"
#include "Menus.h"
#include "LineSegment.h"

Camera::Camera() : Module(), width(800), height(600), Position(0.0f, 2.0f, 10.0f),
FOV(60.0f), nearPlane(0.1f), farPlane(100.0f), MOVESPEED(5.0f), sensitivity(0.1f)
{
    name = "Camera";
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
}

Camera::~Camera() {}

bool Camera::Awake()
{
    Position = { 0, 2, 10 }; // Default camera position
   // SDL_GetWindowSize(Application::GetInstance().window->window, &width, &height);
    CameraName = "MainCamera";
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

LineSegment Camera::CreatePickingRay(int mouseX, int mouseY)
{
    glm::mat4 view = GetViewMatrix();
    glm::mat4 proj = GetProjectionMatrix();
    return GenerateRayFromMouse(mouseX, mouseY, width, height, view, proj);
}

LineSegment Camera::GenerateRayFromMouse(int mouseX, int mouseY, int screenWidth, int screenHeight, const glm::mat4& view, const glm::mat4& projection)
{
    // Normalizar coordenadas de pantalla
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight; // invertir Y
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    // Pasar a espacio de vista
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // Pasar a espacio mundo
    glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    glm::vec3 rayOrigin = glm::vec3(glm::inverse(view)[3]); // posición de la cámara

    return LineSegment(rayOrigin, rayOrigin + rayDir * 1000.0f); // rayo largo
}


void Camera::Inputs(SDL_Window* window)
{
    ImGuiIO& io = ImGui::GetIO();
    auto& app = Application::GetInstance();
    auto* input = app.input.get();
    auto* menus = app.menus.get();
    auto* selectedObj = menus->selectedObj;

    if (input->GetWindowEvent(WE_RESIZED)) {
        SDL_GetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        input->windowEvents[WE_RESIZED] = false;
    }

    if (input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && selectedObj) {
        glm::vec3 target = selectedObj->center;
        glm::vec3 size = selectedObj->size;
        target.y -= size.y * 0.25f;

        distance = glm::length(size) * 1.0f; 
        Position = target - Orientation * distance;
    }

    speed = (input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) ? MOVESPEED * 2.0f : MOVESPEED;

    float dx, dy;
    SDL_GetRelativeMouseState(&dx, &dy);

    if (!io.WantCaptureMouse && input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT &&
        input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
    {
        SDL_SetWindowRelativeMouseMode(window, true);

        glm::vec3 target = selectedObj ? selectedObj->center : glm::vec3(0.0f);

        yaw += sensitivity * dx;
        pitch += sensitivity * dy;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        float pitchRad = glm::radians(pitch);
        float yawRad = glm::radians(yaw);

        glm::vec3 offset;
        offset.x = distance * cos(pitchRad) * cos(yawRad);
        offset.y = distance * sin(pitchRad);
        offset.z = distance * cos(pitchRad) * sin(yawRad);

        Position = target + offset;
        Orientation = glm::normalize(target - Position);
    }
    else if (input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        SDL_SetWindowRelativeMouseMode(window, true);

        glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
        if (input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) Position += speed * Orientation;
        if (input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) Position -= speed * Orientation;
        if (input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) Position -= speed * right;
        if (input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) Position += speed * right;
        if (input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) Position += speed * Up;
        if (input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) Position -= speed * Up;

        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(-sensitivity * dy), right);
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(-sensitivity * dx), Up);

        Orientation = glm::normalize(glm::vec3(rotY * rotX * glm::vec4(Orientation, 0.0f)));

        pitch = glm::degrees(asin(Orientation.y));
        yaw = glm::degrees(atan2(Orientation.z, Orientation.x));
    }
    else
    {
        if (SDL_GetWindowRelativeMouseMode(window))
            SDL_SetWindowRelativeMouseMode(window, false);
    }

    if (!io.WantCaptureMouse && input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN &&
        input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
    {
        float mx, my;
        SDL_GetMouseState(&mx, &my);
        LineSegment ray = CreatePickingRay(mx, my);
        app.scene->Raycast(ray);
    }
}


glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Orientation, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float FOVdeg, float nearPlane, float farPlane) const
{
    float aspect = (height != 0) ? float(width) / float(height) : 1.0f;

    return glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);
}

glm::mat4 Camera::GetVPMatrix(float FOVdeg, float nearPlane, float farPlane) const
{
    glm::mat4 proj = GetProjectionMatrix(FOVdeg, nearPlane, farPlane);
    glm::mat4 view = GetViewMatrix();
    return proj * view;
}

void Camera::UpdateViewMatrix()
{
    // Llama a glm::lookAt con Position y Orientation actual
    viewMatrix = glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::UpdateProjectionMatrix()
{
    projectionMatrix = glm::perspective(glm::radians(FOV), float(width) / height, nearPlane, farPlane);
}

void Camera::ChangeCamera(Camera * cam)
{
    Camera Temp; 
    
    Temp.Orientation.x = Orientation.x;
    Temp.Orientation.y = Orientation.y;
    Temp.Orientation.z = Orientation.z;
    Temp.Orientation = Orientation;
    Temp.FOV = FOV;
    Temp.nearPlane = nearPlane;
    Temp.farPlane = farPlane;
    Temp.MOVESPEED = MOVESPEED;
    Temp.sensitivity = sensitivity;
    Temp.Position = Position;

    Orientation.x = cam->Orientation.x;
    Orientation.y = cam->Orientation.y;
    Orientation.z = cam->Orientation.z;
    //Orientation =   cam->Orientation;
    UpdateViewMatrix();

    FOV = cam->FOV;
    nearPlane = cam->nearPlane;
    farPlane =  cam->farPlane;
    MOVESPEED = cam->MOVESPEED;
    sensitivity = cam->sensitivity;
    Position =  cam->Position;
    UpdateProjectionMatrix();

    cam->Orientation.x = Temp.Orientation.x;
    cam->Orientation.y = Temp.Orientation.y;
    cam->Orientation.z = Temp.Orientation.z;
    cam->Orientation = Temp.Orientation;
    cam->FOV = Temp.FOV;
    cam->nearPlane = Temp.nearPlane;
    cam->farPlane = Temp.farPlane;
    cam->MOVESPEED = Temp.MOVESPEED;
    cam->sensitivity = Temp.sensitivity;
    cam->Position = Temp.Position;
}

bool Camera::CleanUp()
{
    return true;
}