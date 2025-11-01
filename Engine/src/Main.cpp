#define SDL_MAIN_HANDLED   // Prevents conflict with SDL_main
#include "Application.h"
#include <iostream>

int main(int argc, char* argv[]) 
{
	Application::EngineState state = Application::EngineState::CREATE;
	int result = EXIT_FAILURE;

	while (state != Application::EngineState::EXIT)
	{
		switch (state)
		{
			// Allocate the engine --------------------------------------------
		case Application::EngineState::CREATE:
			state = Application::EngineState::AWAKE;
			break;

			// Awake all modules ----------------------------------------------
		case Application::EngineState::AWAKE:
			if (Application::GetInstance().Awake() == true)
			{
				state = Application::EngineState::START;
			}
			else
			{
				state = Application::EngineState::FAIL;
			}
			break;

			// Call all modules before first frame  ---------------------------
		case Application::EngineState::START:
			if (Application::GetInstance().Start() == true)
			{
				state = Application::EngineState::LOOP;
			}
			else
			{
				state = Application::EngineState::FAIL;
			}
			break;

			// Loop all modules until we are asked to leave -------------------
		case Application::EngineState::LOOP:
			if (Application::GetInstance().Update() == false)
			{
				state = Application::EngineState::CLEAN;
			}
			break;

			// Cleanup allocated memory ---------------------------------------
		case Application::EngineState::CLEAN:
			if (Application::GetInstance().CleanUp() == true)
			{
				result = EXIT_SUCCESS;
				state = Application::EngineState::EXIT;
			}
			else
			{
				state = Application::EngineState::FAIL;
			}
			break;

			// Exit with errors and shame -------------------------------------
		case Application::EngineState::FAIL:
			result = EXIT_FAILURE;
			state = Application::EngineState::EXIT;
			break;
		}
	}
    return result;
}
