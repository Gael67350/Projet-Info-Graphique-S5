#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 verticalAxis, float aspectRatio, float fov) : m_position{ position }, m_target{ target },
m_verticalAxis{ verticalAxis }, m_aspectRatio{ aspectRatio }, m_fov{ fov } {}

Camera::Camera(float aspectRatio, float fov) : m_position(0, 0, 0), m_target(0, 0, 0), m_verticalAxis(0, 1.f, 0), m_aspectRatio(aspectRatio), m_fov(fov) {}

glm::mat4 Camera::lookAt() {
	glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(m_position, m_target, m_verticalAxis);

	return projection * view;
}

glm::vec3 Camera::getPosition() const {
	return m_position;
}

glm::vec3 Camera::getTarget() const {
	return m_target;
}

void Camera::setPosition(glm::vec3 position) {
	m_position = position;
}

void Camera::setTarget(glm::vec3 target) {
	m_target = target;
}

void Camera::setAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;
}

void Camera::setFov(float fov) {
	m_fov = fov;
}