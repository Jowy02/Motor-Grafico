#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Model.h"
#include "Input.h"
#include <iostream> 

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <windows.h>
#include "Menus.h"

Scene::Scene() : Module()
{
}

// Destructor
Scene::~Scene()
{
}

bool Scene::Awake()
{
	return true;
}

bool Scene::Start()
{
    Application::GetInstance().scene->LoadFBX("../FBX/BakerHouse.fbx");

    Texture* tex = new Texture("../Images/Baker_house.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    models[0].texturePath = "../Images/Baker_house.png";

    models[0].Mmesh.texture = tex;
    models[0].actualTexture = tex;
    models[0].modelId = 0;

	return true;
}

void Scene::LoadFBX(const std::string& path) 
{
    Model model(path.c_str());
    model.modelId = models.size();
    models.push_back(model);
}

void Scene::ApplyTextureToSelected(const std::string& path) 
{
   
    if (!Application::GetInstance().menus.get()->selectedObj == NULL)
    {
        Texture* tex = new Texture(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

        for (auto& Model : models) 
        {
            if (Model.modelId == Application::GetInstance().menus.get()->selectedObj->modelId)
            {
                Model.Mmesh.texture = tex;
                Model.actualTexture = tex;
                Model.texturePath = path;
            }
        }
    }
    else 
        Application::GetInstance().menus->LogToConsole("ERROR APPLYING TEXTURE, NO OBJECT SELECTED");
}

bool Scene::PreUpdate()
{
    return true;
}

bool Scene::Update(float dt)
{
    for(auto& Model : models) Model.Draw();

	return true;
}

bool Scene::PostUpdate()
{
 	return true;
}

bool Scene::CleanUp() 
{
    Application::GetInstance().menus->LogToConsole("Scene::CleanUp started");

    models.clear();
    imagesFiles.clear();
    for (auto& tex : images) 
        tex.Delete();
    images.clear();

    Application::GetInstance().menus->LogToConsole("Scene::CleanUp completed");
	return true;
}