#include "Window.h"
#include "Application.h"


Window::Window() : Module()
{
	window = NULL;
	name = "window";
}

// Destructor
Window::~Window()
{
}

// Called before render is available
bool Window::Awake()
{
	bool ret = true;

    // --- Inicialización de SDL ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        //std::cerr << "Error inicializando SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    // Configuración del contexto OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Crear ventana
    window = SDL_CreateWindow("MyEngine",
        width, height,
        SDL_WINDOW_OPENGL);

    SDL_ShowWindow(window);
    if (!window) {
        //std::cerr << "Error creando ventana: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Crear contexto OpenGL
     context = SDL_GL_CreateContext(window);
    if (!context) {
        //std::cerr << "Error creando contexto OpenGL: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        //std::cerr << "Error inicializando GLAD\n";
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


}

// Called before quitting
bool Window::CleanUp()
{

	// Destroy window
	if (window != NULL)
	{
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
	}

	// Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void Window::SetTitle(const char* new_title)
{
	SDL_SetWindowTitle(window, new_title);
}

void Window::SetScreen(bool fullscreen)
{
	SDL_SetWindowFullscreen(window, fullscreen);
}