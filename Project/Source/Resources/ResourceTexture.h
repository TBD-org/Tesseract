#pragma once

#include "Resource.h"

#include <string>


class ResourceTexture : public Resource {
public:
	ResourceTexture(UID id, const char* assetFilePath, const char* resourceFilePath);

	void Load() override;
	void Unload() override;

public:
	unsigned int glTexture = 0;

};