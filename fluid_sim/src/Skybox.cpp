#include "Skybox.h"

#include <GLAD/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include "Bitmap.h"

Skybox::Skybox(float sizeIn)
{
	float vertices[] =
	{
		-sizeIn, -sizeIn, -sizeIn, // V0
		-sizeIn,  sizeIn, -sizeIn, // V1
		 sizeIn,  sizeIn, -sizeIn, // V2
		 sizeIn, -sizeIn, -sizeIn, // V3

		-sizeIn, -sizeIn,  sizeIn, // V4
		-sizeIn,  sizeIn,  sizeIn, // V5
		 sizeIn,  sizeIn,  sizeIn, // V6
		 sizeIn, -sizeIn,  sizeIn  // V7
	};

	unsigned int indices[] = 
	{
		6, 2, 3, // Right
		7, 6, 3,

		1, 5, 4, // Left
		0, 1, 4,

		6, 5, 1, // Top
		2, 6, 1,

		0, 4, 7, // Bottom
		0, 7, 3,

	    2, 1, 0, // Back
		3, 2, 0,

		5, 6, 7, // Front 
		4, 5, 7
	};

	glGenBuffers(1, &VBO); // Generate both VBO buffers
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO); 
	
	glBindVertexArray(VAO);
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),&vertices[0],  GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW); 

	
	//glEnable(GL_DEBUG_OUTPUT);
	glBindVertexArray(0);
	

	glGenTextures(1, &textureID); // Generate texture names
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // Binds cube map texture before using
	GLuint CubeMapFaces[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, // RIGHT
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // LEFT
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // TOP
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // BOTTOM
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // BACK
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // FRONT
	};

	for (GLint i = 0; i < faces; i++)
	{
		std::string TextureDir = "IMAGES/cubemap" + std::to_string(i) + ".bmp"; // Set directory
		Bitmap bmp = Bitmap::bitmapFromFile(TextureDir); // Load bitmap from directory
		glTexImage2D(CubeMapFaces[i],
			0,
			GL_RGB,
			bmp.width(),
			bmp.height(),
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bmp.pixelBuffer());
		
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);	
	
}

void Skybox::render()
{
	//std::cout << " Rendering Skybox" << std::endl;
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6 * faces, GL_UNSIGNED_INT, (void*)0);
	
	glBindVertexArray(0);
}