#include "Environment.h"

#include <cmath>
#include <stack>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cube.h>

Environment::Environment() {
	Cube cube = Cube();

	m_vertices.assign(cube.getVertices(), cube.getVertices() + (3 * cube.getNbVertices()));
	m_normals.assign(cube.getNormals(), cube.getNormals() + (3 * cube.getNbVertices()));
	m_uvs.assign(cube.getUVs(), cube.getUVs() + (2 * cube.getNbVertices()));

	// Put all fir tree parts in the VBO
	GLuint environmentBuffer;
	glGenBuffers(1, &environmentBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, environmentBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube.getNbVertices() * (3 + 3 + 2) * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * cube.getNbVertices(), cube.getVertices());
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * cube.getNbVertices(), 3 * sizeof(float) * cube.getNbVertices(), cube.getNormals());
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * cube.getNbVertices(), 2 * sizeof(float) * cube.getNbVertices(), cube.getUVs());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_buffer = environmentBuffer;
}

Environment::~Environment() {
	glDeleteBuffers(1, &m_buffer);

	delete m_texturedShader;

	for (SDL_Surface* &texture : m_textures) {
		SDL_FreeSurface(texture);
	}
}

bool Environment::loadShaders() {
	FILE* VertFileT = fopen("./Shaders/EnvironmentShaders/textured.vert", "r");
	FILE* FragFileT = fopen("./Shaders/EnvironmentShaders/textured.frag", "r");

	Shader* textureShader = Shader::loadFromFiles(VertFileT, FragFileT);

	fclose(VertFileT);
	fclose(FragFileT);

	if (textureShader == NULL) {
		return false;
	}

	m_texturedShader = textureShader;

	return true;
}

void Environment::loadTextures() {
	SDL_Surface* imgFront = IMG_Load("Ressources/front.JPG");
	SDL_Surface* imgDown = IMG_Load("Ressources/down.JPG");

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

void Environment::initLight(glm::vec3 lightPosition, glm::vec3 lightColor, float ambientStrength, float diffuseStrength) {
	m_lightPosition = lightPosition;
	m_lightColor = lightColor;
	m_materials = glm::vec4(ambientStrength, diffuseStrength , 0.1f, 15.f);

	m_isInitLight = true;
	m_lightState = true;
}

bool Environment::draw(Camera &camera, glm::vec3 const &position, float const &scaling) {
	float angle = 90 * (M_PI / 180.f);

	//definition of the parameter of texture multiplication factor
	GLint textureMultiplier = glGetUniformLocation(m_texturedShader->getProgramID(),"textureMultiplier");

	glm::mat4 id(1.f);
	glm::mat4 environmentModel = glm::translate(id, position);

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

	GLint uMVP = glGetUniformLocation(m_texturedShader->getProgramID(), "uMVP");
	GLint uTexture = glGetAttribLocation(m_texturedShader->getProgramID(), "uTexture");

	// Build scene graph
	std::stack<glm::mat4> matrices;
	matrices.push(environmentModel);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	glUseProgram(m_texturedShader->getProgramID());


	glUniform1f(textureMultiplier,5.f);

	// drawing front background element

	initTexturedShaderData();
	initLightData(camera);
	toggleLight();

	matrices.push(matrices.top() * backgroundModel);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(camera.lookAt() * matrices.top()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texturesIDs[0]);
	glUniform1i(uTexture, 0);
	glDrawArrays(GL_TRIANGLES, 0, getNbVertices());

	matrices.pop();

	// drawing left background element

	matrices.push(matrices.top() * leftModel);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(camera.lookAt() * matrices.top()));

	glDrawArrays(GL_TRIANGLES, 0, getNbVertices());

	matrices.pop();


	glUniform1f(textureMultiplier,35.0f);

	// drawing floor element

	matrices.push(matrices.top() * floorModel);
	toggleLight();
	initModelViewMatrixData(matrices.top(), camera);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(camera.lookAt() * matrices.top()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texturesIDs[1]);
	glUniform1i(uTexture, 0);
	glDrawArrays(GL_TRIANGLES, 0, getNbVertices());

	matrices.pop();

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	matrices.pop();

	if (!matrices.empty()) {
		return false;
	}

	return true;
}

void Environment::initTexturedShaderData() {
	GLint vPosition, vNormal, vUV ;

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

void Environment::initLightData(Camera const &camera) {
	GLint uIsLight, uModel, uLightColor, uLightPosition, uMaterials, uCameraPosition;

	uIsLight = glGetUniformLocation(m_texturedShader->getProgramID(), "uIsLight");
	glUniform1i(uIsLight, m_isInitLight);

	if (!m_isInitLight || !m_lightState) {
		return;
	}

	uLightColor = glGetUniformLocation(m_texturedShader->getProgramID(), "uLightColor");
	glUniform3f(uLightColor, m_lightColor.r, m_lightColor.g, m_lightColor.b);

	uLightPosition = glGetUniformLocation(m_texturedShader->getProgramID(), "uLightPosition");
	glUniform3f(uLightPosition, m_lightPosition.x, m_lightPosition.y, m_lightPosition.z);

	uMaterials = glGetUniformLocation(m_texturedShader->getProgramID(), "uMaterials");
	glUniform4f(uMaterials, m_materials.x, m_materials.y, m_materials.z, m_materials.w);

	glm::vec3 cameraPos = camera.getPosition();

	uCameraPosition = glGetUniformLocation(m_texturedShader->getProgramID(), "uCameraPosition");
	glUniform3f(uCameraPosition, cameraPos.x, cameraPos.y, cameraPos.z);
}

void Environment::initModelViewMatrixData(glm::mat4 const & model, Camera const &camera) {
	if (!m_isInitLight || !m_lightState) {
		return;
	}

	GLint uModelViewMatrix, uInvModelViewMatrix;

	uModelViewMatrix = glGetUniformLocation(m_texturedShader->getProgramID(), "uModelViewMatrix");
	glUniformMatrix4fv(uModelViewMatrix, 1, false, glm::value_ptr(camera.getViewMatrix() * model));

	uInvModelViewMatrix = glGetUniformLocation(m_texturedShader->getProgramID(), "uInvModelViewMatrix");
	glUniformMatrix4fv(uInvModelViewMatrix, 1, true, glm::value_ptr(glm::inverse(camera.getViewMatrix() * model)));
}

void Environment::toggleLight() {
	if (!m_isInitLight) {
		return;
	}

	m_lightState = !m_lightState;

	GLint uLightState = glGetUniformLocation(m_texturedShader->getProgramID(), "uLightState");
	glUniform1i(uLightState, m_lightState);
}