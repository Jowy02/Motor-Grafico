#pragma once

#include "Module.h" 
#include "glm/glm.hpp"

enum class GameState {
    STOPPED, 
    RUNNING,
    PAUSED  
};

class SimulationController : public Module
{
public:
    SimulationController();
    ~SimulationController();

    void Play();
    void Pause();
    void Stop();

    GameState GetState() const { return currentState; }
    float GetGameDeltaTime(float realDeltaTime) const;

private:
    GameState currentState;

    void SaveInitialSceneState();
    void LoadInitialSceneState();
};