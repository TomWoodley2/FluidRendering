#pragma once

#include <GLAD/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//https://learnopengl.com/Getting-started/Shaders

class Shader
{
public:
	unsigned int ID; // program ID

	Shader(const std::string vertexPath, const std::string fragmentPath); // Constructor reads and builds the shader
	void use(); // Use/activate the shader
	// Utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

};

