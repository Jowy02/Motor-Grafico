#include "Application.h"
#include "Input.h"
#include "Window.h"
#include "Scene.h"
#include "Camera.h"
#include "Menus.h"
#include "ImGuizmo.h"

#define MAX_KEYS 300

Input::Input() : Module()
{
	name = "input";

	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

bool Input::Awake()
{
	//LOG("Init SDL input event system");
	Application::GetInstance().menus->LogToConsole(std::string("ERROR: Assimp load failed: "));

	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		//LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Input::Start()
{
	//SDL_StopTextInput();
	return true;
}

bool Input::PreUpdate()
{
	auto camera = Application::GetInstance().camera.get();
	auto selectedObj = Application::GetInstance().menus->selectedObj;  
	glm::vec3 target = selectedObj ? selectedObj->center : glm::vec3(0.0f);

	static SDL_Event event;
	

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouseButtons[i] == KEY_DOWN)
			mouseButtons[i] = KEY_REPEAT;

		if (mouseButtons[i] == KEY_UP)
			mouseButtons[i] = KEY_IDLE;
	}
	
	const bool* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	while(SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSDL3_ProcessEvent(&event);

		int btn = event.button.button; 
		switch(event.type)
		{
			case SDL_EVENT_QUIT:
				windowEvents[WE_QUIT] = true;
			break;
			case SDL_EVENT_WINDOW_RESIZED:
				windowEvents[WE_RESIZED] = true;
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (btn >= 1 && btn <= NUM_MOUSE_BUTTONS) {
					mouseButtons[btn - 1] = KEY_DOWN;
					SDL_GetMouseState(&mouseX, &mouseY); // guarda posición exacta del clic
				}

			break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
			
				if (btn >= 1 && btn <= NUM_MOUSE_BUTTONS)
					mouseButtons[btn - 1] = KEY_UP;
			break;

			case SDL_EVENT_MOUSE_MOTION:
				
				SDL_GetMouseState(&mouseX, &mouseY);
				break;

            case SDL_EVENT_MOUSE_WHEEL:
				{
				glm::vec3 offset = camera->Position - target;
				float radius = glm::length(offset);

				float zoomAmount = event.wheel.y * camera->speed;
				float newRadius = glm::clamp(radius - zoomAmount, 2.0f, 50.0f);

				glm::vec3 direction = glm::normalize(offset);
				camera->Position = target + direction * newRadius;
				camera->Orientation = glm::normalize(target - camera->Position);
		}
            break;

            case SDL_EVENT_DROP_FILE:
               
                std::string path = event.drop.data;
                std::string extension = path.substr(path.find_last_of('.') + 1);
                if (extension == "fbx" || extension == "FBX") 
				{
					Application::GetInstance().menus.get()->selectedObj = NULL;
                    Application::GetInstance().scene->LoadFBX(path);
                }
                else if (extension == "png" || extension == "dds")
                    Application::GetInstance().scene->ApplyTextureToSelected(path);
            break;
		}
	}


	if (keys[SDL_SCANCODE_W] == KEY_DOWN) {
		click = true;
		Application::GetInstance().scene->currentGizmo = GizmoOperation::TRANSLATE;
	}
	if (keys[SDL_SCANCODE_E] == KEY_DOWN) {
		click = true;

		Application::GetInstance().scene->currentGizmo = GizmoOperation::ROTATE;
	}
	if (keys[SDL_SCANCODE_R] == KEY_DOWN) {
		click = true;

		Application::GetInstance().scene->currentGizmo = GizmoOperation::SCALE;
	}
	// CTRL + S
	if (keyboard[SDL_SCANCODE_S] == KEY_DOWN && (SDL_GetModState() & SDL_KMOD_CTRL))
	{
		Application::GetInstance().scene->SaveSceneToLibrary("Library");
	}
	if (keyboard[SDL_SCANCODE_Z] == KEY_DOWN && (SDL_GetModState() & SDL_KMOD_CTRL))
	{
		Application::GetInstance().scene->LoadSceneFromLibrary("Library");
	}


	return true;
}

bool Input::CleanUp()
{
	//LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}

glm::vec2 Input::GetMousePosition()
{
	return glm::vec2(mouseX, mouseY);
}

glm::vec2  Input::GetMouseMotion()
{
	return glm::vec2(mouseMotionX, mouseMotionY);
}