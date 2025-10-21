#pragma once
#include "Module.h"
#include "Model.h"

struct SDL_Texture;

class Scene : public Module
{
public:
	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


public:


private:
	std::vector<Model>models;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	Model* myModel = nullptr;
};