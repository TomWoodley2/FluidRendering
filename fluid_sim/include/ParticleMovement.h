#pragma once

#include <vector>
#include "Particle.h"
#include "glm/glm.hpp"


class ParticleMovement
{
private:
	std::vector<Particle> * m_Particles;
	int no_points_root = 10; // Default value
	int no_points = no_points_root * no_points_root * no_points_root;

	// Need to get these from ParticleGenerator later
	const glm::vec3 pointStart = { 0.5f,0.f,0.f }; // Starting point of particle array
	const glm::vec3 pointEnd = { 1.5f,1.f,1.f }; // End point of particle array
	glm::vec3 midPoint; // defined in constructor

	glm::vec3 GetParticlePositon(int xArrayPos, int yArrayPos, int zArrayPos); // return the position of a particle
	Particle& GetParticle(int xArrayPos, int yArrayPos, int zArrayPos);

public:
	ParticleMovement(int no_points_rootIn);
	void setParticleVector(std::vector<Particle> *m_ParticlesIn);
	void update(float dt);
	void MoveParticle(glm::vec3 arrayPos, glm::vec3 movementValue);
	void setNoPointsRoot(int noPointsRootIn);
	
};