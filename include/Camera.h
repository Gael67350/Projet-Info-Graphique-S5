#ifndef  CAMERA_INC
#define  CAMERA_INC

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 verticalAxis, float aspectRatio = 1920 / 1080.f, float m_fov = 45.f);

	Camera(float aspectRatio = 1920 / 1080.f, float m_fov = 45.f);

	glm::mat4 lookAt() const;

	glm::vec3 getPosition() const;

	glm::vec3 getTarget() const;

	void setPosition(glm::vec3 position);

	void setTarget(glm::vec3 target);

	void setAspectRatio(float aspectRatio);

	void setFov(float fov);

private:
	glm::vec3 m_position;
	glm::vec3 m_target;
	glm::vec3 m_verticalAxis;

	float m_aspectRatio;
	float m_fov;
};

#endif
