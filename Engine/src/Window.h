#pragma once

#define SDL_MAIN_HANDLED   // evita conflicto con SDL_main

#include "Module.h"
#include "SDL3/SDL_Video.h"
#include "SDL3/SDL.h"
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
//#include "imgui_impl_sdlrenderer3.h"

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
	void GetWindowSize(int& width, int& height) const;

	// Retrieve window scale
	int GetScale() const;

	// The window we'll be rendering to
	SDL_Window* window;
	void SetScreen(bool fullscreen);

	int width = 800;
	int height = 600;
private:
	SDL_GLContext context;

};
