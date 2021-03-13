#pragma once

#include "Module.h"
#include "UI/FontImporter.h"

class Font;
struct Character;

class ModuleUserInterface : public Module {
public:

	bool Start();

	bool CleanUp();

	void AddFont(const std::string& fontPath); //Try to load the font and store it.
	Character GetCharacter(const std::string& font, char c); //Returns the Character that matches the given one in the given font or null otherwise.
	void GetCharactersInString(const std::string& font, const std::string& sentence, std::vector<Character>& charsInSentence); //Fills the given vector with the glyphs of the given font to form the given sentence.

private:
	std::unordered_map<std::string, Font*> fonts; //Container for different fonts with the name of the font as key.
};
