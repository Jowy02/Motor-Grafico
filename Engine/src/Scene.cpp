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
    listFBX.push_back(Model("../FBX/warrior.fbx"));

    //std::string devILPath = std::string("C:../Devil/");
    //std::string devILPath = std::string("C:/Users/Usuari/Documents/GitHub/Motor-Grafico/Engine/Devil/");



    std::string parentDir = std::string("../Images/");
    imagesFiles.push_back(std::string("Bird.png"));


    for (size_t i = 0; i < imagesFiles.size(); ++i)
    {
        std::string fullPath = parentDir + imagesFiles[i];
        // Crear la textura con tu clase Texture (usa DevIL internamente)
        Texture tex(fullPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + i, GL_RGBA, GL_UNSIGNED_BYTE);
        images.push_back(tex);

    }
	return true;
}

bool Scene::PreUpdate()
{

	return true;
}

bool Scene::Update(float dt)
{

    for(auto& Model : models) Model.Draw();
   
    GLuint shaderProgram = Application::GetInstance().render->shaderProgram;

    /* for (auto& modelFBX : listFBX) {
         modelFBX.Draw(shaderProgram);
     }*/

    for (int i = 0; i < images.size(); i++)
    {
        images[i].texUnit(shaderProgram, "tex0", 0);
        images[i].Bind();

        GLfloat vertices2[] =
        { 
            -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // inferior izquierda
             0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // inferior derecha
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // superior derecha
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f  // superior izquierda
        };

        //// Indices para formar dos triángulos
        GLuint indices2[] =
        {
            0, 1, 2,  // primer triángulo
            2, 3, 0   // segundo triángulo
        };

        int vertexCount = sizeof(vertices2) / sizeof(float);
        int indexCount = sizeof(indices2) / sizeof(unsigned int);
        Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 0.0f, &images[i]);

        images[i].Unbind();
    }

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
    listFBX.clear();
    imagesFiles.clear();
    for (auto& tex : images) {
        tex.Delete();
    }
    images.clear();


	return true;
}