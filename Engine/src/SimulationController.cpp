#include "SimulationController.h"
#include "Application.h" 
#include "Scene.h"       
#include "Time.h"


SimulationController::SimulationController() : Module()
{
    currentState = GameState::STOPPED;
    name = "SimulationController";
}

SimulationController::~SimulationController()
{

}

void SimulationController::OnCameraCreatedDuringPlay(Camera* cam)
{
    if (currentState == GameState::RUNNING)
    {
        addedCamerasDuringPlay.push_back(cam);
    }
}

void SimulationController::OnCameraRemovedDuringPlay(Camera* cam)
{
    if (currentState == GameState::RUNNING)
    {
        removedCamerasDuringPlay.push_back(cam);
    }
}

void SimulationController::Play()
{
    if (currentState == GameState::STOPPED) {
        SaveInitialSceneState();

        Camera* cam = Application::GetInstance().camera.get();
        for (auto& cam : Application::GetInstance().scene.get()->cameras) {
            savedCamera.Position = cam->Position;
            savedCamera.Orientation = cam->Orientation;
            savedCamera.FOV = cam->FOV;
            savedCamera.nearPlane = cam->nearPlane;
            savedCamera.farPlane = cam->farPlane;
            savedCamera.MOVESPEED = cam->MOVESPEED;
            savedCamera.sensitivity = cam->sensitivity;
        }
    }


    currentState = GameState::RUNNING;
    Time::Resume();

}

void SimulationController::Pause()
{
    if (currentState == GameState::RUNNING) {
        currentState = GameState::PAUSED;
        Time::Pause();

    }
    else if (currentState == GameState::PAUSED) {
        currentState = GameState::RUNNING;
        Time::Resume();
    }
}

void SimulationController::Stop()
{
    if (currentState != GameState::STOPPED) {
        LoadInitialSceneState();
        LoadInitialcamaras();

        currentState = GameState::STOPPED;
        Time::Init(Time::fixedDeltaTime);
        Time::Pause();
    }

}

void SimulationController::Step()
{
    if (currentState == GameState::STOPPED) {
        SaveInitialSceneState();
        currentState = GameState::PAUSED;
        Time::Pause();
        Time::RequestStepOnce();
        return;
    }

    if (currentState == GameState::RUNNING) {
        return;
    }

    if (currentState == GameState::PAUSED) {
        Time::RequestStepOnce();
    }
}

float SimulationController::GetGameDeltaTime(float realDeltaTime) const
{
    /*if (currentState == GameState::RUNNING) {
        return realDeltaTime;
    }
    return 0.0f;*/

    return Time::deltaTime;

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

    //Clean up
    for (auto& model : currentScene->models)
    {
        model.CleanUpChilds();
        model.CleanUp();
    }
    currentScene->models.clear();

    if (currentScene->octreeRoot)
        currentScene->octreeRoot->Clear();

    // Restaurar Objetos
    std::vector<InitialGameObjectData> modelsToRestore = savedSceneBlueprints;
    savedSceneBlueprints.clear();

    Application::GetInstance().render.get()->CreateGrid(10, 20);

    std::vector<int> newParentIDs;
    int objectStart = currentScene->models.size();

    for (const auto& blueprint : modelsToRestore)
    {
        currentScene->RecreateGameObject(blueprint);
        newParentIDs.push_back(blueprint.parentID);
    }

    // Restaurar jerarquía
    for (int i = 0; i < newParentIDs.size(); ++i)
    {
        int newModelIndex = objectStart + i;
        int parentID = newParentIDs[i];

        if (parentID != -1 && parentID < currentScene->models.size())
        {
            currentScene->models[parentID].SetChild(&currentScene->models[newModelIndex]);
        }
    }

    // Reconstruir octree
    currentScene->BuildOctree();

    // Reset selección y estado
    Application::GetInstance().menus.get()->selectedObj = nullptr;
    currentState = GameState::STOPPED;
}

void SimulationController::LoadInitialcamaras() {

    auto& menusCams = Application::GetInstance().scene.get()->cameras;

    //Eliminar
    for (Camera* cam : addedCamerasDuringPlay)
    {
        auto it = std::find(menusCams.begin(), menusCams.end(), cam);
        if (it != menusCams.end())
        {
            menusCams.erase(it);
            delete cam;
        }
    }
    addedCamerasDuringPlay.clear();

    //Restaurar
    for (Camera* cam : removedCamerasDuringPlay)
    {
        menusCams.push_back(cam);
    }
    removedCamerasDuringPlay.clear();

    for (Camera* cam : menusCams)
    {
        cam->UpdateViewMatrix();
        cam->UpdateProjectionMatrix();
    }


    Camera* cam = Application::GetInstance().camera.get();


    for (auto& cam : Application::GetInstance().scene.get()->cameras) {
        cam->Position = savedCamera.Position;
        cam->Orientation = savedCamera.Orientation;
        cam->FOV = savedCamera.FOV;
        cam->nearPlane = savedCamera.nearPlane;
        cam->farPlane = savedCamera.farPlane;
        cam->MOVESPEED = savedCamera.MOVESPEED;
        cam->sensitivity = savedCamera.sensitivity;

        cam->UpdateViewMatrix();
        cam->UpdateProjectionMatrix();
    }
}