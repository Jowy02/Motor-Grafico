#pragma once
#define _USE_MATH_DEFINES

#include "Module.h"
#include "Camera.h"
#include "Texture.h"

#include "SDL3/SDL.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp> 
#include<glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <vector>
#include <cmath>

struct gemotryMesh {
	GLuint VAO = 0, VBO = 0, EBO = 0;
	unsigned int indexCount = 0;
	Texture* texture = nullptr;

};

class Render : public Module
{
public:
	Render();

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CreateTriangle();
	void CreateCube();
	void CreateDiamond();
	void CreateSphere();

	gemotryMesh Draw3D(const GLfloat* vertices, size_t vertexCount, const GLuint* indices, size_t indexCount, float rotation, Texture* texture = nullptr);
	gemotryMesh CreateGrid(int size, int divisions);
	gemotryMesh SetBuffers(gemotryMesh Mesh, std::vector<float> vertices, std::vector<unsigned int> indices);


	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();

	SDL_Renderer* renderer;
	SDL_Rect camera;
	SDL_Rect viewport;
	SDL_Color background;
	unsigned int shaderProgram;
	unsigned int normalShaderProgram;

	gemotryMesh DrawFaceNormals(const GLfloat* vertices, const GLuint* indices, size_t indexCount, std::vector<float>& outLines);
	gemotryMesh DrawVertexNormalsFromMesh(const float* vertices, size_t vertexCount, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents, std::vector<float>& outLines);

	void ShowFaceNormals();
	void ShowVertexNormals();
	bool FaceNormals = false;
	bool VertexNormals = false;

private:
	unsigned int VBO, VAO, EBO;

	unsigned int vertexShader;
	unsigned int fragmentShader;

	double prevTime;

	SDL_Window* temp;

};