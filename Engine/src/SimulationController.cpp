#include "SimulationController.h"
#include "Application.h" 
#include "Scene.h"       


SimulationController::SimulationController() : Module()
{
    currentState = GameState::STOPPED;
    name = "SimulationController"; 
}

SimulationController::~SimulationController()
{
    
}

void SimulationController::Play()
{
    if (currentState == GameState::STOPPED) {
        SaveInitialSceneState();
    }
    currentState = GameState::RUNNING;
}

void SimulationController::Pause()
{
    if (currentState == GameState::RUNNING) {
        currentState = GameState::PAUSED;
    }
    else if (currentState == GameState::PAUSED) {
        currentState = GameState::RUNNING;
    }
}

void SimulationController::Stop()
{
    if (currentState != GameState::STOPPED) {
        LoadInitialSceneState();
        currentState = GameState::STOPPED;
    }
}


float SimulationController::GetGameDeltaTime(float realDeltaTime) const
{
    if (currentState == GameState::RUNNING) {
        return realDeltaTime;
    }
    return 0.0f;
}

void SimulationController::SaveInitialSceneState()
{

    Scene* currentScene = Application::GetInstance().scene.get();

    for (auto& model : currentScene->models) {
        model.SaveInitialState();
    }
}

void SimulationController::LoadInitialSceneState()
{
    Scene* currentScene = Application::GetInstance().scene.get();

    for (auto& model : currentScene->models) {
        model.LoadInitialState();
    }
}