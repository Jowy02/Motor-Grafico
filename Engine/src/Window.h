#pragma once

#define SDL_MAIN_HANDLED   // Prevents conflict with SDL_main

#include "Module.h"
#include "SDL3/SDL_Video.h"
#include "SDL3/SDL.h"
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"

class Window : public Module
{
public:
    // --- Constructors / Destructors ---
    Window();
    virtual ~Window();

    // --- Main lifecycle ---
    bool Awake();     // Called before render is available
    bool CleanUp();   // Called before quitting

    // --- Window control ---
    void SetTitle(const char* title);
    void SetScreen(bool fullscreen);
    void GetWindowSize(int width, int height);
    void SetWindowSize(int width, int height);
    int GetScale() const;

    // --- Accessors ---
    SDL_Window* GetSDLWindow() const { return window; }
    SDL_GLContext GetGLContext() const { return context; }

    // --- Window data ---
    SDL_Window* window = nullptr;
    int width = 1600;
    int height = 900;

private:
    // --- OpenGL context ---
    SDL_GLContext context;
};