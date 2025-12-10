#pragma once

#include "Module.h" 
#include "Menus.h" 
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include "Time.h"

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

struct CameraSnapshot {
    glm::vec3 Position;
    glm::vec3 Orientation;
    float FOV;
    float nearPlane;
    float farPlane;
    float MOVESPEED;
    float sensitivity;
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
    void Step();

    GameState GetState() const { return currentState; }
    float GetGameDeltaTime(float realDeltaTime) const;

    void SaveInitialSceneState();
    void OnCameraRemovedDuringPlay(Camera* cam);
    void OnCameraCreatedDuringPlay(Camera* cam);

private:
    GameState currentState;

    void LoadInitialSceneState();
    void LoadInitialcamaras();
    std::vector<InitialGameObjectData> savedSceneBlueprints;
    CameraSnapshot savedCamera;

    std::vector<Camera*> addedCamerasDuringPlay;
    std::vector<Camera*> removedCamerasDuringPlay;

};