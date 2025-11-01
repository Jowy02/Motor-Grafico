#pragma once

#include "Module.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_rect.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#define NUM_MOUSE_BUTTONS 5

// --- Window Events ---
enum EventWindow
{
    WE_QUIT = 0,
    WE_HIDE,
    WE_SHOW,
    WE_RESIZED,
    WE_COUNT
};

// --- Key States ---
enum KeyState
{
    KEY_IDLE = 0,
    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP
};

class Input : public Module
{
public:
    // --- Constructors / Destructors ---
    Input();
    virtual ~Input();

    // --- Main lifecycle ---
    bool Awake();      // Called before render is available
    bool Start();      // Called before the first frame
    bool PreUpdate();  // Called each loop iteration
    bool CleanUp();    // Called before quitting

    // --- Input state queries ---
    KeyState GetKey(int id) const { return keyboard[id]; }
    KeyState GetMouseButtonDown(int id) const { return mouseButtons[id - 1]; }
    bool GetWindowEvent(EventWindow ev);
    glm::vec2 GetMousePosition();
    glm::vec2 GetMouseMotion();

    bool windowEvents[WE_COUNT];
private:
    // --- Window and input state ---
    KeyState* keyboard;
    KeyState mouseButtons[NUM_MOUSE_BUTTONS];

    // --- Mouse data ---
    int mouseMotionX;
    int mouseMotionY;
    float mouseX;
    float mouseY;
};