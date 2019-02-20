#include "Environement.h"

#include <cmath>
#include <stack>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cube.h>

Environement::Environement() {
	Cube cube = Cube();

	m_vertices.assign(cube.getVertices(), cube.getVertices() + (3 * cube.getNbVertices()));
	m_normals.assign(cube.getNormals(), cube.getNormals() + (3 * cube.getNbVertices()));
	m_uvs.assign(cube.getUVs(), cube.getUVs() + (2 * cube.getNbVertices()));

	// Put all fir tree parts in the VBO
	GLuint environementBuffer;
	glGenBuffers(1, &environementBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, environementBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube.getNbVertices() * (3 + 3 + 2) * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * cube.getNbVertices(), cube.getVertices());
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * cube.getNbVertices(), 3 * sizeof(float) * cube.getNbVertices(), cube.getNormals());
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * cube.getNbVertices(), 2 * sizeof(float) * cube.getNbVertices(), cube.getUVs());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_buffer = environementBuffer;
}

Environement::~Environement() {
	glDeleteBuffers(1, &m_buffer);

	delete m_texturedShader;

	for (SDL_Surface* &texture : m_textures) {
		SDL_FreeSurface(texture);
	}
}

bool Environement::loadShaders() {
	FILE* VertFileT = fopen("./Shaders/EnvironnementShaders/EnviTexture.vert", "r");
	FILE* FragFileT = fopen("./Shaders/EnvironnementShaders/EnviTexture.frag", "r");

	Shader* textureShader = Shader::loadFromFiles(VertFileT, FragFileT);

	fclose(VertFileT);
	fclose(FragFileT);

	if (textureShader == NULL) {
		return false;
	}

	m_texturedShader = textureShader;

	return true;
}

void Environement::loadTextures() {
	SDL_Surface* imgFront = IMG_Load("Ressources/front.jpg");
	SDL_Surface* imgDown = IMG_Load("Ressources/down.jpg");

	SDL_Surface* imgFrontRGB = SDL_ConvertSurfaceFormat(imgFront, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_Surface* imgDownRGB = SDL_ConvertSurfaceFormat(imgDown, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_FreeSurface(imgFront);
	SDL_FreeSurface(imgDown);

	GLuint frontTextureID;
	glGenTextures(1, &frontTextureID);
	glBindTexture(GL_TEXTURE_2D, frontTextureID);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgFrontRGB->w, imgFrontRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)imgFrontRGB->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	GLuint downTextureID;
	glGenTextures(1, &downTextureID);
	glBindTexture(GL_TEXTURE_2D, downTextureID);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgDownRGB->w, imgDownRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)imgDownRGB->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	m_textures.push_back(imgFrontRGB);
	m_textures.push_back(imgDownRGB);
	m_texturesIDs.push_back(frontTextureID);
	m_texturesIDs.push_back(downTextureID);
}

bool Environement::draw(Camera &camera, glm::vec3 const &position, float const &scaling) {
	float angle = 90 * (M_PI / 180.f);

	glm::mat4 id(1.f);
	glm::mat4 environementModel = glm::translate(id, position);

	// Background
	glm::mat4 rotateX = glm::rotate(id, angle, glm::vec3(1.f, 0, 0));
	glm::mat4 scaleX = glm::scale(id, glm::vec3(scaling * 2.f, scaling * 2.f, 0));
	glm::mat4 translateZ = glm::translate(id, glm::vec3(0, scaling * 1.f, scaling * 1.f));

	glm::mat4 backgroundModel = translateZ * scaleX * rotateX;

	// Floor
	glm::mat4 scaleY = glm::scale(id, glm::vec3(scaling * 2.f, 0, scaling * 2.f));

	glm::mat4 floorModel = scaleY;

	// Left
	glm::mat4 rotateZ = glm::rotate(id, angle, glm::vec3(1.f, 0, 0)) * glm::rotate(id, angle, glm::vec3(0, 0, 1.f)); // Rotation texture around X, RotationZ
	glm::mat4 scaleZ = glm::scale(id, glm::vec3(0, scaling * 2.f, scaling * 2.f));
	glm::mat4 translateX = glm::translate(id, glm::vec3(scaling * 1.f, scaling * 1.f, 0));

	glm::mat4 leftModel = translateX * scaleZ * rotateZ;

	GLint uMVP = glGetUniformLocation(m_texturedShader->getProgramID(), "uMvp");
	GLint uTexture = glGetAttribLocation(m_texturedShader->getProgramID(), "uTexture");

	// Build scene graph
	std::stack<glm::mat4> matrices;
	matrices.push(camera.lookAt());

	matrices.push(matrices.top() * environementModel);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	glUseProgram(m_texturedShader->getProgramID());

	initTexturedShaderData();

	matrices.push(matrices.top() * backgroundModel);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(matrices.top()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texturesIDs[0]);
	glUniform1i(uTexture, 0);
	glDrawArrays(GL_TRIANGLES, 0, getNbVertices());

	matrices.pop();

	matrices.push(matrices.top() * leftModel);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(matrices.top()));

	glDrawArrays(GL_TRIANGLES, 0, getNbVertices());

	matrices.pop();
	matrices.push(matrices.top() * floorModel);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(matrices.top()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texturesIDs[1]);
	glUniform1i(uTexture, 0);
	glDrawArrays(GL_TRIANGLES, 0, getNbVertices());

	matrices.pop();

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	matrices.pop();
	matrices.pop();

	if (!matrices.empty()) {
		return false;
	}

	return true;
}

void Environement::initTexturedShaderData() {
	GLint vPosition, vNormal, vUV;

	int offset = 0;

	vPosition = glGetAttribLocation(m_texturedShader->getProgramID(), "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vPosition);

	offset += 3 * getNbVertices() * sizeof(float);

	vNormal = glGetAttribLocation(m_texturedShader->getProgramID(), "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vNormal);

	offset += 3 * getNbVertices() * sizeof(float);

	vUV = glGetAttribLocation(m_texturedShader->getProgramID(), "vUV");
	glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)(offset));
	glEnableVertexAttribArray(vUV);
}