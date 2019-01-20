#ifndef  FIRE_TREE_INC
#define  FIRE_TREE_INC

#include <vector>
#include <glm/glm.hpp>
#include <Shader.h>
#include <Camera.h>

class FireTree {
public:
	FireTree(uint32_t nbLatitude = 50);

	bool draw(Shader* shader, Camera &camera, glm::vec3 const &position = {0,0,0}, float const &scaling = 1.f) const;

	const float* getVertices() const { return &m_vertices[0]; }

	const float* getNormals() const { return &m_normals[0]; }

	const float* getColors() const { return &m_colors[0]; }

	const float* getUVs() const { return &m_uvs[0]; }

	uint32_t getNbVertices() const { return m_nbVertices; }

	uint32_t getNbTrunkVertices() const { return m_nbTrunkVertices; }

	uint32_t getNbLeavesVertices() const { return m_nbLeavesVertices; }

private:
	uint32_t m_nbVertices = 0;
	uint32_t m_nbTrunkVertices = 0;
	uint32_t m_nbLeavesVertices = 0;

	std::vector<float> m_vertices;
	std::vector<float> m_normals;
	std::vector<float> m_colors;
	std::vector<float> m_uvs;
};

#endif
