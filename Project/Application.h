#pragma once

#include "MSTimer.h"

#include <vector>

enum class UpdateStatus;

class Module;
class ModuleHardwareInfo;
class ModuleRender;
class ModuleEditor;
class ModuleCamera;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModulePrograms;
class ModuleDebugDraw;
class ModuleScene;
class ModuleTime;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	void RequestBrowser(char* url);

public:
	ModuleHardwareInfo* hardware = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditor* editor = nullptr;
	ModulePrograms* programs = nullptr;
	ModuleDebugDraw* debug_draw = nullptr;
	ModuleScene* scene = nullptr;
	ModuleTime* time = nullptr;

	// Application Configuration

	char app_name[20] = "RealBug Engine";
	char organization[20] = "";

private:
	std::vector<Module*> modules;
};

extern Application* App;
