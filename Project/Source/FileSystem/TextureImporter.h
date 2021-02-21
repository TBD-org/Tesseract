#pragma once

#include "FileSystem/JsonValue.h"

namespace TextureImporter {
	bool ImportTexture(const char* filePath, JsonValue jMeta);
	bool ImportCubeMap(const char* filePath, JsonValue jMeta); // namespace TextureImporter
} // namespace TextureImporter