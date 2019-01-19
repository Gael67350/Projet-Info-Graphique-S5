//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "logger.h"

// Geometry
#include <Geometry.h>
#include <FireTree.h>

// Camera
#include <Camera.h>

// Utils
#include <stdio.h>
#include <stack>

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))
#define DEGREES_TO_RADIANS(x) x*(3.141592f/180.0f)

int main(int argc, char *argv[]) {
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////

    //Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
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

    //TODO
    //From here you can load your OpenGL objects, like VBO, Shaders, etc.

	// Create FireTree
	FireTree fireTree = FireTree(500);

    // Put all fire tree parts in the VBO
    GLuint buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, (3 + 3) * sizeof(float) * fireTree.getNbVertices(), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * fireTree.getNbVertices(), fireTree.getVertices());
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * fireTree.getNbVertices(), 3 * sizeof(float) * fireTree.getNbVertices(), fireTree.getColors());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // View from world space to camera space
    Camera camera = Camera((float)WIDTH / HEIGHT, 45.f);
    camera.setPosition(glm::vec3(4.f, 4.f, -10.f));

    // Load Shaders
    FILE *vertFile;
    FILE *fragFile;

    vertFile = fopen("Shaders/color.vert", "r");
    fragFile = fopen("Shaders/color.frag", "r");

    Shader *shader = Shader::loadFromFiles(vertFile, fragFile);

    fclose(vertFile);
    fclose(fragFile);

    if (shader == NULL) {
        return EXIT_FAILURE;
    }

    glUseProgram(shader->getProgramID());
    //TODO

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





        //TODO rendering
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(0));
        glEnableVertexAttribArray(vPosition);

		GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(3 * sizeof(float) * fireTree.getNbVertices()));
		glEnableVertexAttribArray(vColor);

        // Draw fire tree
		fireTree.draw(shader, camera);

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
    delete shader;
    glDeleteBuffers(1, &buffer);

    //Free everything
    if (context != NULL)
        SDL_GL_DeleteContext(context);
    if (window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}
