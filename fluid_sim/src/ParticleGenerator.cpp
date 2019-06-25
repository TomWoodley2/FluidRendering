#include "ParticleGenerator.h"
#include <map>
#include <deque>
#include <iostream>
#include <limits>

#include <ctime> // Used for random numbers

ParticleGenerator::ParticleGenerator()
{
	InitialSetup();
}



ParticleGenerator::ParticleGenerator(glm::vec3 pointStartIn, glm::vec3 pointEndIn, int rootIn)
{
	pointStart = pointStartIn;
	pointEnd = pointEndIn;
	root = rootIn;
	no_points = root*root*root;
	InitialSetup();
}

void ParticleGenerator::InitialSetup()
{
	m_particleMovement = new ParticleMovement(root);


	GenerateVertices(no_points,pointStart,pointEnd); // Generate the points of each particle based on the particle array position
	GenerateVector(); // Generate the vector of particles

	GenerateRandomParticleVelocities(); // Set each particle to have a random velocity
	//m_particles
	// Horribly inefficent at the minute, need to use an iterator
	
	
	for (int i = 0; i < m_Particles.size() ; i++)
	{
		m_locationVector.push_back(m_Particles.at(i).posX);
		m_locationVector.push_back(m_Particles.at(i).posY);
		m_locationVector.push_back(m_Particles.at(i).posZ);

	}

	//v_drawingIndices = new std::vector<unsigned int>; // Indices put in drawing order
	v_drawingIndices.reserve(root*root*6);
	//v_drawingIndices.reserve(root*root); // Changed for just one plane

	SetupVAOParticle();
	SetupVAOSolid();

}

ParticleGenerator::~ParticleGenerator()
{
	delete m_particleMovement;
}

void ParticleGenerator::update(float dt)
{
	// used to move all particles on program update

	// Removed while working on a mesh
	

	

	m_particleMovement->setParticleVector(&m_Particles);
	m_particleMovement->update(dt);
	
	

	int j = 0;
	for (int i = 0; i < m_Particles.size() ; i++)
	{
		j = i* 3;
		m_locationVector[j] = m_Particles.at(i).posX;
		m_locationVector[j+1] = m_Particles.at(i).posY;
		m_locationVector[j+2] = m_Particles.at(i).posZ;
	}
	
	// Need to clear v_drawingIndices before update
	v_drawingIndices.clear();
	v_drawingIndices.reserve(root*root * 6);

	faceNormals.clear();
	SetupSolidPlanesDynamic();

	
	finalNormals.clear(); // final normals which will be used for phong rendering
	calculateFinalNormals(); // Put here for now to test with just one plane 

	// UPDATE PARTICLES
	glBindVertexArray(VAOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, no_points * 3 * sizeof(float), &m_locationVector[0], GL_DYNAMIC_DRAW);
	glBindVertexArray(0);

	// UPDATE SOLID

	glBindVertexArray(VAOSolid);
	glBindBuffer(GL_ARRAY_BUFFER, VBOSolid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * no_points* 3 , &m_locationVector[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2Solid); // Update normals
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * no_points * 3, &finalNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOSolid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * v_drawingIndices.size(), &v_drawingIndices[0], GL_DYNAMIC_DRAW);


	
	
}


void ParticleGenerator::SetupVAOParticle()
{
	
	glGenBuffers(1, &VBOParticle); 

	// Tell openGL how to should interpret vertex data per vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // Enable attrib pointer

	glGenVertexArrays(1, &VAOParticle); // Generate VAO

	// VAO initialization code
	// 1 - bind VAO
	glBindVertexArray(VAOParticle);
	// 2 - copy vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBOParticle);
	// works through the vector from the first point for the size of the vector, moving a float each time. Also needs to be dynamicly drawn as particles will move
	glBufferData(GL_ARRAY_BUFFER, no_points * 3 * sizeof(float), &m_locationVector[0], GL_DYNAMIC_DRAW); // Use dynamic draw for these
	
	// 4 - Set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	glEnableVertexAttribArray(0);
}


void ParticleGenerator::SetupSolidIndices(std::vector<unsigned int> faceIndexValues)
{
	std::vector<unsigned int> v_planeIndices = faceIndexValues;

	std::vector<unsigned int> v_requiredIndices; // Vector with non-essential indices removed
	v_planeIndices.reserve(root * root); // Currently reserve for one plane
	v_requiredIndices.reserve((root-1 )* (root-1));

	
	// 2 - Order vertices based on corner indices
	// For rendering each plane, we only need the corner value of each plane

	for(unsigned int i = 0; i < ((root-1) * (root-1)) + root-2 ; i++)
	{
		if ((i+1) % root != 0)
		{
			v_requiredIndices.push_back(v_planeIndices[i]);
		}
	}

	// 3 - Generate drawing indices based on these corner indices

	for (unsigned int i = 0 ; i < v_requiredIndices.size() ; i++)
	{
		int current = 0;

		auto it = std::find(v_planeIndices.begin(), v_planeIndices.end(), v_requiredIndices[i]);
		if (it != v_planeIndices.end())
		{
			current = std::distance(v_planeIndices.begin(), it);
		}
		

		// Face normals for triangles on faces
		
		// First triangle
		glm::vec3 particle1Pos = glm::vec3(m_Particles[(v_planeIndices[current+root])].posX,m_Particles[(v_planeIndices[current+root])].posY,m_Particles[(v_planeIndices[current+root])].posZ);
		glm::vec3 particle2Pos = glm::vec3(m_Particles[(v_planeIndices[current])].posX,m_Particles[(v_planeIndices[current])].posY,m_Particles[(v_planeIndices[current])].posZ);
		glm::vec3 particle3Pos = glm::vec3(m_Particles[(v_planeIndices[current+root+1])].posX,m_Particles[(v_planeIndices[current+root+1])].posY,m_Particles[(v_planeIndices[current+root+1])].posZ);

		glm::vec3 vector1 = glm::vec3(particle2Pos - particle1Pos);
		glm::vec3 vector2 = glm::vec3(particle3Pos - particle2Pos);

		glm::vec3 triangle_normal_1 = glm::normalize(glm::cross(vector1, vector2));
		faceNormals.push_back(triangle_normal_1);


		// Second triangle
		particle1Pos = glm::vec3(m_Particles[(v_planeIndices[current+root+1])].posX,m_Particles[(v_planeIndices[current+root+1])].posY,m_Particles[(v_planeIndices[current+root+1])].posZ);
		particle2Pos = glm::vec3(m_Particles[(v_planeIndices[current])].posX,m_Particles[(v_planeIndices[current])].posY,m_Particles[(v_planeIndices[current])].posZ);
		particle3Pos = glm::vec3(m_Particles[(v_planeIndices[current+1])].posX,m_Particles[(v_planeIndices[current+1])].posY,m_Particles[(v_planeIndices[current+1])].posZ);

		vector1 = glm::vec3(particle2Pos - particle1Pos);
		vector2 = glm::vec3(particle3Pos - particle2Pos);

		glm::vec3 triangle_normal_2 = glm::normalize(glm::cross(vector1, vector2));
		faceNormals.push_back(triangle_normal_2);

		//int test = 3.33f / (int)3;


		// Setting up indices
		//unsigned int current = v_planeIndices[v_requiredIndices[i]];
		v_drawingIndices.push_back(v_planeIndices[current]);
		v_drawingIndices.push_back(v_planeIndices[current+root]);
		v_drawingIndices.push_back(v_planeIndices[current+root + 1]);
		v_drawingIndices.push_back(v_planeIndices[current]);
		v_drawingIndices.push_back(v_planeIndices[current+root + 1]);
		v_drawingIndices.push_back(v_planeIndices[current+1]);
	}

	v_planeIndices.clear();
	v_requiredIndices.clear();
}

void ParticleGenerator::SetupSolidPlanes()
{
	std::vector<unsigned int> v_planeIndices; // Store all indices on current plane

	// 1 - Get all vertices on the plane



	// FRONT PLANE
	for(int i = no_points-(root * root); i < no_points-1; i+= root)
	{
		for(int j = root ; j > 0; j--)
		{
			v_planeIndices.push_back(i + j -1);
		}
	}

	SetupSolidIndices(v_planeIndices);
	v_planeIndices.clear();

	
	// BACK PLANE
	// For back plane, these will start at 0 and be ordered up to root^2
	for(int i = 0; i < root * root ; i++)
	{
		v_planeIndices.push_back(i);
	}
	
	SetupSolidIndices(v_planeIndices);
	v_planeIndices.clear();
	
	// LEFT PLANE
	
	for (int i = 0; i < (root * root); i += root)
	{
		for (int j = (root-1)*(root * root) ; j > -(root*root); j += -(root*root))
		{
			v_planeIndices.push_back(i+j);
		}
	}
	
	SetupSolidIndices(v_planeIndices);
	v_planeIndices.clear();
	
	// RIGHT PLANE

	for (int i = 0; i < (root*root); i += root)
	{
		for (int j = 0; j < no_points  ;j+= (root*root))
		{
			v_planeIndices.push_back(i + j + root -1);
		}
	}

	SetupSolidIndices(v_planeIndices);
	v_planeIndices.clear();
	

	// TOP PLANE 

	for (int i = 0; i < no_points; i += (root*root))
	{
		for (int j = 0; j < root  ;j+= 1)
		{
			v_planeIndices.push_back(i + j + (root*root) - root);
		}
	}

	SetupSolidIndices(v_planeIndices);
	v_planeIndices.clear();

	// BOTTOM PLANE

	for (int i = root ; i > 0; i--)
	{
		for (int j = no_points-(root*root); j > -(root*root)  ;j+= -(root*root))
		{
			v_planeIndices.push_back(i + j - 1);
		}
	}

	SetupSolidIndices(v_planeIndices);
	v_planeIndices.clear();

}

std::vector<unsigned int> ParticleGenerator::SetupFrontPlane()
{
	// FRONT PLANE

	// - Initial particle sorted by greater
	// - Blank particles given minimum z position so all new particles more
	// - aParticle.posZ > it->first
	// Y sort by std::less
	// X sort by std::greater

	// Multimap used here as need duplicate keys
	std::multimap<float, Particle> frontMostParticles; // Stores the -x values along with each particle
	// Setup default values - have to use from particle array
	Particle blankParticle;
	for (int i = 0; i < root * root ; i++)
	{
		frontMostParticles.insert(std::pair<float,Particle>(std::numeric_limits<float>::min(),blankParticle));
	}
	
	// Getting the left plane particles
	// LOOK FOR CORNER PARTICLES HERE
	for (auto aParticle : m_Particles)
	{
		std::multimap<float,Particle>::iterator it;
		for (it = frontMostParticles.begin(); it != frontMostParticles.end(); it++)
		{
			if (aParticle.posZ >= it->first)
			{
				frontMostParticles.erase(it); // Erase based on iterator position
				frontMostParticles.insert(std::pair<float,Particle>(aParticle.posZ,aParticle));
				break;
			}
		}			
	}

	// Y Sort (by std::less)
	std::multimap<float, Particle> frontMostParticlesYSorted;
	std::multimap<float,Particle>::iterator it;
	for (it = frontMostParticles.begin(); it != frontMostParticles.end(); it++)
	{
		
		frontMostParticlesYSorted.insert(std::pair<float,Particle>(it->second.posY,it->second));
	}

	std::vector<Particle> frontMostParticlesSorted;
	std::multimap<float, Particle,std::greater<float>> frontMostParticlesXSorted;

	std::vector<unsigned int> v_planeIndices;

	// X sort (by std::greater)
	int j = 0;
	for (it = frontMostParticlesYSorted.begin(); it != frontMostParticlesYSorted.end(); it++)
	{
		frontMostParticlesXSorted.insert(std::pair<float,Particle>(it->second.posX,it->second));
		if ((j+1) % root == 0)
		{
			std::multimap<float,Particle>::iterator it2;
			for (it2 = frontMostParticlesXSorted.begin(); it2 != frontMostParticlesXSorted.end(); it2++)
			{
				//leftMostParticlesSorted.push_back(it->second);
				v_planeIndices.push_back(it2->second.index);
				
			}
			frontMostParticlesXSorted.clear();
		}
		
		j++;
	}

	return v_planeIndices;
}

std::vector<unsigned int> ParticleGenerator::SetupBackPlane()
{
	// BACK PLANE

	// - Initial particle sorted by less
	// - Blank particles given maximum z position so all new particles less
	// - aParticle.posZ < it->first
	// Y sort by std::less
	// X sort by std::less

	// Multimap used here as need duplicate keys
	std::multimap<float, Particle> backMostParticles; // Stores the -x values along with each particle
	// Setup default values - have to use from particle array
	Particle blankParticle;
	for (int i = 0; i < root * root ; i++)
	{
		backMostParticles.insert(std::pair<float,Particle>(std::numeric_limits<float>::max(),blankParticle));
	}
	
	// Getting the left plane particles
	for (auto aParticle : m_Particles)
	{
		std::multimap<float,Particle>::iterator it;
		for (it = backMostParticles.begin(); it != backMostParticles.end(); it++)
		{
			if (aParticle.posZ < it->first)
			{
				backMostParticles.erase(it); // Erase based on iterator position
				backMostParticles.insert(std::pair<float,Particle>(aParticle.posZ,aParticle));
				break;
			}
		}			
	}

	// Y Sort (by std::less)
	std::multimap<float, Particle> frontMostParticlesYSorted;
	std::multimap<float,Particle>::iterator it;
	for (it = backMostParticles.begin(); it != backMostParticles.end(); it++)
	{
		
		frontMostParticlesYSorted.insert(std::pair<float,Particle>(it->second.posY,it->second));
	}

	std::vector<Particle> frontMostParticlesSorted;
	std::multimap<float, Particle> frontMostParticlesXSorted;

	std::vector<unsigned int> v_planeIndices;

	// X sort (by std::greater)
	int j = 0;
	for (it = frontMostParticlesYSorted.begin(); it != frontMostParticlesYSorted.end(); it++)
	{
		frontMostParticlesXSorted.insert(std::pair<float,Particle>(it->second.posX,it->second));
		if ((j+1) % root == 0)
		{
			std::multimap<float,Particle>::iterator it2;
			for (it2 = frontMostParticlesXSorted.begin(); it2 != frontMostParticlesXSorted.end(); it2++)
			{
				//leftMostParticlesSorted.push_back(it->second);
				v_planeIndices.push_back(it2->second.index);
				
			}
			frontMostParticlesXSorted.clear();
		}
		
		j++;
	}

	return v_planeIndices;
}

std::vector<unsigned int> ParticleGenerator::SetupLeftPlane()
{
	// LEFT PLANE

	// - Initial particle sorted by greater
	// - Blank particles given maximum x position so all new particles less
	// - aParticle.posX < it->first
	// Y sort by std::less
	// Z sort by std::greater
	
	// Multimap used here as need duplicate keys
	std::multimap<float, Particle,std::greater<float>> leftMostParticles; // Stores the -x values along with each particle
	// Setup default values - have to use from particle array
	Particle blankParticle;
	for (int i = 0; i < root*root ; i++)
	{
		leftMostParticles.insert(std::pair<float,Particle>(std::numeric_limits<float>::max(),blankParticle));
	}
	
	// Getting the left plane particles
	for (auto aParticle : m_Particles)
	{
		std::multimap<float,Particle>::iterator it;
		for (it = leftMostParticles.begin(); it != leftMostParticles.end(); it++)
		{
			if (aParticle.posX < it->first)
			{
				leftMostParticles.erase(it); // Erase based on iterator position
				leftMostParticles.insert(std::pair<float,Particle>(aParticle.posX,aParticle));
				break;
			}
		}			
	}

	// Y Sort (by std::less)
	std::multimap<float, Particle> leftMostParticlesYSorted;
	std::multimap<float,Particle>::iterator it;
	for (it = leftMostParticles.begin(); it != leftMostParticles.end(); it++)
	{
		
		leftMostParticlesYSorted.insert(std::pair<float,Particle>(it->second.posY,it->second));
	}

	std::vector<Particle> leftMostParticlesSorted;
	std::multimap<float, Particle,std::greater<float>> leftMostParticlesZSorted;

	std::vector<unsigned int> v_planeIndices;

	// Z sort (by std::greater)
	int j = 0;
	for (it = leftMostParticlesYSorted.begin(); it != leftMostParticlesYSorted.end(); it++)
	{
		leftMostParticlesZSorted.insert(std::pair<float,Particle>(it->second.posZ,it->second));
		if ((j+1) % root == 0)
		{
			std::multimap<float,Particle>::iterator it2;
			for (it2 = leftMostParticlesZSorted.begin(); it2 != leftMostParticlesZSorted.end(); it2++)
			{
				//leftMostParticlesSorted.push_back(it->second);
				v_planeIndices.push_back(it2->second.index);
				
			}
			leftMostParticlesZSorted.clear();
		}
		
		j++;
	}

	return v_planeIndices;
}



std::vector<unsigned int> ParticleGenerator::SetupRightPlane()
{
	// RIGHT PLANE

	// - Initial particle sorted by less
	// - Blank particles given minimum x position so all new particles more
	// - aParticle.posX > it->first
	// Y sort by std::less
	// Z sort by std::less

	// Multimap used here as need duplicate keys
	std::multimap<float, Particle> rightMostParticles; // Stores the -x values along with each particle
	// Setup default values - have to use from particle array
	Particle blankParticle;
	for (int i = 0; i < root * root ; i++)
	{
		rightMostParticles.insert(std::pair<float,Particle>(std::numeric_limits<float>::min(),blankParticle));
	}
	
	// Getting the left plane particles
	for (auto aParticle : m_Particles)
	{
		std::multimap<float,Particle>::iterator it;
		for (it = rightMostParticles.begin(); it != rightMostParticles.end(); it++)
		{
			if (aParticle.posX > it->first)
			{
				rightMostParticles.erase(it); // Erase based on iterator position
				rightMostParticles.insert(std::pair<float,Particle>(aParticle.posX,aParticle));
				break;
			}
		}			
	}

	// Y Sort (by std::less)
	std::multimap<float, Particle> rightMostParticlesYSorted;
	std::multimap<float,Particle>::iterator it;
	for (it = rightMostParticles.begin(); it != rightMostParticles.end(); it++)
	{
		
		rightMostParticlesYSorted.insert(std::pair<float,Particle>(it->second.posY,it->second));
	}

	std::vector<Particle> rightMostParticlesSorted;
	std::multimap<float, Particle> rightMostParticlesZSorted;

	std::vector<unsigned int> v_planeIndices;

	// Z sort (by std::greater)
	int j = 0;
	for (it = rightMostParticlesYSorted.begin(); it != rightMostParticlesYSorted.end(); it++)
	{
		rightMostParticlesZSorted.insert(std::pair<float,Particle>(it->second.posZ,it->second));
		if ((j+1) % root == 0)
		{
			std::multimap<float,Particle>::iterator it2;
			for (it2 = rightMostParticlesZSorted.begin(); it2 != rightMostParticlesZSorted.end(); it2++)
			{
				//leftMostParticlesSorted.push_back(it->second);
				v_planeIndices.push_back(it2->second.index);
				
			}
			rightMostParticlesZSorted.clear();
		}
		
		j++;
	}

	return v_planeIndices;
}

std::vector<unsigned int> ParticleGenerator::SetupTopPlane()
{
	// TOP PLANE

	// - Initial particle  (Y) sorted by less
	// - Blank particles given minimum y position so all new particles more
	// - aParticle.posY > it->first
	// Z sort by std::less
	// X sort by std::less
	
	// Multimap used here as need duplicate keys
	std::multimap<float, Particle> topMostParticles; // Stores the -x values along with each particle
	// Setup default values - have to use from particle array
	Particle blankParticle;
	for (int i = 0; i < root*root ; i++)
	{
		topMostParticles.insert(std::pair<float,Particle>(std::numeric_limits<float>::min(),blankParticle));
	}
	
	// Getting the left plane particles
	for (auto aParticle : m_Particles)
	{
		std::multimap<float,Particle>::iterator it;
		for (it = topMostParticles.begin(); it != topMostParticles.end(); it++)
		{
			if (aParticle.posY > it->first)
			{
				topMostParticles.erase(it); // Erase based on iterator position
				topMostParticles.insert(std::pair<float,Particle>(aParticle.posY,aParticle));
				break;
			}
		}			
	}

	// Z Sort (by std::less)
	std::multimap<float, Particle> topMostParticlesZSorted;
	std::multimap<float,Particle>::iterator it;
	for (it = topMostParticles.begin(); it != topMostParticles.end(); it++)
	{
		
		topMostParticlesZSorted.insert(std::pair<float,Particle>(it->second.posZ,it->second));
	}

	std::vector<Particle> topMostParticlesSorted;
	std::multimap<float, Particle,std::greater<float>> topMostParticlesXSorted;

	std::vector<unsigned int> v_planeIndices;

	// X sort (by std::greater)
	int j = 0;
	for (it = topMostParticlesZSorted.begin(); it != topMostParticlesZSorted.end(); it++)
	{
		topMostParticlesXSorted.insert(std::pair<float,Particle>(it->second.posX,it->second));
		if ((j+1) % root == 0)
		{
			std::multimap<float,Particle>::iterator it2;
			for (it2 = topMostParticlesXSorted.begin(); it2 != topMostParticlesXSorted.end(); it2++)
			{
				//leftMostParticlesSorted.push_back(it->second);
				v_planeIndices.push_back(it2->second.index);
				
			}
			topMostParticlesXSorted.clear();
		}
		
		j++;
	}

	return v_planeIndices;
}


std::vector<unsigned int> ParticleGenerator::SetupBottomPlane()
{
	// Bottom Plane

	// - Initial particle  (Y) sorted by more
	// - Blank particles given maxium y position so all new particles less
	// - aParticle.posY < it->first
	// Z sort by std::less
	// X sort by std::greater
	
	// Multimap used here as need duplicate keys
	std::multimap<float, Particle,std::greater<float>> bottomMostParticles; // Stores the -x values along with each particle
	// Setup default values - have to use from particle array
	Particle blankParticle;
	for (int i = 0; i < root*root ; i++)
	{
		bottomMostParticles.insert(std::pair<float,Particle>(std::numeric_limits<float>::max(),blankParticle));
	}
	
	// Getting the left plane particles
	for (auto aParticle : m_Particles)
	{
		std::multimap<float,Particle>::iterator it;
		for (it = bottomMostParticles.begin(); it != bottomMostParticles.end(); it++)
		{
			if (aParticle.posY < it->first)
			{
				bottomMostParticles.erase(it); // Erase based on iterator position
				bottomMostParticles.insert(std::pair<float,Particle>(aParticle.posY,aParticle));
				break;
			}
		}			
	}

	// Z Sort (by std::less)
	std::multimap<float, Particle> bottomMostParticlesZSorted;
	std::multimap<float,Particle>::iterator it;
	for (it = bottomMostParticles.begin(); it != bottomMostParticles.end(); it++)
	{
		
		bottomMostParticlesZSorted.insert(std::pair<float,Particle>(it->second.posZ,it->second));
	}

	std::vector<Particle> bottomMostParticlesSorted;
	std::multimap<float, Particle,std::greater<float>> bottomMostParticlesXSorted;

	std::vector<unsigned int> v_planeIndices;

	// X sort (by std::greater)
	int j = 0;
	for (it = bottomMostParticlesZSorted.begin(); it != bottomMostParticlesZSorted.end(); it++)
	{
		bottomMostParticlesXSorted.insert(std::pair<float,Particle>(it->second.posX,it->second));
		if ((j+1) % root == 0)
		{
			std::multimap<float,Particle>::iterator it2;
			for (it2 = bottomMostParticlesXSorted.begin(); it2 != bottomMostParticlesXSorted.end(); it2++)
			{
				//leftMostParticlesSorted.push_back(it->second);
				v_planeIndices.push_back(it2->second.index);
				
			}
			bottomMostParticlesXSorted.clear();
		}
		
		j++;
	}

	return v_planeIndices;
}


void ParticleGenerator::SetupSolidPlanesDynamic()
{
	std::vector<unsigned int> v_planeIndices; // Store all indices on current plane

	// 1 - Get all vertices on the plane

	// FRONT PLANE
	SetupSolidIndices(SetupFrontPlane());
	v_planeIndices.clear();

	

	// Back plane
	SetupSolidIndices(SetupBackPlane());
	v_planeIndices.clear();
	
	// Left plane
	SetupSolidIndices(SetupLeftPlane());
	v_planeIndices.clear();
	
	// Right plane
	SetupSolidIndices(SetupRightPlane());
	v_planeIndices.clear();
	
	// TOP PLANE 
	SetupSolidIndices(SetupTopPlane());
	v_planeIndices.clear();

	// BOTTOM PLANE

	SetupSolidIndices(SetupBottomPlane());
	v_planeIndices.clear();
}


void ParticleGenerator::calculateFinalNormals()
{
	//faceNormals
	//v_drawingIndices

	
	//1 - for every index, search through v_drawingIndices and look for all occurences of that index
	for (unsigned int i = 0; i < no_points; i++)
	{
		std::vector<unsigned int> indexLocations; // Storing locations of indices
		//unsigned int current = 0;

		auto it = std::find(v_drawingIndices.begin(), v_drawingIndices.end(),i);
		
		//2 - Store locations of index in a vector
		while (it != v_drawingIndices.end())
		{
			indexLocations.push_back((std::distance(v_drawingIndices.begin(), it)/3));
			it = std::find(std::next(it), v_drawingIndices.end(), i);

		}

		std::vector<glm::vec3> indexNormals; // Stores the normals for the current index

		//3 - Get the corresponding normals based on location from faceNormals
		for (int j = 0; j < indexLocations.size(); j++)
		{
			indexNormals.push_back(faceNormals[indexLocations[j]]);
		}
		
		//4 - Calculate the vertex normal based on these normals and push to final normals
		// Add together and then normalize
		glm::vec3 finalVertexNormal = glm::vec3(0.0f,0.0f,0.0f);
		for (int j = 0; j < indexNormals.size(); j++)
		{
			finalVertexNormal = finalVertexNormal + indexNormals[j];

		}
		finalVertexNormal = glm::normalize(finalVertexNormal);

		// Push to final normal vector
		finalNormals.push_back(finalVertexNormal.x);
		finalNormals.push_back(finalVertexNormal.y);
		finalNormals.push_back(finalVertexNormal.z);
	}


	
	
	
}
	

void ParticleGenerator::SetupVAOSolid()
{
	SetupSolidPlanesDynamic();

	calculateFinalNormals(); // Put here for now to test with just one plane 
	
	
	glGenBuffers(1, &VBO2Solid);
	glGenBuffers(1, &EBOSolid); // Create EBO
	glGenBuffers(1, &VBOSolid);	
	glGenVertexArrays(1, &VAOSolid); // Generate VAO

	glBindVertexArray(VAOSolid);
	// 2 - copy vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBOSolid);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * no_points* 3 , &m_locationVector[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2Solid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * no_points * 3, &finalNormals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// 3 - copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOSolid);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * v_drawingIndices.size(), &v_drawingIndices[0], GL_DYNAMIC_DRAW);
	// 4 - Set our vertex attributes pointers
	
	
	glBindVertexArray(0);
	//delete v_drawingIndices;
}

void ParticleGenerator::GenerateVertices(int no_points, glm::vec3 pointStart,glm::vec3 pointEnd)
{
	// Generate the particle array based on the given values

	m_verticesVec.reserve(no_points);

	glm::vec3 gap = {pointEnd - pointStart};
	glm::vec3 increase = gap/(root-1.0f);
	for( int i = 0; i <  root; i++)
	{
		for( int j = 0; j <  root; j++)
		{
			for (int k = 0; k < root; k++)
			{
				m_verticesVec.push_back(glm::vec3(pointStart.x+(increase.x*k), pointStart.y+(increase.y*j), pointStart.z+(increase.z*i)));
			}
			
		}
	}


}

void ParticleGenerator::GenerateVector()
{
	for(int i = 0; i < no_points; i++)
	{
		Particle c_particle;
		c_particle.posX = m_verticesVec[i].x;
		c_particle.posY = m_verticesVec[i].y;
		c_particle.posZ = m_verticesVec[i].z;
		c_particle.index = i; // Store index of the particle

		m_Particles.push_back(c_particle);
	}
	//std::cout << "test" << std::endl;
}

// Assign each particle in m_particles a random velocity between two set values
void ParticleGenerator::GenerateRandomParticleVelocities() 
{
	std::uniform_int_distribution<> distr(-5, 5);
	std::random_device rd; // Obtain random number from hardware
	std::mt19937 eng(rd()); // Seed the generator

	for (int i = 0; i < m_Particles.size(); i++)
	{
		int random_number = distr(eng);
		glm::vec3 randomVector =  glm::vec3((float)distr(eng) / 1000, (float)distr(eng) / 1000, (float)distr(eng) / 1000);
		//std::cout << random_number << std::endl;
		m_Particles[i].velocity = randomVector;
	}

}

std::vector<Particle>& ParticleGenerator::GetParticles()
{
	return m_Particles;
}


void ParticleGenerator::renderParticles()
{
	glBindVertexArray(VAOParticle);
	glPointSize(10.f);
	glDrawArrays(GL_POINTS, 0, m_Particles.size()); // Draw all the particles
	glBindVertexArray(0);
	//glDrawElements(GL_TRIANGLES, 36*sizeof(unsigned int) ,GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, m_Particles.size());
}


void ParticleGenerator::renderSolid()
{
	
	glBindVertexArray(VAOSolid);
	glDrawElements(GL_TRIANGLES,v_drawingIndices.size() ,GL_UNSIGNED_INT, 0); // size of gets the number of points required for the number of triangles to be drawn
	glBindVertexArray(0);
	
}

void ParticleGenerator::renderSolidPlane(std::string planeIn) // Render a single plane of the solid
{
	int planeSize = 6 * (root - 1) * (root - 1);
	
	glBindVertexArray(VAOSolid);

	if (planeIn == "front")
	{
		glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, 0); // size of gets the number of points required for the number of triangles to be drawn
	}
	else if (planeIn == "back")
	{
		glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, (void*)(planeSize * sizeof(GLuint))); // size of gets the number of points required for the number of triangles to be drawn
	}
	else if (planeIn == "left")
	{
		glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, (void*)(2 *planeSize * sizeof(GLuint))); // size of gets the number of points required for the number of triangles to be drawn
	}
	else if (planeIn == "right")
	{
		glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, (void*)(3 *planeSize * sizeof(GLuint))); // size of gets the number of points required for the number of triangles to be drawn
	}
	else if (planeIn == "top")
	{
		glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, (void*)(4 *planeSize * sizeof(GLuint))); // size of gets the number of points required for the number of triangles to be drawn
	}
	else if (planeIn == "bottom")
	{
		glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, (void*)(5 *planeSize * sizeof(GLuint))); // size of gets the number of points required for the number of triangles to be drawn
	}
	
	glBindVertexArray(0);
}

int ParticleGenerator::getParticleCount() // Return the number of particles
{
	return no_points;
}

int ParticleGenerator::getParticleRoot()
{
	return root;
}

void ParticleGenerator::setParticleRoot(int rootIn)
{
	// Update size values
	root = rootIn;
	no_points = rootIn * rootIn * rootIn;

	m_particleMovement->setNoPointsRoot(rootIn); // Update the number of roots in particle movement

	// Regenerate vertices
	m_verticesVec.clear();
	GenerateVertices(no_points,pointStart,pointEnd);

	// Regenerate m_Particles
	m_Particles.clear();
	GenerateVector(); // Recreate

	v_drawingIndices.clear();
	v_drawingIndices.reserve(rootIn*rootIn*6);

	SetupSolidPlanesDynamic();

	

	m_locationVector.clear();
	m_locationVector.reserve(m_Particles.size() * 3);
	

	// Regenerate m_locationVector
	for (int i = 0; i < m_Particles.size() ; i++)
	{
		m_locationVector.push_back(m_Particles.at(i).posX);
		m_locationVector.push_back(m_Particles.at(i).posY);
		m_locationVector.push_back(m_Particles.at(i).posZ);

	}


	glBindVertexArray(VAOSolid);
	glBindBuffer(GL_ARRAY_BUFFER, VBOSolid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * no_points* 3 , &m_locationVector[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOSolid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * rootIn*rootIn*6, &v_drawingIndices[0], GL_DYNAMIC_DRAW);
	

	glBindVertexArray(VAOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, no_points * 3 * sizeof(float), &m_locationVector[0], GL_DYNAMIC_DRAW);
}

glm::vec4 ParticleGenerator::getColour()
{
	return colour;

}
void ParticleGenerator::setColour(glm::vec4 colourIn)
{
	colour = colourIn;
}