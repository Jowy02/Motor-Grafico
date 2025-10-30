#include "Window.h"
#include "Application.h"
#include "Menus.h"

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
    Application::GetInstance().menus->LogToConsole("Initializing SDL...");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Application::GetInstance().menus->LogToConsole("ERROR: SDL initialization failed: " + std::string(SDL_GetError()));
        //std::cerr << "Error inicializando SDL: " << SDL_GetError() << "\n";
        return -1;
    }
    Application::GetInstance().menus->LogToConsole("SDL initialized successfully");

    // Configuración del contexto OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    Application::GetInstance().menus->LogToConsole("Configured OpenGL context attributes");

    // Crear ventana
    Application::GetInstance().menus->LogToConsole("Creating SDL window...");
    window = SDL_CreateWindow("MyEngine",
        width, height,
        SDL_WINDOW_OPENGL);

    SDL_ShowWindow(window);
    if (!window) {
        Application::GetInstance().menus->LogToConsole("ERROR: Failed to create SDL window: " + std::string(SDL_GetError()));
        //std::cerr << "Error creando ventana: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }
    Application::GetInstance().menus->LogToConsole("SDL window created successfully");

    // Crear contexto OpenGL
    Application::GetInstance().menus->LogToConsole("Creating OpenGL context...");
     context = SDL_GL_CreateContext(window);
    if (!context) {
        Application::GetInstance().menus->LogToConsole("ERROR: Failed to create OpenGL context: " + std::string(SDL_GetError()));
        //std::cerr << "Error creando contexto OpenGL: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    Application::GetInstance().menus->LogToConsole("OpenGL context created successfully");

    SDL_GL_SetSwapInterval(1);  // Enable V-Sync
     
    // Inicializar GLAD
    Application::GetInstance().menus->LogToConsole("Initializing GLAD...");
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        Application::GetInstance().menus->LogToConsole("ERROR: GLAD initialization failed");
        //std::cerr << "Error inicializando GLAD\n";
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    Application::GetInstance().menus->LogToConsole("GLAD initialized successfully");
}

// Called before quitting
bool Window::CleanUp()
{
    Application::GetInstance().menus->LogToConsole("Cleaning up SDL and OpenGL resources...");
	// Destroy window
	if (window != NULL)
	{
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
	}

	// Quit SDL subsystems
	SDL_Quit();
    Application::GetInstance().menus->LogToConsole("Window cleanup completed");
	return true;
}

void Window::GetWindowSize(int with, int height)
{
    SDL_SetWindowSize(window, with, height);

    this->width = with;
    this->height = height;
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