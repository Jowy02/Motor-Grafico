#pragma once
#include "Module.h"
#include "Model.h"
#include <vector>
#include "Texture.h"

struct SDL_Texture;

class Menus : public Module
{
public:
	Menus();

	// Destructor
	virtual ~Menus();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	bool Update(float dt);
	
	// Called before all Updates
	bool PreUpdate();

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


	void FPS_graph();

	void DrawConsole();

	void LogToConsole(const std::string& msg) {
		consoleLog.push_back(msg);
	}

	std::vector<std::string> consoleLog;
	void Hierarchy_Menu();
	void DrawGameObjectNode(Model* model);

	Model* selectedObj = nullptr;

	float GetRAMUsageMB();

	void DrawSystemInfo();
	void DrawAboutWindow();
private:
	std::vector<Model>models;

	std::vector<float> fpsHistory;
	uint32_t lastFrameTime = 0;
	float currentFPS = 0.0f;
	int framesCounter = 0;

	float timeAccumulator = 0.0f;


	bool showConsole = true;
	bool showFPS = true;
	bool showHierarchy = true;
	bool showSystemInfo = true;
	bool showAbout = false;
};