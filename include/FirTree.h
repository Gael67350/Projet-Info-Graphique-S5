#ifndef  FIR_TREE_INC
#define  FIR_TREE_INC

#include <vector>
#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include <Shader.h>
#include <Camera.h>

class FirTree {
public:
	FirTree(uint32_t nbLatitude = 50);

	~FirTree();

	bool loadShaders();

	void loadTextures();

	bool draw(Camera &camera, glm::vec3 const &position = { 0,0,0 }, float const &scaling = 1.f);

	const float* getVertices() const;

	const float* getNormals() const;

	const float* getColors() const;

	const float* getUVs() const;

	uint32_t getNbVertices() const { return (m_trunkVertices.size() + m_leavesVertices.size()) / 3; }

	uint32_t getNbTrunkVertices() const { return m_trunkVertices.size() / 3; }

	uint32_t getNbLeavesVertices() const { return m_leavesVertices.size() / 3; }

private:
	static bool isInit;

	void initShadersData();

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
};

#endif
