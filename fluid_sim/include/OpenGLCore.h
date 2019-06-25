#pragma once

/*! \class OpenGLCore
 * Contains all core elements for setting up OpenGL
 */

#include <GLAD/glad.h> // Glad must be initiated before GLFW as contains openGL header includes
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <chrono> // Timing
#include "ParticleGenerator.h"
#include "Shader.h"
#include "Cuboid.h"
#include "Camera.h"
#include "Skybox.h"
#include "Text.h"

class OpenGLCore
{
private:

	// CAMERA MOVEMENT SPEED
	//const float cameraMovementSpeed = 0.01f; 
	const float cameraMovementSpeed = 0.03f;

	// CAMERA ROTATION SPEED
	//const float cameraRotationSpeed = 0.002f;
	const float cameraRotationSpeed = 0.015f;

	// Shaders
	Shader* m_ObsShader;
	Shader* m_PhongShader;
	Shader* m_WaterShader;
	Shader* m_SkyboxShader;
	Shader* m_FontShader;

	void processInput(GLFWwindow *window); //! Process any inputs
	void CameraUniformsToShader(unsigned int shaderID); //! Send camera uniforms to the specified shader

	Cuboid * m_cube; // CUBE
	Cuboid * m_plane; // PLANE

	int currentGenerator = 0;
	std::vector<ParticleGenerator*> m_Generators; // Stores all particle generators


	ParticleGenerator * m_currentParticleGenerator; // Points to the particle generator which is currently in use
	Skybox * m_skybox; // SKYBOX
	Text * m_text; // On screen text

	
	Camera m_camera;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	GLFWwindow* window;

	unsigned int currentShaderID;

	

	// Manage 64 frames for frame rate
	static const int framesToMonitor = 64;
	std::vector<double> frameTimes;

	bool drawParticles = false;
	bool isRenderingAsLines = false; // Used to keep skybox when rendering scene as lines
	bool isRenderingSinglePlane = false; // Used for rendering a single plane
	std::string planeName; // Name of the plane to render
	bool isPaused = true; // Set if the game is paused or not
	bool isDebug = true; // Set if the debug mode is on or off
	void key_callback(GLFWwindow* windowIn, int key, int scancode, int action, int mods);
	void renderText();
	
	// Size of window
	float windowSizeX = 800.0f;
	float windowSizeY = 600.0f;
	bool isFullscreen = false;

	void update(float dt); // Update the scene
	void render(); // Render the scene
	
	void AssignPhongUniforms(); // Setup of the phong uniforms for phong shading


public:
	OpenGLCore();
	int Run();
};
