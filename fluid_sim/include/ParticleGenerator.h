#pragma once


#include <GLAD/glad.h> 
#include <GLFW/glfw3.h>

#include <vector>
#include <unordered_set>
#include <array>
#include "Particle.h"
#include "ParticleMovement.h"

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <random>
#include <iostream>
#include <string>

class ParticleGenerator
{
private:
	// Generating particles in map of set amount
	// Square value e.g. 2 would give 2^2 particles
	// define square and bounds

	void InitialSetup(); // Initial construction setup

	glm::vec4 colour = glm::vec4(1.0f,0.0f,0.0f,1.0f); // Colour of the particle array

	ParticleMovement *m_particleMovement;
	
	std::vector<glm::vec3> m_verticesVec;
	std::vector<Particle> m_Particles; // Vector to store all particles
	
	std::vector<float> m_locationVector; // Stores all the locations of the particles

	int root = 3;// Number of particles on a plane
    int no_points = root * root  * root;
	glm::vec3 pointStart = {0.5f,0.f,0.f}; // Starting point of particle array
	glm::vec3 pointEnd = {1.5f,1.f,1.f}; // End point of particle array
	
	void GenerateVertices(int no_points,glm::vec3 pointStart,glm::vec3 pointEnd);
	void GenerateRandomParticleVelocities(); // Assign each particle in m_particles a random velocity between two set values

	void MoveParticle(glm::vec3 arrayPos, glm::vec3 movementValue);

	unsigned int VAOSolid; 
	unsigned int VAOParticle;
	void SetupVAOParticle();
	void SetupVAOSolid();

	void SetupSolidIndices(std::vector<unsigned int> faceIndexValues); // Setup the indices for a set plane of a solid
	void SetupSolidPlanes(); // For static cube
	void SetupSolidPlanesDynamic();

	//std::unordered_set<unsigned int> v_planeIndices; // Store all indices on current plane
	//std::vector<unsigned int> v_requiredIndices; // Vector with non-essential indices removed
	std::vector<unsigned int> v_drawingIndices; // Indices put in drawing order

	unsigned int VBOSolid; // Vertex Buffer Object - can store a large number of vertices in GPU memory
	unsigned int VBO2Solid;
	unsigned int VBOParticle;

	// Element Buffer Object (EBO) - stores indices that openGL uses to decide what vertices to draw
	unsigned int EBOSolid;

	// Setting up the different planes for dynamic movement
	std::vector<unsigned int> SetupLeftPlane(); 
	std::vector<unsigned int> SetupRightPlane();
	std::vector<unsigned int> SetupTopPlane();
	std::vector<unsigned int> SetupBottomPlane();
	std::vector<unsigned int> SetupFrontPlane();
	std::vector<unsigned int> SetupBackPlane();

	std::vector<glm::vec3> faceNormals; // For storing the face normals of the front plane
	//std::vector<int> faceRelatingIndices; // Indices of the faces that form each normal - will be divided by 3 to get which normal each value relates to

	void calculateFinalNormals(); // Calculate the final normals and store in a vector
	std::vector<float> finalNormals; // final normals which will be used for phong rendering
	
	
public:
	ParticleGenerator(); // Default constructor
	ParticleGenerator(glm::vec3 pointStartIn, glm::vec3 pointEndIn, int rootIn); // Constructor which takes setup params
	
	~ParticleGenerator();
	void update(float dt);
	void GenerateVector();
	std::vector<Particle>& GetParticles();
	void renderSolid();
	void renderParticles();
	int getParticleCount(); // Return the number of particles
	void setParticleRoot(int rootIn); // Set and update the particle count - may need some polishing, takes root amount of points
	int getParticleRoot(); // Return root num of particles
	void renderSolidPlane(std::string planeIn); // Render a single plane of the solid

	// Get and set the colour of the particle array
	glm::vec4 getColour();
	void setColour(glm::vec4 colourIn);

};