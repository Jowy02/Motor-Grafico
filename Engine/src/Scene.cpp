#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Model.h"


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
    models.push_back(Model("C:/Users/joelv/Downloads/warrior.fbx"));

	return true;
}

bool Scene::PreUpdate()
{

	return true;
}

bool Scene::Update(float dt)
{

    for(auto& Model : models) Model.Draw();
   

    ////Triangle
    //GLfloat vertices2[] =
    //{ //     COORDINATES     /        COLORS      /   TexCoord  //
    //    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    //    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
    //     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    //     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
    //     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
    //};

    //// Indices for vertices order
    //GLuint indices2[] =
    //{
    //    0, 1, 2,
    //    0, 2, 3,
    //    0, 1, 4,
    //    1, 2, 4,
    //    2, 3, 4,
    //    3, 0, 4
    //};
    //int vertexCount = sizeof(vertices2) / sizeof(float);
    //int indexCount = sizeof(indices2) / sizeof(unsigned int);
    //// Cada frame:
    //Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount,60.0f);

 //   //Cuadrado
 //   GLfloat vertices2[] =
 //   { //     COORDINATES     /        COLORS      /   TexCoord  //
 //      -0.5f, -0.5f, -0.5f,    0.80f, 0.40f, 0.0f,    0.0f, 0.0f, // 0
 //       0.5f, -0.5f, -0.5f,    0.80f, 0.40f, 0.0f,    1.0f, 0.0f, // 1
 //       0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // 2
 //      -0.5f,  0.5f, -0.5f,    0.80f, 0.40f, 0.0f,    0.0f, 1.0f, // 3
 //      -0.5f, -0.5f,  0.5f,    0.80f, 0.40f, 0.0f,    0.0f, 0.0f, // 4
 //       0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // 5
 //       0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // 6
 //      -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f  // 7
 //   };

 //   // Indices for vertices order
 //   GLuint indices2[] =
 //   {
 //       // Cara trasera
 //       0, 1, 2, 2, 3, 0,
 //       // Cara delantera
 //       4, 5, 6, 6, 7, 4,
 //       // Cara izquierda
 //       0, 4, 7, 7, 3, 0,
 //       // Cara derecha
 //       1, 5, 6, 6, 2, 1,
 //       // Cara inferior
 //       0, 1, 5, 5, 4, 0,
 //       // Cara superior
 //       3, 2, 6, 6, 7, 3
 //   };
 //   int vertexCount = sizeof(vertices2) / sizeof(float);
 //   int indexCount = sizeof(indices2) / sizeof(unsigned int);
 //   // Cada frame:
 //   Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 60.0f);

    ////Rombo
    //GLfloat vertices2[] =
    //{ //     COORDINATES     /        COLORS      /   TexCoord  //
    //     0.0f,  0.5f,  0.0f,     1.0f, 0.8f, 0.2f, 0.5f, 1.0f, // top
    //     0.5f,  0.0f,  0.0f,     1.0f, 0.0f, 0.0f, 1.0f, 0.5f,
    //     0.0f,  0.0f,  0.5f,     0.0f, 1.0f, 0.0f, 0.5f, 0.0f,
    //    -0.5f,  0.0f,  0.0f,     0.0f, 0.0f, 1.0f, 0.0f, 0.5f,
    //     0.0f,  0.0f, -0.5f,     1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
    //     0.0f, -0.5f,  0.0f,     0.3f, 0.3f, 0.3f, 0.5f, 0.5f  // bottom
    //};

    //// Indices for vertices order
    //GLuint indices2[] =
    //{
    //    // Mitad superior
    //    0, 1, 2,
    //    0, 2, 3,
    //    0, 3, 4,
    //    0, 4, 1,
    //    // Mitad inferior
    //    5, 2, 1,
    //    5, 3, 2,
    //    5, 4, 3,
    //    5, 1, 4
    //};
    //int vertexCount = sizeof(vertices2) / sizeof(float);
    //int indexCount = sizeof(indices2) / sizeof(unsigned int);
    //// Cada frame:
    //Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 60.0f);



	return true;

}

bool Scene::PostUpdate()
{
	return true;
}

bool Scene::CleanUp() {
    if (myModel)
    {
        delete myModel;
        myModel = nullptr;
    }

	return true;
}