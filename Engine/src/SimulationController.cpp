#include "SimulationController.h"
#include "Application.h" 
#include "Scene.h"       


SimulationController::SimulationController() : Module()
{
    // El estado inicial debe ser STOPPED (Modo Edición)
    currentState = GameState::STOPPED;
    name = "SimulationController"; // Opcional, si hereda de Module
}

SimulationController::~SimulationController()
{
    // No hay lógica compleja de limpieza aquí, ya que los punteros
    // son gestionados por Application o Scene.
}

// =================================================================
// 1. FUNCIONES DE CONTROL PÚBLICO
// =================================================================

void SimulationController::Play()
{
    // Solo guardar el estado si se pasa de STOPPED a RUNNING.
    if (currentState == GameState::STOPPED) {
        // Llama a la función para guardar la posición inicial de todos los modelos
        SaveInitialSceneState();
    }
    // Si ya estábamos pausados, simplemente reanudamos.
    currentState = GameState::RUNNING;
    // Logica para ocultar o mostrar GameUI de Juego.
}

void SimulationController::Pause()
{
    if (currentState == GameState::RUNNING) {
        currentState = GameState::PAUSED;
    }
    // Si ya estábamos pausados, al pulsar de nuevo, reanudamos (Toggle Pause)
    else if (currentState == GameState::PAUSED) {
        currentState = GameState::RUNNING;
    }
    // Logica para ocultar o mostrar GameUI de Pausa.
}

void SimulationController::Stop()
{
    // Solo si el juego estaba corriendo o pausado, restauramos el estado inicial.
    if (currentState != GameState::STOPPED) {
        // Llama a la función para restaurar la posición inicial de todos los modelos
        LoadInitialSceneState();
        currentState = GameState::STOPPED;
    }
    // Logica para volver al modo Edición (mostrar herramientas de editor).
}

// =================================================================
// 2. GETTERS Y TIEMPO
// =================================================================

float SimulationController::GetGameDeltaTime(float realDeltaTime) const
{
    // El 'DeltaTime' del juego solo es diferente de cero si estamos RUNNING.
    if (currentState == GameState::RUNNING) {
        // Puedes añadir aquí una escala de tiempo (time_scale * realDeltaTime) si quieres slow motion
        return realDeltaTime;
    }
    // Si está PAUSED o STOPPED, el tiempo de juego es 0.
    return 0.0f;
}

// =================================================================
// 3. FUNCIONES DE COORDINACIÓN (Checkpoint)
// =================================================================

void SimulationController::SaveInitialSceneState()
{
    // ?? IMPORTANTE: Itera sobre todos los modelos/GameObjects y guarda su estado.
    // Necesitas que la clase Model tenga implementada la función SaveInitialState().

    // Acceder a la escena a través de la Application (Singleton)
    Scene* currentScene = Application::GetInstance().scene.get();

    // Itera y guarda
    for (auto& model : currentScene->models) {
        model.SaveInitialState();
    }
    // Si tu escena tiene otros elementos que se mueven (ej: partículas, luces con movimiento),
    // también deberás iterar sobre ellos.
}

void SimulationController::LoadInitialSceneState()
{
    // ?? IMPORTANTE: Itera sobre todos los modelos/GameObjects y restaura su estado.
    // Necesitas que la clase Model tenga implementada la función LoadInitialState().

    // Acceder a la escena a través de la Application (Singleton)
    Scene* currentScene = Application::GetInstance().scene.get();

    // Itera y carga
    for (auto& model : currentScene->models) {
        model.LoadInitialState();
    }
}