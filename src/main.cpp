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

#include "Shader.h"

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

int main(int argc, char *argv[]) {
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

	glEnable(GL_DEPTH_TEST); //Active the depth test

	//enableing transparency
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//TODO
	//From here you can load your OpenGL objects, like VBO, Shaders, etc.

	// Init random
	srand(time(NULL));

	// Create FirTree
	FirTree firTree = FirTree(500);

	// View from world space to camera space
	Camera camera = Camera((float)WIDTH / HEIGHT, 110.f);
	camera.setPosition(glm::vec3(0, 4.f, -10.f));

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

			float randOffsetX = (float)(rand() % 2 + 1);
			float randOffsetZ = (float)(rand() % 2 + 1);

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

  //loading Fire
  Fire campFire;
  
	bool isOpened = true;

	//Main application loop
	while (isOpened) {
		//Time in ms telling us when this frame started. Useful for keeping a fix framerate
		uint32_t timeBegin = SDL_GetTicks();

		//Fetch the SDL events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
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

		// Draw forest
		for (glm::vec3 const &coord : treesCoordinates) {
			firTree.draw(camera, glm::vec3(coord.x, coord.y, coord.z), 1.f);
		}

    //Draw fire
    campFire.draw();

    
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

	//Free everything
	if (context != NULL)
		SDL_GL_DeleteContext(context);
	if (window != NULL)
		SDL_DestroyWindow(window);

	return 0;
}
