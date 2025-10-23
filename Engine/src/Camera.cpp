#include"Camera.h"

Camera::Camera() : Module()
{
    name = "camera";
}

Camera::~Camera()
{
}
bool Camera::Awake()
{
    Camera::width = Application::GetInstance().window.get()->width;
    Camera::height = Application::GetInstance().window.get()->height;
    Position = {0,0,3};
    return true;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, GLuint shaderID)
{
    // Inicializa matrices
    glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    // Envía cada matriz al shader
    GLint viewLoc = glGetUniformLocation(shaderID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    GLint projLoc = glGetUniformLocation(shaderID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::Inputs(SDL_Window* window)
{
    const Uint8* state = SDL_GetKeyboardState(NULL);
    bool shiftHeld = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];

    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEWHEEL) {
            if (event.wheel.y > 0) {
                Position += 1.0f * Orientation;
            }
            else if (event.wheel.y < 0) {
                Position += 1.0f * -Orientation;
            }
        }
    }
 


    // --- Manejo del ratón ---
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState && SDL_BUTTON(SDL_BUTTON_LEFT)) {
        SDL_ShowCursor(SDL_DISABLE);

        if (firstClick) {
            SDL_WarpMouseInWindow(window, width / 2, height / 2);
            firstClick = false;
        }

        // Obtener posición del ratón y calcular rotaciones
        SDL_GetMouseState(&mouseX, &mouseY);
        float rotX = sensitivity * (float)(mouseY - height / 2) / height;
        float rotY = sensitivity * (float)(mouseX - width / 2) / width;

        if (state[SDL_SCANCODE_LALT]) {

            //glm::vec3 target = Position + Orientation * distanceToTarget;
            glm::vec3 target = glm::vec3(0.0f);// cambiar por la posicion (0,0,0) del objeto que queremos centrar

            glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

            glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(-rotY * 180.0f), Up);
            glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX * 180.0f), right);

            glm::vec3 offset = Position - target;
            offset = glm::vec3(yaw * pitch * glm::vec4(offset, 1.0f));

            Position = target + offset;
            Orientation = glm::normalize(target - Position);
        }
        else{
            // Rotación vertical
            glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

            // Rota verticalmente (pitch)
            glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX), right);
            glm::vec3 newOrientation = glm::vec3(pitch * glm::vec4(Orientation, 0.0f));

            // Limitar la inclinación vertical
            float angleBetween = glm::degrees(acos(glm::dot(newOrientation, Up) /
                (glm::length(newOrientation) * glm::length(Up))));

            if (fabs(angleBetween - 90.0f) <= 85.0f) {
                Orientation = newOrientation;
            }

            // Rota horizontalmente (yaw)
            glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(-rotY), Up);
            Orientation = glm::vec3(yaw * glm::vec4(Orientation, 0.0f));
        }
    }
    else if(mouseState && SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        // Movimiento del teclado
        if (state[SDL_SCANCODE_W]) {
            Position += speed * Orientation;
        }
        if (state[SDL_SCANCODE_A]) {
            Position += speed * -glm::normalize(glm::cross(Orientation, Up));
        }
        if (state[SDL_SCANCODE_S]) {
            Position += speed * -Orientation;
        }
        if (state[SDL_SCANCODE_D]) {
            Position += speed * glm::normalize(glm::cross(Orientation, Up));
        }
        if (state[SDL_SCANCODE_SPACE]) {
            Position += speed * Up;
        }
        if (state[SDL_SCANCODE_LCTRL]) {
            Position += speed * -Up;
        }
    }
    else {
        SDL_ShowCursor(SDL_ENABLE);
        firstClick = true;
    }
}

bool Camera::CleanUp()
{
    return true;
}
