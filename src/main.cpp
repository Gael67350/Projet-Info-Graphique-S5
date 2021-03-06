﻿#define  GLM_FORCE_RADIANS

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
#include <Environment.h>

// Camera
#include <Camera.h>

// Utils
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <stack>
#include <GrassElement.h>
#include <cmath>

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
	glClearColor(0.0, 0.0, 0.0, 1.0); //Full Black

	// Init random
	srand(time(NULL));

	// Create Environment
	Environment environment = Environment();

	// Create FirTree
	FirTree firTree = FirTree(500);

	//creating grass element

	GrassElement grass = GrassElement();

	// View from world space to camera space
	Camera camera = Camera((float)WIDTH / HEIGHT, 110.f);
	camera.setPosition(glm::vec3(0.f, 5.f, -12.f));

	// Generate trees coordinates
	size_t nbFirTrees = 200;
	size_t nbDrawnTrees = 0;

	float maxRadius = 42, minRadius = 10, nbSlice = 10;

	std::vector<glm::vec3> treesCoordinates;
	std::vector<std::pair<float, bool>> treesWindData;

    // Generate trees coordinates
	float maxRadiusGrass = 2.5, minRadiusGrass = 1.8, nbSliceGrass = 2;
    size_t nbGrass = 9;

    std::vector<glm::vec3> grassCoordinates;

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
      
			glm::vec3 currentVec = glm::vec3(x, 0, z);

			treesCoordinates.push_back(currentVec);
      
      	float windAngle = (float(rand()) / (float)RAND_MAX) * (FirTree::WIND_MAX_ANGLE - FirTree::WIND_MIN_ANGLE) + FirTree::WIND_MIN_ANGLE;

			treesWindData.push_back(std::make_pair(windAngle, false));

            size_t nbDrawnGrass = 0;

            if(radius < minRadius+3*((maxRadius - minRadius) / nbSlice))
            {
                //generating grass elements arround trees
                for (float radiusC = minRadiusGrass;
                     radiusC <= maxRadiusGrass; radiusC += (maxRadiusGrass - minRadiusGrass) / nbSliceGrass) {
                    if (nbDrawnGrass >= nbGrass) {
                        break;
                    }

                    for (double thetaC = 0; thetaC <= 2*M_PI; thetaC += (2*M_PI * nbSliceGrass) / nbGrass) {
                        if (nbDrawnGrass >= nbGrass) {
                            break;
                        }

                        float randOffsetXC = (float) (rand() % 2 + 1);
                        float randOffsetZC = (float) (rand() % 2 + 1);

                        if (randOffsetXC > 1.6) { randOffsetXC = 1.6f; }
                        if (randOffsetZC > 1.6) { randOffsetZC = 1.6f; }

                        float xC = (radiusC * randOffsetXC) * cos(thetaC);
                        float zC = (radiusC * randOffsetZC) * sin(thetaC);

                        grassCoordinates.push_back(glm::vec3(xC, 0, zC) + currentVec);

                        nbDrawnGrass++;
                    }
                }
            }
			nbDrawnTrees++;
		}
	}

	// Load textures
	firTree.loadTextures();
	environment.loadTextures();

	// Load Shaders
	if (!firTree.loadShaders()) {
		return EXIT_FAILURE;
	}

	if (!environment.loadShaders()) {
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

	if (!(IMG_Init(IMG_INIT_JPG)&IMG_INIT_JPG))
	{
		ERROR("could not load SDL2_image with JPG files \n");
		throw EXIT_FAILURE;
	}

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

		// Update lights
		std::tuple<glm::vec3, float, float, glm::vec3> lightInfo = campFire.getLightInfo();
		firTree.initLight(std::get<0>(lightInfo), std::get<3>(lightInfo), std::get<2>(lightInfo), std::get<1>(lightInfo));

		float environmentDiffuse = 1.f * std::get<1>(lightInfo);
		environment.initLight(std::get<0>(lightInfo), std::get<3>(lightInfo), 0.3f, environmentDiffuse <= 1.f ? environmentDiffuse : 1.f);

		// Draw environment
		environment.draw(camera, glm::vec3(-40.f, -10.f, -40.f), 120.f);

        //Draw grass
        for (glm::vec3 const &coord : grassCoordinates)
        {
            grass.draw(camera,glm::vec3(coord.x, coord.y, coord.z) , campFire.getLightInfo());
        }

		// Draw forest
		for (int i = 0; i < treesCoordinates.size(); i++) {
			glm::vec3 coord = treesCoordinates.at(i);
			std::pair<float, bool> &windData = treesWindData.at(i);

			firTree.draw(camera, glm::vec3(coord.x, coord.y, coord.z), 5.f, windData.first);

			if (windData.first < FirTree::WIND_MIN_ANGLE) {
				windData.second = !windData.second;
				windData.first = FirTree::WIND_MIN_ANGLE;
			}
			else if (windData.first > FirTree::WIND_MAX_ANGLE) {
				windData.second = !windData.second;
				windData.first = FirTree::WIND_MAX_ANGLE;
			}

			if (windData.second) {
				windData.first += FirTree::WIND_SPEED * (M_PI / 180.f);
			}
			else {
				windData.first -= FirTree::WIND_SPEED_RETURN * (M_PI / 180.f);
			}


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

	//Free everything
	if (context != NULL)
		SDL_GL_DeleteContext(context);
	if (window != NULL)
		SDL_DestroyWindow(window);
	return 0;
}