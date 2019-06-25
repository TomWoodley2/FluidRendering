#include "ParticleMovement.h"


ParticleMovement::ParticleMovement(int no_points_rootIn)
{
	no_points_root = no_points_rootIn; // Update points if set in constructor
	no_points = no_points_rootIn * no_points_rootIn * no_points_rootIn;

	midPoint = glm::vec3(pointStart.x + (pointEnd.x-pointStart.x)/2, pointStart.y + (pointEnd.y - pointStart.y) / 2, pointStart.z + (pointEnd.y - pointStart.y) / 2 );
}

void ParticleMovement::setParticleVector(std::vector<Particle> *m_ParticlesIn)
{
	m_Particles = m_ParticlesIn;
}

void ParticleMovement::update(float dt)
{
	
	/*
	for (int i = 0; i< no_points; i++)
	{
		//if (no_points % no_points_root == 0)
		
		
		if (m_Particles->at(i).posY >= 0)
			//m_Particles[i].posY -= ((i % no_points_root)+1) / 1000.f;
			m_Particles->at(i).posY -= dt/2;
		else
			m_Particles->at(i).posX -= dt/2;
	}
	
	
	

	for (int i = 0; i < no_points_root; i++)
	{
		MoveParticle(glm::vec3(i,i,i),glm::vec3(dt/2,0,0));
	}
	*/

	// Moving based on particle velocity
	for (int i = 0; i < no_points_root; i++)
	{
		for (int j = 0; j < no_points_root; j++)
		{
			for (int k = 0; k < no_points_root ; k++)
			{
				MoveParticle(glm::vec3(i, j, k),GetParticle(i,j,k).velocity);
			}
		}
	}
	
	// Expanding cube update - for inner cube
	
	/*
	for (int i = no_points_root - 3; i < no_points_root-1; i++)
	{
		for (int j = no_points_root - 3; j < no_points_root-1; j++)
		{
			for (int k = no_points_root - 3; k < no_points_root-1; k++)
			{
				int xDirection = 1;
				int yDirection = 1;
				int zDirection = 1;

				if (GetParticlePositon(i, j, k).x < midPoint.x)
				{
					xDirection = -1;
				}
				if (GetParticlePositon(i, j, k).y < midPoint.y)
				{
					yDirection = -1;
				}
				if (GetParticlePositon(i, j, k).z < midPoint.z)
				{
					zDirection = -1;
				}
				
				float xSpeed = dt/2 * (GetParticlePositon(i, j, k).x - midPoint.x);
				float ySpeed = dt/2 * (GetParticlePositon(i, j, k).y - midPoint.y);
				float zSpeed = dt/2 * (GetParticlePositon(i, j, k).z - midPoint.z);
				//float zSpeed = zDirection * dt / 2;
				MoveParticle(glm::vec3(i, j, k), glm::vec3(xSpeed,ySpeed,zSpeed));
					
			}
		}
	}
	*/
	
	
	//MoveParticle(glm::vec3(1,1,1),glm::vec3(dt/2, dt / 2,0)); // Move to top right corner - causes issues
	//MoveParticle(glm::vec3(1,2,2),glm::vec3(0, dt / 2,0)); // Move a single middle particle edge particle
	//MoveParticle(glm::vec3(1,2,1),glm::vec3(-dt/2,0,0)
	
	
}

void ParticleMovement::MoveParticle(glm::vec3 arrayPos, glm::vec3 movementValue)
{
	int x_index = arrayPos.x;
	int y_index = arrayPos.y * no_points_root;
	int z_index = arrayPos.z* no_points_root * no_points_root;
	int p_a_pos = x_index + y_index + z_index;

	// might be able to use interation based on references here for increased efficiency

	
	Particle *currentParticle = &(m_Particles->at(p_a_pos));
	currentParticle->posX += movementValue.x;
	currentParticle->posY += movementValue.y;
	currentParticle->posZ += movementValue.z;
	
}

glm::vec3 ParticleMovement::GetParticlePositon(int xArrayPos, int yArrayPos, int zArrayPos)
{
	int x_index = xArrayPos;
	int y_index = yArrayPos * no_points_root;
	int z_index = zArrayPos * no_points_root * no_points_root;
	int p_a_pos = x_index + y_index + z_index;
	Particle *currentParticle = &(m_Particles->at(p_a_pos));

	return glm::vec3(currentParticle->posX, currentParticle->posY, currentParticle->posZ);
}

void ParticleMovement::setNoPointsRoot(int noPointsRootIn)
{
	no_points_root = noPointsRootIn;
	no_points = noPointsRootIn * noPointsRootIn * noPointsRootIn;
}

Particle& ParticleMovement::GetParticle(int xArrayPos, int yArrayPos, int zArrayPos)
{
	int x_index = xArrayPos;
	int y_index = yArrayPos * no_points_root;
	int z_index = zArrayPos * no_points_root * no_points_root;
	int p_a_pos = x_index + y_index + z_index;
	//Particle *currentParticle = &(m_Particles->at(p_a_pos));
	return m_Particles->at(p_a_pos);
}