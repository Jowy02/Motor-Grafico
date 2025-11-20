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

//    std::string parentDir = std::string("../Images/");
//imagesFiles.push_back(std::string("textura.png"));
//
//for (size_t i = 0; i < imagesFiles.size(); ++i)
//{
//    std::string fullPath = parentDir + imagesFiles[i];
//    // Crear la textura con tu clase Texture (usa DevIL internamente)
//    Texture tex(fullPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + i, GL_RGBA, GL_UNSIGNED_BYTE);
//    images.push_back(tex);
//}

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
   
    //if (Application::GetInstance().menus.get()->selectedObj != NULL)
    //    //if (!Application::GetInstance().menus.get()->selectedObj == NULL)
    //{
    //    Texture* tex = new Texture(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

    //    for (auto& Model : models) 
    //    {
    //        if (Model.name == Application::GetInstance().menus.get()->selectedObj->name) 
    //        {
    //            Model.Mmesh.texture = tex;
    //            Model.actualTexture = tex;
    //            Model.texturePath = path;
    //        }
    //    }
    //}
    //else 
    //    Application::GetInstance().menus->LogToConsole("ERROR APPLYING TEXTURE, NO OBJECT SELECTED");

    auto selected = Application::GetInstance().menus.get()->selectedObj;
    if (selected)
    {
        Texture* tex = new Texture(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

        for (auto& model : models)
        {

            if (Model.modelId == Application::GetInstance().menus.get()->selectedObj->modelId)
            {
                model.Mmesh.texture = tex;
                model.actualTexture = tex;
                model.texturePath = path;
                model.hasTransparency = tex->hasAlpha;
            }
        }
    }
    else
    {
        Application::GetInstance().menus->LogToConsole("ERROR APPLYING TEXTURE, NO OBJECT SELECTED");
    }

}

bool Scene::PreUpdate()
{
    return true;
}

bool Scene::Update(float dt)
{
    for(auto& Model : models) Model.Draw();
    //GLuint shaderProgram = Application::GetInstance().render->shaderProgram;


 
    ////Image 2D
    //for (int i = 0; i < images.size(); i++)
    //{
    //    images[i].texUnit(shaderProgram, "tex0", 0);
    //    images[i].Bind();

    //    GLfloat vertices2[] =
    //    {
    //        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // inferior izquierda
    //         0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // inferior derecha
    //         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // superior derecha
    //        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f  // superior izquierda
    //    };

    //    //// Indices para formar dos tri�ngulos
    //    GLuint indices2[] =
    //    {
    //        0, 1, 2,  // primer tri�ngulo
    //        2, 3, 0   // segundo tri�ngulo
    //    };

    //    int vertexCount = sizeof(vertices2) / sizeof(float);
    //    int indexCount = sizeof(indices2) / sizeof(unsigned int);
    //    Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 0.0f, &images[i]);

    //    images[i].Unbind();
    //}


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