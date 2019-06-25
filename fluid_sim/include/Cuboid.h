#pragma once

#include <GLAD/glad.h> 
#include <GLFW/glfw3.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class Cuboid 
{
private:
	unsigned int VAO; 
	glm::vec4 colour = glm::vec4(0.5f,0.5f,0.5f,1.0f); // Colour of object
public:
	Cuboid(glm::vec3 size, glm::vec3 centre);
	void render();

	glm::vec4 getColour();
	void setColour(glm::vec4 colourIn);
};