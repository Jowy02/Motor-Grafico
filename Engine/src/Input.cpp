#include "Application.h"
#include "Input.h"
#include "Window.h"
#include "Scene.h"
#include "Camera.h"



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


// Called before render is available
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

// Called before the first frame
bool Input::Start()
{
	//SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	static SDL_Event event;
	float speed = Application::GetInstance().camera.get()->speed;
	glm::vec3 Orientation = Application::GetInstance().camera.get()->Orientation;

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_EVENT_QUIT:
				windowEvents[WE_QUIT] = true;
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