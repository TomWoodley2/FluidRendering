#pragma once

typedef unsigned int GLuint;

class Skybox
{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	const int faces = 6;
	GLuint textureID;
public:
	Skybox(float sizeIn); 
	void render();
};