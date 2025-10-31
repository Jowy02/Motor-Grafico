#pragma once
#include <memory>
#include <list>
#include "Module.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

// Modules
class Window;
class Input;
class Camera;
class Render;
class Scene;
class Menus;

class Application
{
public:

	// Public method to get the instance of the Singleton
	static Application& GetInstance();

	void AddModule(std::shared_ptr<Module> module);

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();
	
private:

	// Private constructor to prevent instantiation
	// Constructor
	Application();

	// Delete copy constructor and assignment operator to prevent copying
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	std::list<std::shared_ptr<Module>> moduleList;

public:

	enum EngineState
	{
		CREATE = 1,
		AWAKE,
		START,
		LOOP,
		CLEAN,
		FAIL,
		EXIT
	};

	// --- MODULES ---
	std::shared_ptr<Window> window;
	std::shared_ptr<Input> input;
	std::shared_ptr<Render> render;
	std::shared_ptr<Scene> scene;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Menus> menus;

	bool requestExit = false;

private:

	
	float dt;	// Delta time
	int frames;	// Frames since startup

	// Calculate timing measures

	int frameCount = 0;
	int framesPerSecond = 0;
	int lastSecFrameCount = 0;

	bool showControls = false;

	float averageFps = 0.0f;
	int secondsSinceStartup = 0;

	//Maximun frame duration in miliseconds.
	int maxFrameDuration = 16;

	Uint32 frameStart = 0;
	Uint32 frameTime = 0;

	//Limitar fps
	const int targetFPS = 60;
	const int frameDelay = 1000 / targetFPS;
	
	uint64_t perfLastTime = 0;
};