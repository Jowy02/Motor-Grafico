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
    savedSceneBlueprints.clear();

    for (auto& model : currentScene->models) {
      
        model.SaveInitialState();

        savedSceneBlueprints.push_back({
                    model.modelPath,
                    model.GetInitialPosition(),
                    model.GetInitialRotation(),
                    model.GetInitialScale(),
                    model.GetInitialTexturePath(),
                    model.GetInitialIsHidden(),
                    model.GetInitialParentID(),
                    model.name,
                    model.modelId
            });
        
    }
}


void SimulationController::LoadInitialSceneState()
{
    Scene* currentScene = Application::GetInstance().scene.get();

    for (auto& model : currentScene->models)
    {
        model.CleanUpChilds();
        model.CleanUp();
    }
    currentScene->models.clear(); // limpiar vector
    currentScene->octreeRoot.get()->Clear(); // Limpiar el Octree

    std::vector<InitialGameObjectData> modelsToRestore = savedSceneBlueprints;
    savedSceneBlueprints.clear();

    Application::GetInstance().render.get()->CreateGrid(10, 20);

   int objectStart = currentScene->models.size();

    if (!currentScene->models.empty() && currentScene->models[0].name == "Grid")
    {
        currentScene->models[0].position;
    }

    std::vector<int> newParentIDs;
    for (const auto& blueprint : modelsToRestore)
    {
        currentScene->RecreateGameObject(blueprint);

        newParentIDs.push_back(blueprint.parentID);
    }


    for (int i = 0; i < newParentIDs.size(); ++i)
    {
        int newModelIndex = objectStart + i; 
        int parentID = newParentIDs[i];

        if (parentID != -1) {
            if (parentID < currentScene->models.size()) {
                currentScene->models[parentID].SetChild(&currentScene->models[newModelIndex]);
            }
        }
    }

    currentScene->BuildOctree();
    Application::GetInstance().menus.get()->selectedObj = nullptr;
    currentState = GameState::STOPPED;
}
