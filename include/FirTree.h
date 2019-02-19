#ifndef  FIR_TREE_INC
#define  FIR_TREE_INC

#include <vector>
#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include <Shader.h>
#include <Camera.h>

class FirTree {
public:
	static const float WIND_MIN_ANGLE;
	static const float WIND_MAX_ANGLE;
	static const float WIND_SPEED;
	static const float WIND_SPEED_RETURN;

	FirTree(uint32_t nbLatitude = 50);

	~FirTree();

	bool loadShaders();

	void loadTextures();

	void initLight(glm::vec3 lightPosition, glm::vec3 lightColor, float lightIntensity);

	bool draw(Camera &camera, glm::vec3 const &position = { 0,0,0 }, float const &scaling = 1.f, float const &windAngle = 0);

	const float* getVertices() const;

	const float* getNormals() const;

	const float* getColors() const;

	const float* getUVs() const;

	uint32_t getNbVertices() const { return (m_trunkVertices.size() + m_leavesVertices.size()) / 3; }

	uint32_t getNbTrunkVertices() const { return m_trunkVertices.size() / 3; }

	uint32_t getNbLeavesVertices() const { return m_leavesVertices.size() / 3; }

private:
	void initColorizedShaderData();
	void initTexturedShaderData();

	void initLightData(Shader* const &shader, Camera const &camera);
	void initModelViewMatrixData(Shader* const &shader, glm::mat4 const & model, Camera const &camera);

	std::vector<float> m_trunkVertices;
	std::vector<float> m_trunkNormals;
	std::vector<float> m_trunkColors;
	std::vector<float> m_trunkUVs;

	std::vector<float> m_leavesVertices;
	std::vector<float> m_leavesNormals;
	std::vector<float> m_leavesColors;
	std::vector<float> m_leavesUVs;

	GLuint m_buffer;

	std::vector<Shader*> m_shaders;
	std::vector<SDL_Surface*> m_textures;
	std::vector<GLuint> m_texturesIDs;

	bool m_isInitLight = false;
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColor;
	float m_lightIntensity;
	glm::vec4 m_materials; // Ambient strength, Diffuse strength, Specular strength, Shininess
};

#endif
