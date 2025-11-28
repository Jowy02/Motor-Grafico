#pragma once
#define _USE_MATH_DEFINES

#include "Module.h"
#include "Camera.h"
#include "Texture.h"

#include "SDL3/SDL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <vector>
#include <cmath>

class Model;

// Simple mesh structure for GPU buffers
struct gemotryMesh
{
    GLuint VAO = 0, VBO = 0, EBO = 0;
    unsigned int indexCount = 0;
    Texture* texture = nullptr;
};

class Render : public Module
{
public:
    Render();
    virtual ~Render(); // Destructor
    
    // --- LIFE CYCLE ---
    bool Awake() override;            // Called before render is available
    bool Start() override;            // Called before the first frame
    bool PreUpdate() override;        // Called before each loop iteration
    bool Update(float dt) override;   // Called each loop iteration
    bool PostUpdate() override;       // Called after each loop iteration
    bool CleanUp() override;          // Called before quitting

    // ---- MESH CREATION ---
    void CreatePyramid();
    void CreateCube();          
    void CreateDiamond();       
    void CreateSphere();        
    gemotryMesh CreateGrid(int size, int divisions); 
    gemotryMesh SetBuffers(gemotryMesh Mesh, std::vector<float> vertices, std::vector<unsigned int> indices); // Initialize VAO/VBO/EBO

    // --- NORMAL VISUALIZATION ---
    void ShowFaceNormals();      // Toggle face normals visualization
    void ShowVertexNormals();    // Toggle vertex normals visualization

    // --- MESH DRAWING ---
    gemotryMesh Draw3D(const GLfloat* vertices, size_t vertexCount, const GLuint* indices, size_t indexCount, float rotation, Texture* texture = nullptr); // Draw 3D mesh
    gemotryMesh DrawFaceNormals(const GLfloat* vertices, const GLuint* indices, size_t indexCount, std::vector<float>& outLines); // Draw face normals
    gemotryMesh DrawVertexNormalsFromMesh(const float* vertices, size_t vertexCount, const std::vector<glm::vec3>& vertexNormals, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents, std::vector<float>& outLines); // Draw vertex normals
  

    std::vector<glm::vec3> CalculateVertexNormalsPrueva(const GLfloat* vertices, const GLuint* indices, int vertexCount, int indexCount, float smoothingAngleDeg);


    std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> CalculateTangentsAndBitangents(  const GLfloat* vertices,  const GLuint* indices, const std::vector<glm::vec3>& vertexNormals ,int vertexCount, int indexCount);
 

    void InitRaycastData(Model& model, const GLfloat* vertices, int vertexCount, GLuint* indices, int indexCount);
    void InitRaycastDataSphere(Model& model, const std::vector<float>& vertices, const std::vector<unsigned int>& indices, int stride = 8);


    void OrderModels();
    void FrustumModels();
    bool FaceNormals = false;    
    bool VertexNormals = false;  

    // --- SDL / OpenGL objects ---
    SDL_Renderer* renderer = nullptr;               
    SDL_Color background;             
    unsigned int shaderProgram = 0;   
    unsigned int normalShaderProgram = 0;

    unsigned int selectedProgram = 0; // program to render selected object as white
    unsigned int sobelProgram = 0;    // postprocess sobel program (uses quad vertex shader)


    void DrawAABBOutline(Model& model, glm::vec3 color= glm::vec3(0.0f, 0.8f, 1.0f));

private:
    unsigned int VBO = 0, VAO = 0, EBO = 0; // OpenGL buffers
    unsigned int vertexShader = 0;          // Vertex shader ID
    unsigned int fragmentShader = 0;        // Fragment shader ID

    int numPyramid = 0;
    int numCube = 0;
    int numDiamond = 0;
    int numSphere = 0;

    double prevTime = 0.0;   // Time tracking for updates

    SDL_Window* temp = nullptr; // Temporary window pointer
    std::vector<std::pair<float, int>> modelOrder;


};