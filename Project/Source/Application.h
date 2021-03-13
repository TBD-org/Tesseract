#pragma once

#include "Utils/MSTimer.h"
#include <vector>

enum class UpdateStatus;

class Module;
class ModuleHardwareInfo;
class ModuleSceneRender;
class ModuleRender;
class ModuleEditor;
class ModuleCamera;
class ModuleWindow;
class ModuleResources;
class ModuleFiles;
class ModuleInput;
class ModulePrograms;
class ModuleDebugDraw;
class ModuleScene;
class ModuleTime;
class ModuleEventSystem;
class ModuleUserInterface;

struct Event;
class Application {
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	void RequestBrowser(char* url);
	void BroadCastEvent(const Event& e);

public:
	ModuleHardwareInfo* hardware = nullptr;
	ModuleResources* resources = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleWindow* window = nullptr;
	ModuleFiles* files = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditor* editor = nullptr;
	ModulePrograms* programs = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleScene* scene = nullptr;
	ModuleTime* time = nullptr;
	ModuleEventSystem* eventSystem = nullptr;
	ModuleUserInterface* UI = nullptr;
	// Application Configuration

	char appName[20] = "Tesseract";
	char organization[20] = "";

private:
	std::vector<Module*> modules;
};

extern Application* App;
