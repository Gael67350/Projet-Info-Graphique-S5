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

	bool draw(Camera &camera, glm::vec3 const &position = { 0,0,0 }, float const &scaling = 1.f);

	const std::vector<float> getVertices() const { return m_vertices; };

	const std::vector<float> getNormals() const { return m_normals; };

	const std::vector<float> getUVs() const { return m_uvs; };

	uint32_t getNbVertices() const { return m_vertices.size() / 3; }

private:
	void initTexturedShaderData();

	std::vector<float> m_vertices;
	std::vector<float> m_normals;
	std::vector<float> m_uvs;

	GLuint m_buffer;

	Shader* m_texturedShader;
	std::vector<SDL_Surface*> m_textures;
	std::vector<GLuint> m_texturesIDs;
};

#endif
