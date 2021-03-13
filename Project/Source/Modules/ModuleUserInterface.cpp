#include "ModuleUserInterface.h"

#include "Globals.h"
#include "Utils/Logging.h"
#include "Resources/Font.h"
#include "Application.h"
#include "ModuleFiles.h"

#include "Utils/Leaks.h"

bool ModuleUserInterface::Start() {
	// Testing font importer
	AddFont("./Fonts/fontawesome-webfont.ttf");
	GetCharacter("fontawesome-webfont", 'b');
	std::vector<Character> phrase;
	GetCharactersInString("fontawesome-webfont", "-This Is a test-", phrase);

	return true;
}

bool ModuleUserInterface::CleanUp() {
	// Don't like this, review
	// TODO: call glDeleteTextures for each character in a font
	for (std::unordered_map<std::string, Font*>::iterator it = fonts.begin(); it != fonts.end(); ++it) {
		RELEASE((*it).second); 
	}
	fonts.clear();
	
	return true;
}

void ModuleUserInterface::AddFont(const std::string& fontPath) {

	std::string fontName = App->files->GetFileName(fontPath.c_str());

	std::unordered_map<std::string, Font*>::const_iterator existsKey = fonts.find(fontName);

	if (existsKey == fonts.end()) {
		Font* font = FontImporter::ImportFont(fontPath);
		//FontImporter::LoadFont(fontPath, characters);
		if (font) { // TODO: This is a bad check right now. It will always be initialized. Change it when we have the resource manager
			font->name = fontName;
			fonts.insert(std::pair<std::string, Font*>(fontName, font));
		} else {
			LOG("Couldn't load font %s", fontPath.c_str());
		}
	}	
}

Character ModuleUserInterface::GetCharacter(const std::string& font, char c) { // Should this return a Character*?
	std::unordered_map<std::string, Font*>::const_iterator existsKey = fonts.find(font);

	if (existsKey == fonts.end()) {
		return Character();
	}
	return fonts[font]->characters[c];
}

void ModuleUserInterface::GetCharactersInString(const std::string& font, const std::string& sentence, std::vector<Character>& charsInSentence) {
	std::unordered_map<std::string, Font*>::const_iterator existsKey = fonts.find(font);

	if (existsKey == fonts.end()) {
		return;
	}

	for (std::string::const_iterator i = sentence.begin(); i != sentence.end(); ++i) {
		charsInSentence.push_back(fonts[font]->characters[*i]);
	}
}
