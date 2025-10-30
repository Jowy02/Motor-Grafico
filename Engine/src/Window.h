#pragma once

#define SDL_MAIN_HANDLED   // evita conflicto con SDL_main

#include "Module.h"
#include "SDL3/SDL_Video.h"
#include "SDL3/SDL.h"
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* title);

	// Retrive window size
	void GetWindowSize(int width, int height);

	// Retrieve window scale
	int GetScale() const;

	SDL_Window* GetSDLWindow() const { return window; }
	SDL_GLContext GetGLContext() const { return context; }
	// The window we'll be rendering to
	SDL_Window* window;
	void SetScreen(bool fullscreen);

	int width = 1600;
	int height = 900;
private:
	SDL_GLContext context;

};
