#pragma once

#include "Module.h"
#include "Model.h"

#include "Pool.h"

class ModuleModels : public Module
{
public:
	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	unsigned LoadModel(const char* file_name);
	void ReleaseModel(unsigned current_model);

public:
	Pool<Model> models;

	unsigned current_model;
};

