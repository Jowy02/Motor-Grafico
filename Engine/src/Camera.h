#pragma once

#include "Window.h"
#include "Render.h"
#include "Module.h"

#include "Application.h"

#include<glad/glad.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

class Camera : public Module
{
public:
	Camera();

	// Destructor
	virtual ~Camera();

	bool Awake();
	bool CleanUp();

	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float MOVESPEED = 0.1f;
	float speed = MOVESPEED;
	float sensitivity = 0.1f;
	
	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, GLuint shaderID);
	// Handles camera inputs
	void Inputs(SDL_Window* window);
};