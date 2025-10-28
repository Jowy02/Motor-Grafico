#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Model.h"
#include <iostream> 

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

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
    //listFBX.push_back(Model("../FBX/BakerHouse.fbx"));
   
    //std::string texPath = "../Images/Baker_house.png";
    //Texture* tex = new Texture(texPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

    //listFBX[0].Mmesh.texture = tex;



    //std::string parentDir = std::string("../Images/");
    //imagesFiles.push_back(std::string("textura.png"));

    //for (size_t i = 0; i < imagesFiles.size(); ++i)
    //{
    //    std::string fullPath = parentDir + imagesFiles[i];
    //    // Crear la textura con tu clase Texture (usa DevIL internamente)
    //    Texture tex(fullPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + i, GL_RGBA, GL_UNSIGNED_BYTE);
    //    images.push_back(tex);
    //}

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    SDL_Window* window = Application::GetInstance().window->GetSDLWindow(); // Asegúrate de tener este método
    SDL_GLContext gl_context = Application::GetInstance().window->GetGLContext(); // También necesitas esto

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");


	return true;
}

void Scene::LoadFBX(const std::string& path) {
    Model model(path.c_str());
    models.push_back(model);
}

void Scene::ApplyTextureToSelected(const std::string& path) {
   
    //if (!models.empty()) 
    //{
        Texture* tex = new Texture(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

        models.back().Mmesh.texture = tex;
    /*}*/
    //else {
    //    Texture tex(path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    //    images.push_back(tex);
    //}
  
}


bool Scene::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

	return true;
}

bool Scene::Update(float dt)
{

    for(auto& Model : models) Model.Draw();
   
    GLuint shaderProgram = Application::GetInstance().render->shaderProgram;

     for (auto& modelFBX : listFBX) {
         modelFBX.Draw();
     }

    //Image 2D
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

    //Imatge 3d
    //for (int i = 0; i < images.size(); i++)
    //{
    //    GLuint shaderProgram = Application::GetInstance().render->shaderProgram;

    //    // Configuramos la textura (asumiendo que brickTex es tu Texture ya cargada)

    //    images[i].texUnit(shaderProgram, "tex0", 0);
    //    images[i].Bind();

    //    // Vertices de la pirámide: posición (x,y,z), color (r,g,b), textura (u,v)
    //    GLfloat vertices[] =
    //    { //     COORDINATES     /        COLORS      /   TexCoord  //
    //        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    //        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 1.0f,
    //         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 1.0f,
    //         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 0.0f,
    //         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	0.5f, 1.0f
    //    };

    //    // Indices for vertices order
    //    GLuint indices[] =
    //    {
    //        0, 1, 2,
    //        0, 2, 3,
    //        0, 1, 4,
    //        1, 2, 4,
    //        2, 3, 4,
    //        3, 0, 4
    //    };

    //    int vertexCount = sizeof(vertices) / sizeof(float);
    //    int indexCount = sizeof(indices) / sizeof(unsigned int);

    //    const Uint8* state = SDL_GetKeyboardState(NULL);
    //    glm::vec3 dir = Application::GetInstance().camera->Orientation;
    //    float rotation = glm::degrees(atan2(dir.x, dir.z));

    //    // Transformaciones
    //    glm::mat4 model = glm::mat4(1.0f);
    //    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    //    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)Application::GetInstance().window.get()->width / Application::GetInstance().window.get()->height, 0.1f, 100.0f);

    //    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    //    // Enviar matrices al shader
    //    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    //    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    //    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    //    int projLoc = glGetUniformLocation(shaderProgram, "proj");
    //    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    //    // Dibujar la pirámide
    //    Application::GetInstance().render.get()->Draw3D(vertices, vertexCount, indices, indexCount, 60.0f, &images[i]);


    //}

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

framesCounter++;
timeAccumulator += dt;

if (timeAccumulator >= 1.0f) // cada segundo
{
    currentFPS = (float)framesCounter / timeAccumulator;
    fpsHistory.push_back(currentFPS);

    if (fpsHistory.size() > 100) // límite para el gráfico
        fpsHistory.erase(fpsHistory.begin());

    framesCounter = 0;
    timeAccumulator = 0.0f;

    std::cout << "FPS: " << currentFPS << std::endl;

}

FPS_graph();


	return true;

}

void Scene::FPS_graph()
{
    ImGui::Begin("FPS Monitor");
    ImGui::SetWindowSize(ImVec2(550, 400), ImGuiCond_Always);
    ImGui::Text("FPS actual: %.1f", currentFPS);
    if (!fpsHistory.empty()) {
        ImGui::PlotLines("FPS", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 120.0f, ImVec2(500, 300));
    }
    ImGui::End();
}

bool Scene::PostUpdate()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return true;
}

bool Scene::CleanUp() {
    listFBX.clear();
    imagesFiles.clear();
    for (auto& tex : images) {
        tex.Delete();
    }
    images.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

	return true;
}