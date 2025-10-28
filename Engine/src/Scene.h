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

	void FPS_graph();

	void DrawConsole();

	void LogToConsole(const std::string& msg) {
		consoleLog.push_back(msg);
	}


	std::vector<std::string> consoleLog;
	static Scene* instance;
	void Hierarchy_Menu();
	void DrawGameObjectNode(Model* model);

	Model* selectedObj = nullptr;


private:
	std::vector<Model>models;

	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";

	std::vector<Model>listFBX;
	std::vector<std::string>imagesFiles;
	std::vector<Texture >images;

	std::vector<float> fpsHistory;
	uint32_t lastFrameTime = 0;
	float currentFPS = 0.0f;
	int framesCounter = 0;

	float timeAccumulator = 0.0f;
};