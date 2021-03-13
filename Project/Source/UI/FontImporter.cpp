#include "FontImporter.h"
#include "Resources/Font.h"

#include "Utils/Logging.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "GL/glew.h"

#include "Utils/Leaks.h"

Font* FontImporter::ImportFont(const std::string& path) {

	FT_Library ft;
	//Initiate freetype library. This piece of code should go into Module initialization
	if (FT_Init_FreeType(&ft)) {
		LOG("Could not init FreeType library.");
		return NULL;
	}

	FT_Face face;

	//Load font as a face
	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		LOG("Could not load the font.");
		return NULL;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	//Disable byte-alignment restriction.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	Font* font = new Font();

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			LOG("Failed to load glyph.");
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x};

		//Store the loaded glyph in the map for later use.
		font->characters.insert(std::pair<char, Character>(c, character));
	}

	//Reset pixel storage mode to default
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//Release resources used by freetype
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return font;
}

/*
void FontImporter::LoadFont(std::string fontPath, std::unordered_map<char, Character>& characters) {
	
	FT_Library ft;

	//Initiate freetype library.
	if (FT_Init_FreeType(&ft)) {
		LOG("Could not init FreeType library.");
		return;
	}

	FT_Face face;

	//Load font as a face
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
		LOG("Could not load the font.");
		return;
	}

	//Set the font size to extract the characters. Setting with to 0 allows to dinamically calculate the width based on the given height.
	FT_Set_Pixel_Sizes(face, 0, 48);

	//Disable byte-alignment restriction.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {

		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			LOG("Failed to load glyph.");
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		//Store the loaded glyph in the map for later use.
		characters.insert(std::pair<char, Character>(c, character));
	}

	//Reset pixel storage mode to default
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//Release resources used by freetype
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}
*/