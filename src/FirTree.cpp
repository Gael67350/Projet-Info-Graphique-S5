#include "FirTree.h"

#include <cmath>
#include <algorithm>
#include <stack>
#include <iterator>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cylinder.h>
#include <Cone.h>

const float FirTree::WIND_MIN_ANGLE = 0;
const float FirTree::WIND_MAX_ANGLE = 2.25 * (M_PI / 180.f);
const float FirTree::WIND_SPEED = 0.018f;
const float FirTree::WIND_SPEED_RETURN = 0.014f;

FirTree::FirTree(uint32_t nbLatitude) {
	Cylinder trunk = Cylinder(nbLatitude);
	Cone leaves = Cone(nbLatitude, 0);

	m_trunkVertices.assign(trunk.getVertices(), trunk.getVertices() + (3 * trunk.getNbVertices()));
	m_leavesVertices.assign(leaves.getVertices(), leaves.getVertices() + (3 * leaves.getNbVertices()));

	m_trunkNormals.assign(trunk.getNormals(), trunk.getNormals() + (3 * trunk.getNbVertices()));
	m_leavesNormals.assign(leaves.getNormals(), leaves.getNormals() + (3 * leaves.getNbVertices()));

	m_trunkUVs.assign(trunk.getUVs(), trunk.getUVs() + (2 * trunk.getNbVertices()));
	m_leavesUVs.assign(leaves.getUVs(), leaves.getUVs() + (2 * leaves.getNbVertices()));

	// Fill colors
	m_trunkColors.resize(3 * trunk.getNbVertices());
	m_leavesColors.resize(3 * leaves.getNbVertices());

	glm::vec3 trunkColor(0.875f, 0.592f, .027f);	// Brown
	glm::vec3 leavesColor(0.078f, 0.353f, 0.196f);	// Dark green

	for (uint32_t i = 0; i < m_trunkColors.size(); i += 3) {
		m_trunkColors[i] = trunkColor.r;
		m_trunkColors[i + 1] = trunkColor.g;
		m_trunkColors[i + 2] = trunkColor.b;
	}

	for (uint32_t i = 0; i < m_leavesColors.size(); i += 3) {
		m_leavesColors[i] = leavesColor.r;
		m_leavesColors[i + 1] = leavesColor.g;
		m_leavesColors[i + 2] = leavesColor.b;
	}

	// Put all fir tree parts in the VBO
	GLuint firTreeBuffer;
	glGenBuffers(1, &firTreeBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, firTreeBuffer);

	glBufferData(GL_ARRAY_BUFFER, (3 + 3 + 2 + 3) * sizeof(float) * getNbVertices(), nullptr, GL_DYNAMIC_DRAW);

	int offset = 0;
	// Vertices
	glBufferSubData(GL_ARRAY_BUFFER, offset, 3 * getNbTrunkVertices() * sizeof(float), &m_trunkVertices[0]);
	offset += 3 * getNbTrunkVertices() * sizeof(float);
	glBufferSubData(GL_ARRAY_BUFFER, offset, 3 * getNbLeavesVertices() * sizeof(float), &m_leavesVertices[0]);
	offset += 3 * getNbLeavesVertices() * sizeof(float);

	// Colors
	glBufferSubData(GL_ARRAY_BUFFER, offset, 3 * getNbTrunkVertices() * sizeof(float), &m_trunkColors[0]);
	offset += 3 * getNbTrunkVertices() * sizeof(float);
	glBufferSubData(GL_ARRAY_BUFFER, offset, 3 * getNbLeavesVertices() * sizeof(float), &m_leavesColors[0]);
	offset += 3 * getNbLeavesVertices() * sizeof(float);

	// UVs
	glBufferSubData(GL_ARRAY_BUFFER, offset, 2 * getNbTrunkVertices() * sizeof(float), &m_trunkUVs[0]);
	offset += 2 * getNbTrunkVertices() * sizeof(float);
	glBufferSubData(GL_ARRAY_BUFFER, offset, 2 * getNbLeavesVertices() * sizeof(float), &m_leavesUVs[0]);
	offset += 2 * getNbLeavesVertices() * sizeof(float);

	// Normals
	glBufferSubData(GL_ARRAY_BUFFER, offset, 3 * getNbTrunkVertices() * sizeof(float), &m_trunkNormals[0]);
	offset += 3 * getNbTrunkVertices() * sizeof(float);
	glBufferSubData(GL_ARRAY_BUFFER, offset, 3 * getNbTrunkVertices() * sizeof(float), &m_leavesNormals[0]);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_buffer = firTreeBuffer;
}

FirTree::~FirTree() {
	glDeleteBuffers(1, &m_buffer);

	for (Shader* &shader : m_shaders) {
		delete shader;
	}

	for (SDL_Surface* &texture : m_textures) {
		SDL_FreeSurface(texture);
	}
}

const float * FirTree::getVertices() const {
	std::vector<float> vertices;

	// Copy vertices
	std::copy(m_trunkVertices.begin(), m_trunkVertices.end(), std::back_inserter(vertices));
	std::copy(m_leavesVertices.begin(), m_leavesVertices.end(), std::back_inserter(vertices));

	return &vertices[0];
}

const float * FirTree::getNormals() const {
	std::vector<float> normals;

	// Copy normals
	std::copy(m_trunkNormals.begin(), m_trunkNormals.end(), std::back_inserter(normals));
	std::copy(m_leavesNormals.begin(), m_leavesNormals.end(), std::back_inserter(normals));

	return &normals[0];
}

const float * FirTree::getColors() const {
	std::vector<float> colors;

	// Copy uvs
	std::copy(m_trunkColors.begin(), m_trunkColors.end(), std::back_inserter(colors));
	std::copy(m_leavesColors.begin(), m_leavesColors.end(), std::back_inserter(colors));

	return &colors[0];
}

const float * FirTree::getUVs() const {
	std::vector<float> uvs;

	// Copy uvs
	std::copy(m_trunkUVs.begin(), m_trunkUVs.end(), std::back_inserter(uvs));
	std::copy(m_leavesUVs.begin(), m_leavesUVs.end(), std::back_inserter(uvs));

	return &uvs[0];
}

bool FirTree::loadShaders() {
	FILE *firTreeColorizedVertFile;
	FILE *fireTreeColorizedFragFile;

	FILE *fireTreeTexturedVertFile;
	FILE *fireTreeTexturedFragFile;

	firTreeColorizedVertFile = fopen("Shaders/TreeShaders/colorized.vert", "r");
	fireTreeColorizedFragFile = fopen("Shaders/TreeShaders/colorized.frag", "r");

	fireTreeTexturedVertFile = fopen("Shaders/TreeShaders/textured.vert", "r");
	fireTreeTexturedFragFile = fopen("Shaders/TreeShaders/textured.frag", "r");

	Shader *firTreeColorizedShader = Shader::loadFromFiles(firTreeColorizedVertFile, fireTreeColorizedFragFile);
	Shader *firTreeTexturedShader = Shader::loadFromFiles(fireTreeTexturedVertFile, fireTreeTexturedFragFile);

	fclose(firTreeColorizedVertFile);
	fclose(fireTreeColorizedFragFile);

	fclose(fireTreeTexturedVertFile);
	fclose(fireTreeTexturedFragFile);

	if (firTreeColorizedShader == NULL || firTreeTexturedShader == NULL) {
		return false;
	}

	m_shaders.push_back(firTreeColorizedShader);
	m_shaders.push_back(firTreeTexturedShader);

	return true;
}

void FirTree::loadTextures() {
	SDL_Surface* firTreeTrunkImg = IMG_Load("Ressources/firTreeTrunk.jpg");
	SDL_Surface* firTreeTrunkRGBImg = SDL_ConvertSurfaceFormat(firTreeTrunkImg, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_FreeSurface(firTreeTrunkImg);

	GLuint firTreeTrunkTextureID;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &firTreeTrunkTextureID);
	glBindTexture(GL_TEXTURE_2D, firTreeTrunkTextureID);
	{
		// Textures settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, firTreeTrunkRGBImg->w, firTreeTrunkRGBImg->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)firTreeTrunkRGBImg->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	m_textures.push_back(firTreeTrunkRGBImg);
	m_texturesIDs.push_back(firTreeTrunkTextureID);
}

void FirTree::initLight(glm::vec3 lightPosition, glm::vec3 lightColor, float ambientStrength, float diffuseStrength) {
	m_lightPosition = lightPosition;
	m_lightColor = lightColor;
	m_materials = glm::vec4(ambientStrength >= 0 ? ambientStrength : 1.f, diffuseStrength >= 0 ? diffuseStrength : 1.f, 0.05f, 128.f);

	m_isInitLight = true;
}

bool FirTree::draw(Camera &camera, glm::vec3 const &position, float const &scaling, float const &windAngle) {
	float angleRad = 90 * (M_PI / 180.f);

	glm::mat4 id(1.f);

	// Trunk transformations
	glm::mat4 trunkRotation = glm::rotate(id, -angleRad, glm::vec3(1.f, 0, 0));
	glm::mat4 trunkScaling = glm::scale(trunkRotation, glm::vec3(scaling * 0.75f, scaling * 0.75f, scaling * 3.5f));
	glm::mat4 trunkModel = glm::translate(trunkScaling, glm::vec3(-position.x, -position.z, position.y));

	// Leaves transformations
	glm::mat4 leavesScaling = glm::scale(id, glm::vec3(3.75f, 3.75f, 1.25f));

	glm::mat4 leavesBaseRangeModel = glm::translate(leavesScaling, glm::vec3(0, 0, 0.35f));
	glm::mat4 leavesUpRangeModel = glm::translate(id, glm::vec3(0, 0, 0.33f)) * glm::scale(id, glm::vec3(0.89f, 1.f, 0.89f));

	// Build fir tree scene graph
	std::stack<glm::mat4> matrices;
	matrices.push(trunkModel);

	// Draw trunk
	Shader* colorizedShader = m_shaders[0];
	Shader* texturedShader = m_shaders[1];

	GLint uMVP, uTexture;

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	glUseProgram(colorizedShader->getProgramID());

	uMVP = glGetUniformLocation(colorizedShader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(camera.lookAt() * matrices.top()));

	glUseProgram(0);

	// Textures
	glUseProgram(texturedShader->getProgramID());

	initTexturedShaderData();
	initLightData(texturedShader, camera);
	initModelViewMatrixData(texturedShader, matrices.top(), camera);

	uMVP = glGetUniformLocation(texturedShader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(camera.lookAt() * matrices.top()));

	uTexture = glGetUniformLocation(texturedShader->getProgramID(), "uTexture");
	glUniform1i(uTexture, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texturesIDs[0]);
	glDrawArrays(GL_TRIANGLES, 0, getNbTrunkVertices());
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	matrices.push(matrices.top() * leavesBaseRangeModel);

	// Draw 1st range of leaves
	glUseProgram(colorizedShader->getProgramID());

	initColorizedShaderData();
	initLightData(colorizedShader, camera);
	initModelViewMatrixData(colorizedShader, matrices.top(), camera);

	uMVP = glGetUniformLocation(colorizedShader->getProgramID(), "uMVP");
	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(camera.lookAt() * matrices.top()));

	glDrawArrays(GL_TRIANGLES, getNbTrunkVertices(), getNbLeavesVertices());

	matrices.push(matrices.top() * leavesUpRangeModel);

	// Draw 2nd range of leaves
	initModelViewMatrixData(colorizedShader, matrices.top(), camera);

	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(camera.lookAt() * glm::rotate(id, windAngle, glm::vec3(0, 0, -1.f)) * matrices.top()));
	glDrawArrays(GL_TRIANGLES, getNbTrunkVertices(), getNbLeavesVertices());

	matrices.push(matrices.top() * leavesUpRangeModel);

	// Draw 3rd range of leaves
	initModelViewMatrixData(colorizedShader, matrices.top(), camera);

	glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(camera.lookAt() * glm::rotate(id, 1.25f * windAngle, glm::vec3(0, 0, -1.f)) * matrices.top()));
	glDrawArrays(GL_TRIANGLES, getNbTrunkVertices(), getNbLeavesVertices());

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	matrices.pop();
	matrices.pop();
	matrices.pop();

	if (!matrices.empty()) {
		return false;
	}

	return true;
}

void FirTree::initColorizedShaderData() {
	Shader* colorizedShader = m_shaders[0];

	GLint vPosition, vColor, vNormal;

	// Colorized Shader data
	int offset = 0;

	vPosition = glGetAttribLocation(colorizedShader->getProgramID(), "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vPosition);

	offset += 3 * getNbVertices() * sizeof(float);

	vColor = glGetAttribLocation(colorizedShader->getProgramID(), "vColor");
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vColor);

	offset += (2 + 3) * getNbVertices() * sizeof(float);

	vNormal = glGetAttribLocation(colorizedShader->getProgramID(), "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vNormal);
}

void FirTree::initTexturedShaderData() {
	Shader* texturedShader = m_shaders[1];

	GLint vPosition, vColor, vUV, vNormal;

	// Textured Shader data
	int offset = 0;

	vPosition = glGetAttribLocation(texturedShader->getProgramID(), "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vPosition);

	offset += 3 * getNbVertices() * sizeof(float);

	vColor = glGetAttribLocation(texturedShader->getProgramID(), "vColor");
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vColor);

	offset += 3 * getNbVertices() * sizeof(float);

	vUV = glGetAttribLocation(texturedShader->getProgramID(), "vUV");
	glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vUV);

	offset += 2 * getNbVertices() * sizeof(float);

	vNormal = glGetAttribLocation(texturedShader->getProgramID(), "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vNormal);
}

void FirTree::initLightData(Shader* const &shader, Camera const &camera) {
	GLint uIsLight, uModel, uLightColor, uLightPosition, uMaterials, uCameraPosition;

	uIsLight = glGetUniformLocation(shader->getProgramID(), "uIsLight");
	glUniform1i(uIsLight, m_isInitLight);

	if (!m_isInitLight) {
		return;
	}

	uLightColor = glGetUniformLocation(shader->getProgramID(), "uLightColor");
	glUniform3f(uLightColor, m_lightColor.r, m_lightColor.g, m_lightColor.b);

	uLightPosition = glGetUniformLocation(shader->getProgramID(), "uLightPosition");
	glUniform3f(uLightPosition, m_lightPosition.x, m_lightPosition.y, m_lightPosition.z);

	uMaterials = glGetUniformLocation(shader->getProgramID(), "uMaterials");
	glUniform4f(uMaterials, m_materials.x, m_materials.y, m_materials.z, m_materials.w);

	glm::vec3 cameraPos = camera.getPosition();

	uCameraPosition = glGetUniformLocation(shader->getProgramID(), "uCameraPosition");
	glUniform3f(uCameraPosition, cameraPos.x, cameraPos.y, cameraPos.z);
}

void FirTree::initModelViewMatrixData(Shader* const &shader, glm::mat4 const & model, Camera const &camera) {
	if (!m_isInitLight) {
		return;
	}

	GLint uModelViewMatrix, uInvModelViewMatrix;

	uModelViewMatrix = glGetUniformLocation(shader->getProgramID(), "uModelViewMatrix");
	glUniformMatrix4fv(uModelViewMatrix, 1, false, glm::value_ptr(camera.getViewMatrix() * model));

	uInvModelViewMatrix = glGetUniformLocation(shader->getProgramID(), "uInvModelViewMatrix");
	glUniformMatrix4fv(uInvModelViewMatrix, 1, true, glm::value_ptr(glm::inverse(camera.getViewMatrix() * model)));
}