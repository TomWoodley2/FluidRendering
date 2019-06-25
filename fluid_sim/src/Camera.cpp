#include "Camera.h"
#include <iostream>

Camera::Camera()
{
	m_pos = glm::vec3(4.0f, 0.0f, 8.0f);
	//m_orient = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	m_orientationYaw = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	m_orientationPitch = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

glm::vec3 Camera::getPosition()
{
	return m_pos;
}

glm::quat Camera::getOrientation()
{
	return m_orientationYaw * m_orientationPitch;
}

glm::mat4 Camera::view()
{
	//glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), m_pos);
	return glm::translate(glm::mat4_cast(inverse(getOrientation())),-m_pos);

	//return transMatrix * rotMatrix;
}

void Camera::translate(float x, float y, float z)
{
	m_pos += (getOrientation() *glm::vec3(x, y, z));
}

void Camera::yaw(float angle)
{
	m_orientationYaw *= glm::angleAxis(angle, glm::vec3(0, 1, 0));
}
void Camera::pitch(float angle)
{
	m_orientationPitch *= glm::angleAxis(angle, glm::vec3(1, 0, 0));
}

void Camera::rotate(float angle, float x, float y, float z)
{
	m_orient *= glm::angleAxis(angle, glm::vec3(x, y, z) * getOrientation());
}
