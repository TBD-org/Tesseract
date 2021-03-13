#pragma once

#include "Math/float2.h"
#include <unordered_map>

class Font;

namespace FontImporter {
	// Provisional. Waiting for resource manager to have the proper structure
	Font* ImportFont(const std::string& path);
	//void LoadFont(std::string fontPath, std::unordered_map<char, Character>& characters); //Loads font from the given path in the given map.	
};	//namespace FontImporter
