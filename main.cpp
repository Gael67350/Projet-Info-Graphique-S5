#define  GLM_FORCE_RADIANS

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stack>

//SDL Libraries
#include <SDL2/SDL.h>
//#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"
#include "Cube.h"

#include "logger.h"

#include <Fire.h>

// Geometry
#include <Geometry.h>
#include <FirTree.h>

// Camera
#include <Camera.h>

// Utils
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <stack>

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)

int main(int argc, char *argv[])
{
	////////////////////////////////////////
	//SDL2 / OpenGL Context initialization :
	////////////////////////////////////////

	//Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
		return 0;
	}

	//Initialize the IMG component
	if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
		ERROR("Could not load SDL2_image with JPG files\n");
		return EXIT_FAILURE;
	}


	//Create a Window
	SDL_Window *window = SDL_CreateWindow("VR Camera",                           //Titre
										  SDL_WINDOWPOS_UNDEFINED,               //X Position
										  SDL_WINDOWPOS_UNDEFINED,               //Y Position
										  WIDTH, HEIGHT,                         //Resolution
										  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)

	//Initialize OpenGL Version (version 3.0)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	//Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//Tells GLEW to initialize the OpenGL function with this version
	glewExperimental = GL_TRUE;
	glewInit();

	//Start using OpenGL to draw something on screen
	glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

	//The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
	glClearColor(1.0, 1.0, 1.0, 1.0); //Full Black

	// Init random
	srand(time(NULL));

	// Create FirTree
	FirTree firTree = FirTree(500);

	// View from world space to camera space
	Camera camera = Camera((float) WIDTH / HEIGHT, 110.f);
	camera.setPosition(glm::vec3(0, 5.f, -12.f));

	// Generate trees coordinates
	size_t nbFirTrees = 200;
	size_t nbDrawnTrees = 0;

	float maxRadius = 42, minRadius = 10, nbSlice = 10;

	std::vector<glm::vec3> treesCoordinates;

	for (float radius = minRadius; radius <= maxRadius; radius += (maxRadius - minRadius) / nbSlice) {
		if (nbDrawnTrees > nbFirTrees) {
			break;
		}

		for (double theta = 0; theta <= M_PI; theta += (M_PI * nbSlice) / nbFirTrees) {
			if (nbDrawnTrees > nbFirTrees) {
				break;
			}

			float randOffsetX = (float) (rand() % 2 + 1);
			float randOffsetZ = (float) (rand() % 2 + 1);

			if (randOffsetX > 1.6) { randOffsetX = 1.6f; }
			if (randOffsetZ > 1.6) { randOffsetZ = 1.6f; }

			float x = (radius * randOffsetX) * cos(theta);
			float z = (radius * randOffsetZ) * sin(theta);

			treesCoordinates.push_back(glm::vec3(x, 0, z));

			nbDrawnTrees++;
		}
	}

	// Load textures
	firTree.loadTextures();

	// Load Shaders
	if (!firTree.loadShaders()) {
		return EXIT_FAILURE;
	}


	//loading Fire and it's global modification matrix modification
	glm::mat4 fireModificationMatrix = glm::mat4(1.0f);
	fireModificationMatrix = glm::scale(fireModificationMatrix, glm::vec3(0.9f, 0.9f, 0.9f));
	fireModificationMatrix = glm::translate(fireModificationMatrix, glm::vec3(-0.3f, 0.f, -7.f));
	Fire campFire(fireModificationMatrix);

    //setting up the transparency management
    glEnable(GL_DEPTH_TEST); //Activation of the depth test

    //enableing transparency
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Create environnement
	Cube cube = Cube();

	if (!(IMG_Init(IMG_INIT_JPG)&IMG_INIT_JPG))
	{
		ERROR("could not load SDL2_image with JPG files \n");
		throw EXIT_FAILURE;
	}


	SDL_Surface* imgleft = IMG_Load("Ressources/left.jpg");
	SDL_Surface* imgRight = IMG_Load("Ressources/right.jpg");
	SDL_Surface* imgFront = IMG_Load("Ressources/front.jpg");
	SDL_Surface* imgBottom = IMG_Load("Ressources/back.jpg");
	SDL_Surface* imgDown = IMG_Load("Ressources/down.jpg");
	SDL_Surface* imgUp = IMG_Load("Ressources/up.jpg");


	SDL_Surface* imgLeftRGB = SDL_ConvertSurfaceFormat(imgleft, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Surface* imgRightRGB = SDL_ConvertSurfaceFormat(imgRight, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Surface* imgFrontRGB = SDL_ConvertSurfaceFormat(imgFront, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Surface* imgBottomRGB = SDL_ConvertSurfaceFormat(imgBottom, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Surface* imgDownRGB = SDL_ConvertSurfaceFormat(imgDown, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Surface* imgUpRGB = SDL_ConvertSurfaceFormat(imgUp, SDL_PIXELFORMAT_RGBA8888, 0);
	//SDL_Surface* rockRGB = SDL_ConvertSurfaceFormat(imgRock, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_FreeSurface(imgleft);
	SDL_FreeSurface(imgRight);
	SDL_FreeSurface(imgFront);
	SDL_FreeSurface(imgBottom);
	SDL_FreeSurface(imgDown);
	SDL_FreeSurface(imgUp);

	//log texture definition
	GLuint leftTexture;
	glGenTextures(1, &leftTexture);
	glBindTexture(GL_TEXTURE_2D, leftTexture);

	//loading textures

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgLeftRGB->w, imgLeftRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)imgLeftRGB->pixels);


	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);


	GLuint frontTexture;
	//rock texture definition
	glGenTextures(1, &frontTexture);
	glBindTexture(GL_TEXTURE_2D, frontTexture);

	//loading textures

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgFrontRGB->w, imgFrontRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)imgFrontRGB->pixels);

	GLuint downTexture;
	//rock texture definition
	glGenTextures(1, &downTexture);
	glBindTexture(GL_TEXTURE_2D, downTexture);

	//loading textures

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgDownRGB->w, imgDownRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)imgDownRGB->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	//two different programs are defined respectively to display colors for the flame
	//and to display textures on the rocks and logs


	FILE* VertFileT = fopen("./Shaders/EnvironnementShaders/EnviTexture.vert", "r");
	FILE* FragFileT = fopen("./Shaders/EnvironnementShaders/EnviTexture.frag", "r");

	//auto colorShader = Shader::loadFromFiles(VertFileC, FragFileC);
	Shader* textureShader = Shader::loadFromFiles(VertFileT,FragFileT);
	fclose(VertFileT);
	fclose(FragFileT);

	if (textureShader == NULL)
	{
		std::cerr << "COULD NOT LOAD SHADERS" << std::endl;
		throw;
	}

	GLuint EnvironementBuffer;

	glGenBuffers(1, &EnvironementBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, EnvironementBuffer);
	glBufferData(GL_ARRAY_BUFFER, cube.getNbVertices() * (3 + 3 + 2)*sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * cube.getNbVertices(), cube.getVertices());
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * cube.getNbVertices(), 3 * sizeof(float) * cube.getNbVertices(), cube.getNormals());
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * cube.getNbVertices(), 2 * sizeof(float) * cube.getNbVertices(), cube.getUVs());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Camera
	glm::mat4 projection = glm::perspective(glm::radians(110.f), (float) WIDTH / HEIGHT, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(1.f, 1.f, -1.f), glm::vec3(0, 0, 0), glm::vec3(0, 1.f, 0));
    //glm::mat4 view = camera.lookAt();
	glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 0.01f, 1.f));

	bool isOpened = true;

	//Main application loop
	while (isOpened)
	{
		//Time in ms telling us when this frame started. Useful for keeping a fix framerate
		uint32_t timeBegin = SDL_GetTicks();

		//Fetch the SDL events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case SDL_WINDOWEVENT:
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_CLOSE:
							isOpened = false;
							break;
						default:
							break;
					}
					break;
					//We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
			}
		}

		//Clear the screen : the depth buffer and the color buffer
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		// Draw environnement
		glUseProgram(textureShader->getProgramID());

		glBindBuffer(GL_ARRAY_BUFFER, EnvironementBuffer);


		std::stack<glm::mat4> matrices;
		matrices.push(projection * view); // Camera matrix


		//redefinition of the vars passed to the shader
		//to bind it on the right program
		//definition of a variable to store the current reading offset (reusing the one used before for the insertion)
		int currentOffset = 0;

		//definition of the vars passed to the shader
		GLint v = glGetUniformLocation(textureShader->getProgramID(), "uMvp");
		GLint uTexture = glGetAttribLocation(textureShader->getProgramID(), "uTexture");
		GLint vPosition = glGetAttribLocation(textureShader->getProgramID(), "vPosition");
		GLint vNormals = glGetAttribLocation(textureShader->getProgramID(), "vNormals");
		GLint vUV = glGetAttribLocation(textureShader->getProgramID(), "vUV");
		GLint uScale= glGetAttribLocation(textureShader->getProgramID(), "uScale");
		//selection of datas in the VBOS
		//vertexes
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(currentOffset));
		glEnableVertexAttribArray(vPosition);

		currentOffset += 3 * sizeof(float) * cube.getNbVertices();

		//normals
		glVertexAttribPointer(vNormals, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(currentOffset));
		glEnableVertexAttribArray(vNormals);

		currentOffset += 3 * sizeof(float) * cube.getNbVertices();

		//uvDatas
		glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(currentOffset));
		glEnableVertexAttribArray(vUV);

		//left skybox drawing section
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frontTexture);
		glUniform1i(uTexture, 0);

		float angle = M_PI / 2.0;
		glm::mat4 translateZ = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 5.f));

		translateZ= glm::scale(translateZ, glm::vec3(10.f, 10.f, 10.f));
		glm::mat4 rotateX = glm::rotate(translateZ, angle, glm::vec3(1.f, 0, 0));

		matrices.push(matrices.top() * rotateX * model);
		glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(matrices.top()));
		glDrawArrays(GL_TRIANGLES, 0, cube.getNbVertices());
		glFinish();



		matrices.pop();
		// glm::mat4 translateX = glm::translate(rotateZ, glm::vec3(0.5f, 0, 0));
		glm::mat4 translateX = glm::translate(glm::mat4(1.f), glm::vec3(-5.f, 0, 0));
		// glm::mat4 rotateZ = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 0, 1.f));

		translateX = glm::scale(translateX, glm::vec3(10.f, 10.f, 10.f));
		glm::mat4 rotateZ = glm::rotate(translateX, angle, glm::vec3(0, 0, 1.f));
        glm::mat4 rotationCorrect = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.f, 0, 0));

		matrices.push(matrices.top() * rotationCorrect *rotateZ* model);

		glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(matrices.top()));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, leftTexture);
        glUniform1i(uTexture, 0);
        glDrawArrays(GL_TRIANGLES, 0, cube.getNbVertices());
		glFinish();
		matrices.pop();

		matrices.pop();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(0);


		// Draw forest
		for (glm::vec3 const &coord : treesCoordinates) {
			firTree.draw(camera, glm::vec3(coord.x, coord.y, coord.z), 5.f);
		}

		//Draw fire
		campFire.draw(camera);

        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if (timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
	}

	//Free
	glUseProgram(0);

	delete textureShader;
	glDeleteBuffers(1, &EnvironementBuffer);

	//Free everything
	if (context != NULL)
		SDL_GL_DeleteContext(context);
	if (window != NULL)
		SDL_DestroyWindow(window);
	return 0;
}