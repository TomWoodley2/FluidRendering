#include "OpenGLCore.h"


OpenGLCore::OpenGLCore()
{	
	glfwInit(); // Initialize GLFW

	http://www.glfw.org/docs/latest/window.html#window_hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Configuring GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window object
	window = glfwCreateWindow(windowSizeX, windowSizeY, "Fluid Simulation ", NULL, NULL);
	//window = glfwCreateWindow(1920,1080, "Fluid Simulation", glfwGetPrimaryMonitor(), NULL);

	

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD  (should be done before calling any openGL function) - manages function pointers for openGL 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	//glViewport(0, 0, 800, 600); // Tell openGL size of rendering window
	glViewport(0, 0, windowSizeX, windowSizeY);
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE); // Turn face culling on
	//glDisable(GL_CULL_FACE); // Turned face culling off

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Used for key input
	// https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback

	glfwSetWindowUserPointer(window, this);
	auto func = [](GLFWwindow* w, int key, int scancode, int action, int mods)
	{
		static_cast<OpenGLCore*>(glfwGetWindowUserPointer(w))->key_callback(w,key,scancode,action,mods);
	};

	glfwSetKeyCallback(window, func);

	// map storing shader name against the actual shader pointer
	std::vector<std::string> shader_names = {
		"obstacle",
		"phong",
		"fluid",
		"skybox",
		"font"
	};

	_bind_shaders(shader_names); // bind the shaders to respective shader classes
	
}

// Callback for when window gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// Used for toggling keys
void OpenGLCore::key_callback(GLFWwindow* windowIn, int key, int scancode, int action, int mods) 
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_1 :
			isRenderingAsLines = !isRenderingAsLines;
			break;

		case GLFW_KEY_2 :
			drawParticles = !drawParticles;
			isRenderingSinglePlane = false;
			break;

		case GLFW_KEY_P :
			isPaused = !isPaused;
			break;

		case GLFW_KEY_TAB :
			isDebug = !isDebug;
			break;

		case GLFW_KEY_LEFT :
			// More to the next left generator
			if (currentGenerator != 0)
			{
				currentGenerator -= 1;
			}
			else
			{
				currentGenerator = m_Generators.size() - 1;
			}
			m_currentParticleGenerator = m_Generators[currentGenerator];
			break;
		case GLFW_KEY_RIGHT :
			// Move to the next right generator
			if (currentGenerator != m_Generators.size() - 1)
			{
				currentGenerator += 1;
			}
			else
			{
				currentGenerator = 0;
			}
			m_currentParticleGenerator = m_Generators[currentGenerator];
			break;

		case GLFW_KEY_F :
			if (!isFullscreen)
			{
				windowSizeX = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
				windowSizeY = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, windowSizeX, windowSizeY, NULL);
				isFullscreen = true;
			}
			else
			{
				windowSizeX = 800.0f;
				windowSizeY = 600.0f;
				glfwSetWindowMonitor(window, NULL, 0, 0, windowSizeX, windowSizeY, NULL);
				//glfwRestoreWindow(window);
				isFullscreen = false;
			}

			glViewport(0, 0, windowSizeX, windowSizeY);
			projection = glm::perspective(glm::radians(70.0f), windowSizeX / windowSizeY, 0.1f, 100.0f);
			break;
		}
	
	}
	
}

//INPUTS
void OpenGLCore::processInput(GLFWwindow *window)
{
	// Close the window if ESC pressed
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Rendering single planes
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
	{
		isRenderingSinglePlane = true;
		planeName = "front";
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
	{
		isRenderingSinglePlane = true;
		planeName = "back";
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
	{
		isRenderingSinglePlane = true;
		planeName = "left";
	}
	if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
	{
		isRenderingSinglePlane = true;
		planeName = "right";
	}
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
	{
		isRenderingSinglePlane = true;
		planeName = "top";
	}
	if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS)
	{
		isRenderingSinglePlane = true;
		planeName = "bottom";
	}

	// CAMERA MOVEMENT
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		// Move forward
		m_camera.translate(0,0, -cameraMovementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		// Move backward
		m_camera.translate(0,0, cameraMovementSpeed);
		
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		// Move left
		m_camera.translate(-cameraMovementSpeed,0,0);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		// Move right
		m_camera.translate(cameraMovementSpeed,0,0);
	}

	// CAMERA ROTATION
	
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		// pitch -
		m_camera.pitch(cameraRotationSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		// pitch +
		m_camera.pitch(-cameraRotationSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		// yaw +
		m_camera.yaw(cameraRotationSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		// yaw -
		m_camera.yaw(-cameraRotationSpeed);
		
	}
}

void OpenGLCore::CameraUniformsToShader(unsigned int shaderID)
{
	// Send all the camera matrices to the current shader in use
	int modelLoc = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(shaderID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projLoc = glGetUniformLocation(shaderID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void OpenGLCore::AssignPhongUniforms()
{
	// Setup phong lighting
	shader_data["phong"]->use();

	glm::vec3 worldLight           = glm::vec3(4.0f, 10.0f, -10.0f);
	glm::vec3 ambient_intensity    = glm::vec3(0.5f,  0.5f,   0.5f);
	glm::vec3 point_intensity      = glm::vec3(0.9f,  0.9f,   0.9f);
	glm::vec3 ambient_coefficient  = glm::vec3(0.3f,  0.3f,   0.3f);
	glm::vec3 specular_coefficient = glm::vec3(0.29f, 0.29f,  0.29f);

	// Ambient intensity
	int IaLoc = glGetUniformLocation(shader_data["phong"]->ID, "Ia");
	glUniform3fv(IaLoc,1, glm::value_ptr(ambient_intensity)); 
	
	// Point intensity
	int IpLoc = glGetUniformLocation(shader_data["phong"]->ID, "Ip");
	glUniform3fv(IpLoc,1, glm::value_ptr(point_intensity));

	// Camera position to base shading on
	int CameraPosLoc = glGetUniformLocation(shader_data["phong"]->ID, "CameraPosition");
	glUniform3fv(CameraPosLoc,1, glm::value_ptr(m_camera.getPosition())); 
	
	// Position of light
	int LightPosLoc = glGetUniformLocation(shader_data["phong"]->ID, "LightPosition"); 
	glUniform3fv(LightPosLoc,1, glm::value_ptr(worldLight)); 
	
	// Ambient coeff
	int KaLoc = glGetUniformLocation(shader_data["phong"]->ID, "Ka"); 
	glUniform3fv(KaLoc,1, glm::value_ptr(ambient_coefficient)); 

	// Specular coeff
	int KsLoc = glGetUniformLocation(shader_data["phong"]->ID, "Ks"); 
	glUniform3fv(KsLoc,1, glm::value_ptr(specular_coefficient));
}


void OpenGLCore::update(float dt)
{
	if (!isPaused)
	{
		m_currentParticleGenerator->update(dt); // Update the particles
	}
}

void OpenGLCore::renderText()
{
	// Change the colour depending on which array is targeted
	glm::vec4 currentGeneratorColour = m_currentParticleGenerator->getColour();
	glUniform3f(glGetUniformLocation(shader_data["font"]->ID, "textColour"),currentGeneratorColour.r , currentGeneratorColour.g, currentGeneratorColour.b);
	m_text->render("Active Array: "+ std::to_string(currentGenerator), 0.02, 0.90, 0.4);

	glUniform3f(glGetUniformLocation(shader_data["font"]->ID, "textColour"),0.0f, 0.0f, 0.0f);
	m_text->render("Particle Count: "+std::to_string(m_Generators[currentGenerator]->getParticleCount()),0.02,0.95,0.4);
	m_text->render("Controls:",0.02,0.34,0.4);
	m_text->render("P - Pause/Resume Simulation",0.02,0.30,0.4);
	m_text->render("Left, Right - Change active particle array ",0.02,0.26,0.4);
	m_text->render("W, A, S, D - Camera Movement",0.02,0.22,0.4);
	m_text->render("I, J, K, L - Camera Rotation",0.02,0.18,0.4);
	m_text->render("1 - Toggle Wireframe",0.02,0.14,0.4);
	m_text->render("2 - Toggle Particles/Solid",0.02,0.10,0.4);
	m_text->render("F1 - F6 - Render a single plane of particle arrays",0.02,0.06,0.4);
	m_text->render("TAB - Toggle UI mode",0.02,0.02,0.4);
	
}


void OpenGLCore::render()
{
	// General line fill settings
	if (isRenderingAsLines)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// -- SKYBOX --
	shader_data["skybox"]->use();
	model[3] = glm::vec4(m_camera.getPosition(), 1.0);
	currentShaderID = shader_data["skybox"]->ID;
	CameraUniformsToShader(currentShaderID);
	// Check if currently drawing outline scene and swap to render skybox if required
	glDisable(GL_DEPTH_TEST); // Disabled for skybox display
	if (isRenderingAsLines )
	{
			
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_skybox->render();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		m_skybox->render();
	}
	glEnable(GL_DEPTH_TEST);
	model = glm::mat4(1.0f);

	// -- OBSTACLES -- 
	shader_data["phong"]->use();
	currentShaderID = shader_data["phong"]->ID;
	CameraUniformsToShader(currentShaderID); // Send transformation data to shader - has to be done per frame
	// Cube
	glUniform4fv(glGetUniformLocation(currentShaderID, "colour"),1, glm::value_ptr(m_cube->getColour()));
	m_cube->render();
	// Plane
	glUniform4fv(glGetUniformLocation(currentShaderID, "colour"),1, glm::value_ptr(m_plane->getColour()));
	m_plane->render();

	// -- PARTICLE GENERATORS -- 

	// Render the particles as particles or as a solid (or a single plane!)
	if (isRenderingSinglePlane)
	{
		_render_particles("phong", "solid_plane");
	}
	else
	{
		if (drawParticles)
		{
			_render_particles("fluid", "particles");	
		}
		else
		{
			_render_particles("phong", "solid");
		}
	}
		

	// -- TEXT --

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowSizeX), 0.0f, static_cast<float>(windowSizeY));
	shader_data["font"]->use();
	glUniformMatrix4fv(glGetUniformLocation(shader_data["font"]->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	// Decide how text should be rendered (if rendered)
	if (isDebug)
	{
		if (isRenderingAsLines )
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			renderText();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			renderText();
		}
	}

}

int OpenGLCore::Run()
{
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // tell openGL to call framebuffer_size_callback on every resize
	
	_setup_objects();

	// Set particle array initially to left most
	m_currentParticleGenerator = m_Generators[currentGenerator]; 

	AssignPhongUniforms();

	// Initialising camera model matrix and setup of projection matrix
	model = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(70.0f), windowSizeX/windowSizeY, 0.1f, 100.0f); 
	
	// Setting up normal matrix for Phong shader
	glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(model)));
	int normalLoc = glGetUniformLocation(shader_data["phong"]->ID, "NormalMatrix");
	glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normMat));

	// Setup dt updation
	int currentFrame = 0;
	frameTimes.resize(framesToMonitor);
	std::fill(frameTimes.begin(), frameTimes.end(), 0); // Fill with 0s
	
	//-----------------------------------------------------------------------------------------

	std::chrono::high_resolution_clock::time_point frameEndTime;
	std::chrono::duration<double> frameDuration;

	// Check if GLFW has been instructed to close
	while(!glfwWindowShouldClose(window))
	{
		// START OF FRAME OPERATIONS

		std::chrono::high_resolution_clock::time_point frameStartTime = std::chrono::high_resolution_clock::now(); // Start of frame
		// Clear window
		glEnable(GL_DEPTH_TEST); 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(window); // Inputs
		view = m_camera.view(); // Update view based on camera

		update(frameTimes[currentFrame]);
		render();

		// END OF FRAME OPERATIONS

		glfwPollEvents(); // Checks if any events are triggered
		glfwSwapBuffers(window); // Swap the colour buffer

		frameEndTime = std::chrono::high_resolution_clock::now(); // End of frame
		frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(frameEndTime - frameStartTime); // Duration of frame

		// Update frame in the vector and change current frame
		frameTimes[currentFrame] = frameDuration.count(); 
		currentFrame = (currentFrame + 1) % framesToMonitor;
	}

	_cleanup_objects();

	glfwTerminate(); // Clean/delete all GLFW resources allocated
	return 0;

}
//=================================================================================================

/// @brief Binds each shader name to a new shader pointer based on the vertex and fragment shaders
///        with the same name
/// @param shader_names string names of each shader - actual name and how they are refered to in code

void OpenGLCore::_bind_shaders(std::vector<std::string> shader_names)
{
	std::string vertex_name;
	std::string frag_name;
	std::string current_shader_name;

	for (int i = 0; i < shader_names.size(); i++)
	{	
		current_shader_name = shader_names[i];
		vertex_name = "shaders/" + current_shader_name + ".vert";
		frag_name = "shaders/" + current_shader_name + ".frag";

		shader_data[current_shader_name] = new Shader(vertex_name, frag_name);
	}
	
}

//=================================================================================================

/// @brief render the particles for each particle generator based on the assigned shader 
///        and rendering name
/// @param shader_name The name of the shader to use for all particle systems
/// @param render_type Type of rendering to use for all particle systems

void OpenGLCore::_render_particles(const std::string shader_name, const std::string render_type)
{
	shader_data[shader_name]->use();
	currentShaderID = shader_data[shader_name]->ID;
	CameraUniformsToShader(currentShaderID);

	for (int i = 0; i < m_Generators.size(); i++)
	{
		glUniform4fv(glGetUniformLocation(currentShaderID, "colour"), 1, glm::value_ptr(m_Generators[i]->getColour()));
		
		if      (render_type == "solid_plane") { m_Generators[i]->renderSolidPlane(planeName); }
		else if (render_type == "particles")   { m_Generators[i]->renderParticles(); }
		else if (render_type == "solid")	   { m_Generators[i]->renderSolid(); }
	}
}

//===============================================================================================

/// @brief Destroys all current object pointers, this could be used when making a new scene

void OpenGLCore::_cleanup_objects()
{
	// Delete particle generators
	for (std::vector< ParticleGenerator* >::iterator it = m_Generators.begin(); it != m_Generators.end(); ++it)
	{
		delete (*it);
	}
	m_Generators.clear();

	// Delete all other pointer objects
	delete m_cube;
	delete m_plane;
	delete m_skybox;
	delete m_text;

	// Cleanup Shader Data
	for (auto it = shader_data.begin(); it != shader_data.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}
}

//===============================================================================================


void OpenGLCore::_setup_objects()
{
	// Cuboid
	glm::vec3 size = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 centre = glm::vec3(-1.0f, -0.75f, 2.0f);
	m_cube = new Cuboid(size, centre);
	m_cube->setColour(glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));

	// Baseplane
	size = glm::vec3(14.0f, 0.05f, 6.0f);
	centre = glm::vec3(5.0f, -1.0f, 0.0f);

	m_plane = new Cuboid(size, centre);
	m_plane->setColour(glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

	// Particle arrays
	m_Generators.push_back(new ParticleGenerator(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 3));
	m_Generators.push_back(new ParticleGenerator(glm::vec3(2.5f, 0.5f, 0.5f), glm::vec3(3.5f, 0.1f, 1.0f), 4));
	m_Generators.push_back(new ParticleGenerator(glm::vec3(4.5f, 0.0f, 0.0f), glm::vec3(6.5f, 2.0f, 2.0f), 5));
	m_Generators.push_back(new ParticleGenerator(glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(8.5f, 0.5f, 0.5f), 6));
	m_Generators.push_back(new ParticleGenerator(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(11.0f, 3.0f, 1.0f), 7));

	m_Generators[0]->setColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Transparent red
	m_Generators[1]->setColour(glm::vec4(1.0f, 0.65f, 0.0f, 0.8f)); // Orange
	m_Generators[2]->setColour(glm::vec4(0.0f, 1.0f, 0.0f, 0.6f)); // Green
	m_Generators[3]->setColour(glm::vec4(0.0f, 0.0f, 1.0f, 0.4f)); // Blue
	m_Generators[4]->setColour(glm::vec4(0.4f, 0.0f, 0.8f, 0.2f)); // Solid purple

	m_skybox = new Skybox(5.f); // Setup skybox size 5

	// text
	m_text = new Text;
	m_text->setupDefaultFont();
}
	