#include "Application.h"
#include "Input.h"
#include "Window.h"
#include "Scene.h"
#include "Camera.h"
#include "Menus.h"

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

	static SDL_Event event;
	float speed = Application::GetInstance().camera.get()->speed;
	glm::vec3 Orientation = Application::GetInstance().camera.get()->Orientation;

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

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (btn >= 1 && btn <= NUM_MOUSE_BUTTONS)
					mouseButtons[btn - 1] = KEY_DOWN;
			break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
			
				if (btn >= 1 && btn <= NUM_MOUSE_BUTTONS)
					mouseButtons[btn - 1] = KEY_UP;
			break;

			case SDL_EVENT_MOUSE_MOTION:
				
				SDL_GetMouseState(&mouseX, &mouseY);
				break;

            case SDL_EVENT_MOUSE_WHEEL:

                if (event.wheel.y > 0.0f) {
					Application::GetInstance().camera.get()->Position += speed * Orientation;
                    
                }
                else if (event.wheel.y < 0.0f) {
					Application::GetInstance().camera.get()->Position += speed * -Orientation;
                }
            break;

            case SDL_EVENT_DROP_FILE:
               
                std::string path = event.drop.data;
                std::string extension = path.substr(path.find_last_of('.') + 1);
                if (extension == "fbx" || extension == "FBX") {
					Application::GetInstance().menus.get()->selectedObj = NULL;
                    Application::GetInstance().scene->LoadFBX(path);
                }
                else if (extension == "png" || extension == "dds") {
                    Application::GetInstance().scene->ApplyTextureToSelected(path);
                }
                //TODO: do we need to free the drag&drop event??
                //SDL_free(&event);
                
            break;
		}
	}


	return true;
}

// Called before quitting
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