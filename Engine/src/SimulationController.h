#pragma once

#include "Module.h" // Si hereda de Module
#include "glm/glm.hpp"

// 1. Definir los posibles estados
enum class GameState {
    STOPPED, // Estado inicial / Modo Edición
    RUNNING, // Modo Juego
    PAUSED   // Modo Pausa
};

class SimulationController : public Module // O simplemente una clase Singleton
{
public:
    SimulationController();
    ~SimulationController();

    // 2. Funciones de Control Público
    void Play();
    void Pause();
    void Stop();

    // 3. Getters para el estado y el tiempo
    GameState GetState() const { return currentState; }
    float GetGameDeltaTime(float realDeltaTime) const;

private:
    GameState currentState;

    // 4. Funciones de Coordinación
    void SaveInitialSceneState();
    void LoadInitialSceneState();
};