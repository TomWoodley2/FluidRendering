#include "Cuboid.h"

#include <iostream>

Cuboid::Cuboid(glm::vec3 size, glm::vec3 centre)
{

	// 24 vertices required for mapping normals

	glm::vec3 half_size = glm::vec3(
		size.x / 2.0f,
		size.y / 2.0f,
		size.z / 2.0f
	);

	// Input vertex data
	float vertices[] = {

		  // Back face
		  centre.x-half_size.x, centre.y-half_size.y, centre.z-half_size.z, 
		  centre.x-half_size.x, centre.y+half_size.y, centre.z-half_size.z,
		  centre.x+half_size.x, centre.y+half_size.y, centre.z-half_size.z,
		  centre.x+half_size.x, centre.y-half_size.y, centre.z-half_size.z,

		  // Front face
		  centre.x-half_size.x, centre.y-half_size.y, centre.z+half_size.z,
		  centre.x-half_size.x, centre.y+half_size.y, centre.z+half_size.z,
		  centre.x+half_size.x, centre.y+half_size.y, centre.z+half_size.z,
		  centre.x+half_size.x, centre.y-half_size.y, centre.z+half_size.z,

		  // Left face
		  centre.x-half_size.x, centre.y-half_size.y, centre.z-half_size.z, 
		  centre.x-half_size.x, centre.y+half_size.y, centre.z-half_size.z,
		  centre.x-half_size.x, centre.y-half_size.y, centre.z+half_size.z,
		  centre.x-half_size.x, centre.y+half_size.y, centre.z+half_size.z,

		  // Right face 
		  centre.x+half_size.x, centre.y+half_size.y, centre.z-half_size.z,
		  centre.x+half_size.x, centre.y-half_size.y, centre.z-half_size.z,
		  centre.x+half_size.x, centre.y+half_size.y, centre.z+half_size.z,
		  centre.x+half_size.x, centre.y-half_size.y, centre.z+half_size.z,

		  // Top face
		  centre.x-half_size.x, centre.y+half_size.y, centre.z-half_size.z,
		  centre.x+half_size.x, centre.y+half_size.y, centre.z-half_size.z,
		  centre.x-half_size.x, centre.y+half_size.y, centre.z+half_size.z,
		  centre.x+half_size.x, centre.y+half_size.y, centre.z+half_size.z,

		  // Bottom face
		  centre.x-half_size.x, centre.y-half_size.y, centre.z-half_size.z, 
		  centre.x+half_size.x, centre.y-half_size.y, centre.z-half_size.z,
		  centre.x-half_size.x, centre.y-half_size.y, centre.z+half_size.z,
		  centre.x+half_size.x, centre.y-half_size.y, centre.z+half_size.z

	};

	// Index data
	unsigned int indices[] = {
		// Back face
		0, 1, 2,
		0, 2, 3,

		// Front face
		7, 6, 5,
		7, 5, 4,

		// Left face
		10, 11, 9,
		10, 9, 8,

		// Right face
		13, 12, 14,
		13, 14, 15,
		
		// Top face
		16, 18, 19,
		16, 19, 17,

		// Bottom face
		20, 22, 23, 
		20, 23, 21
	
	};
	
	float normals[] = {
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};
	
	// Element Buffer Object (EBO) - stores indices that openGL uses to decide what vertices to draw
	unsigned int EBO;
	unsigned int VBO; // Vertex Buffer Object - can store a large number of vertices in GPU memory
	unsigned int VBO2;
	glGenBuffers(1, &EBO); // Create EBO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenVertexArrays(1, &VAO); // Generate VAO  - stores all vertex attribute calls

	// VAO initialization code
	// 1 - bind VAO
	glBindVertexArray(VAO);
	// 2 - copy vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	
	// 3 - copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 4 - Set our vertex attributes pointers
	
	glBindVertexArray(0);

}

void Cuboid::render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36 ,GL_UNSIGNED_INT, 0); // size of gets the number of points required for the number of triangles to be drawn
}

glm::vec4 Cuboid::getColour()
{
	return colour;
}

void Cuboid::setColour(glm::vec4 colourIn)
{
	colour = colourIn;
}