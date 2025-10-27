#include"Input.h"
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

    if (Application::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
        SDL_HideCursor();

        if (firstClick) {
            SDL_WarpMouseInWindow(window, width / 2, height / 2);
            firstClick = false;
        }

        //calcular rotaciones
        float rotX = sensitivity * (float)(Application::GetInstance().input.get()->GetMousePosition().y - height / 2) / height;
        float rotY = sensitivity * (float)(Application::GetInstance().input.get()->GetMousePosition().x - width / 2) / width;

        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {

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
    else if(Application::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
        // Movimiento del teclado
        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
            Position += speed * Orientation;
        }
        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
            Position += speed * -glm::normalize(glm::cross(Orientation, Up));
        }
        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
            Position += speed * -Orientation;
        }
        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
            Position += speed * glm::normalize(glm::cross(Orientation, Up));
        }
        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
            Position += speed * Up;
        }
        if (Application::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) {
            Position += speed * -Up;
        }
    }
    else {
        SDL_ShowCursor();
        firstClick = true;
    }
}

bool Camera::CleanUp()
{
    return true;
}
