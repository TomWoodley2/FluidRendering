#pragma once

#include <GLAD/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
private:
	glm::vec3 m_pos;
	glm::quat m_orient;
	glm::quat m_orientationYaw;
	glm::quat m_orientationPitch;
public:
	Camera();
	glm::vec3 getPosition();
	glm::quat getOrientation();
	glm::mat4 view();

	void translate(float x, float y, float z);
	void rotate(float angle, float x, float y, float z);
	void yaw(float angle);
	void pitch(float angle);
	


};