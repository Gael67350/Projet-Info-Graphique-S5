#include "FireTree.h"

#include <cmath>
#include <algorithm>
#include <stack>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cylinder.h>
#include <Cone.h>

FireTree::FireTree(uint32_t nbLatitude) {
	Cylinder trunk = Cylinder(nbLatitude);
	Cone leaves = Cone(nbLatitude, 0);

	m_nbVertices = trunk.getNbVertices() + leaves.getNbVertices();
	m_nbTrunkVertices = trunk.getNbVertices();
	m_nbLeavesVertices = leaves.getNbVertices();

	m_vertices.resize(3 * m_nbVertices);
	m_normals.resize(3 * m_nbVertices);
	m_colors.resize(3 * m_nbVertices);
	m_uvs.resize(2 * m_nbVertices);

	// Copy vertices
	std::copy(trunk.getVertices(), trunk.getVertices() + (3 * trunk.getNbVertices()), m_vertices.begin());
	std::copy(leaves.getVertices(), leaves.getVertices() + (3 * leaves.getNbVertices()), m_vertices.begin() + (3 * trunk.getNbVertices()));

	// Copy normals
	std::copy(trunk.getNormals(), trunk.getNormals() + (3 * trunk.getNbVertices()), m_normals.begin());
	std::copy(leaves.getNormals(), leaves.getNormals() + (3 * leaves.getNbVertices()), m_normals.begin() + (3 * trunk.getNbVertices()));

	// Copy uvs
	std::copy(trunk.getUVs(), trunk.getUVs() + (2 * trunk.getNbVertices()), m_uvs.begin());
	std::copy(leaves.getUVs(), leaves.getUVs() + (2 * leaves.getNbVertices()), m_uvs.begin() + (2 * trunk.getNbVertices()));

	// Fill colors
	glm::vec3 trunkColor(0.270f, 0.180f, .004f);	// Brown
	glm::vec3 leavesColor(0.078f, 0.353f, 0.196f);	// Dark green

	for (uint32_t i = 0; i < (3 * m_nbTrunkVertices); i += 3) {
		m_colors[i] = trunkColor.r;
		m_colors[i + 1] = trunkColor.g;
		m_colors[i + 2] = trunkColor.b;
	}

	for (uint32_t i = (3 * m_nbTrunkVertices); i < (3 * (m_nbTrunkVertices + m_nbLeavesVertices)); i += 3) {
		m_colors[i] = leavesColor.r;
		m_colors[i + 1] = leavesColor.g;
		m_colors[i + 2] = leavesColor.b;
	}
}

bool FireTree::draw(Shader* shader, Camera &camera, glm::vec3 const &position) const {
	float angleRad = 90 * (M_PI / 180.f);

	glm::mat4 id(1.f);

	// Trunk transformations
	glm::mat4 trunkRotation = glm::rotate(id, -angleRad, glm::vec3(1.f, 0, 0));
	glm::mat4 trunkScaling = glm::scale(trunkRotation, glm::vec3(0.75f, 0.75f, 4.f));
	glm::mat4 trunkModel = glm::translate(trunkScaling, glm::vec3(-position.x, -position.y, -position.z));

	// Leaves transformations
	glm::mat4 leavesScaling = glm::scale(id, glm::vec3(3.75f, 3.75f, 0.75f));

	glm::mat4 leavesBaseRangeModel = glm::translate(leavesScaling, glm::vec3(0, 0, 0.2f));
	glm::mat4 leavesUpRangeModel = glm::translate(id, glm::vec3(0, 0, 0.33f)) * glm::scale(id, glm::vec3(0.89f, 1.f, 0.89f));

	// Build fire tree scene graph
	std::stack<glm::mat4> matrices;
	matrices.push(camera.lookAt());

	matrices.push(matrices.top() * trunkModel);

	// Draw trunk
	GLint uMVP;

	uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(matrices.top()));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, getNbTrunkVertices());

	matrices.push(matrices.top() * leavesBaseRangeModel);

	// Draw 1st range of leaves
	uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(matrices.top()));
	glDrawArrays(GL_TRIANGLE_STRIP, getNbTrunkVertices(), getNbLeavesVertices());

	matrices.push(matrices.top() * leavesUpRangeModel);

	// Draw 2nd range of leaves
	uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(matrices.top()));
	glDrawArrays(GL_TRIANGLE_STRIP, getNbTrunkVertices(), getNbLeavesVertices());

	matrices.push(matrices.top() * leavesUpRangeModel);

	// Draw 3rd range of leaves
	uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(matrices.top()));
	glDrawArrays(GL_TRIANGLE_STRIP, getNbTrunkVertices(), getNbLeavesVertices());

	matrices.pop();

	if (!matrices.empty()) {
		return false;
	}

	return true;
}