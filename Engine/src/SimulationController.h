#pragma once

#include "Module.h" 
#include "Menus.h" 
#include "glm/glm.hpp"
#include <vector>
#include <string>

struct InitialGameObjectData
{
    std::string modelPath;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
    std::string texturePath; // Agregamos la textura
    bool isHidden;
    int parentID; // Para restaurar la jerarquía
    std::string name;
    int modelId;
};

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

    void SaveInitialSceneState();

private:
    GameState currentState;

    void LoadInitialSceneState();

    std::vector<InitialGameObjectData> savedSceneBlueprints;

};