#ifndef  ENV_INC
#define  ENV_INC

#include <vector>
#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include <Shader.h>
#include <Camera.h>

class Environement {
public:
	Environement();

	~Environement();

	bool loadShaders();

	void loadTextures();

	void initLight(glm::vec3 lightPosition, glm::vec3 lightColor, float ambientStrength, float diffuseStrength);

	bool draw(Camera &camera, glm::vec3 const &position = { 0,0,0 }, float const &scaling = 1.f);

	const std::vector<float> getVertices() const { return m_vertices; };

	const std::vector<float> getNormals() const { return m_normals; };

	const std::vector<float> getUVs() const { return m_uvs; };

	uint32_t getNbVertices() const { return m_vertices.size() / 3; }

private:
	void initTexturedShaderData();
	void initLightData(Camera const &camera);
	void initModelViewMatrixData(glm::mat4 const & model, Camera const &camera);
	void toggleLight();

	std::vector<float> m_vertices;
	std::vector<float> m_normals;
	std::vector<float> m_uvs;

	GLuint m_buffer;

	Shader* m_texturedShader;
	std::vector<SDL_Surface*> m_textures;
	std::vector<GLuint> m_texturesIDs;

	bool m_isInitLight = false;
	bool m_lightState = false;
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;
	glm::vec4 m_materials; // Ambient strength, Diffuse strength, Specular strength, Shininess
};

#endif
