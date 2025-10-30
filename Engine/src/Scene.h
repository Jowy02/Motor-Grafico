#pragma once
#include "Module.h"
#include "Model.h"
#include <vector>
#include "Texture.h"

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

	void LoadFBX(const std::string& path);

	void ApplyTextureToSelected(const std::string& path);

	std::vector<Model>models;

private:

	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";

	std::vector<std::string>imagesFiles;
	std::vector<Texture >images;

};