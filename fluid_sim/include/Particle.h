#pragma once

#include "glm/vec3.hpp"

struct Particle
{

//glm::vec3 colour = glm::vec3(0.0f, 1.0f, 0.0f);
float posX = 0.0f;
float posY = 0.0f;
float posZ = 0.0f;
glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f); // velocity of particle
int index = 0;

};

