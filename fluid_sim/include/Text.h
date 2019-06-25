#pragma once

#include <GLAD/glad.h>
#include "glm/glm.hpp"
#include <map>
#include <string>

class Text
{
private:
	// Holds all state information relevant to a character as loaded using FreeType
	// based on https://learnopengl.com/#!In-Practice/Text-Rendering
	struct Character {
		GLuint textureID;   // ID handle of the glyph texture
		glm::ivec2 size;    // Size of glyph
		glm::ivec2 bearing;  // Offset from baseline to left/top of glyph
		GLuint advance;    // Horizontal offset to advance to next glyph
	};
	std::map<GLchar, Character> Characters;
	GLuint font_VAO, font_VBO;

public:
	void setupDefaultFont();
	void render(std::string text, float x, float y, float scale);
};
