#define  GLM_FORCE_RADIANS

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stack>

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
#include "Cube.h"

#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))

int main(int argc, char *argv[])
{
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////
    
    //Initialize SDL2
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
    }

    //Create a Window
    SDL_Window* window = SDL_CreateWindow("VR Camera",                           //Titre
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
    glClearColor(1.0, 1.0, 1.0, 1.0); //Full White

    glEnable(GL_DEPTH_TEST); //Active the depth test

    //TODO
    //From here you can load your OpenGL objects, like VBO, Shaders, etc.
    //TODO

    bool isOpened = true;

    Cube cube;

    GLuint myBuffer;
    glGenBuffers(1, &myBuffer);

      glBindBuffer(GL_ARRAY_BUFFER, myBuffer);
      glBufferData(GL_ARRAY_BUFFER, cube.getNbVertices() * (3 + 3)*sizeof(float), NULL, GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * cube.getNbVertices(), cube.getVertices());
      glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * cube.getNbVertices(), 3 * sizeof(float) * cube.getNbVertices(), cube.getNormals());
      glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Camera
    glm::mat4 projection = glm::perspective(glm::radians(110.f), (float) WIDTH / HEIGHT, 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(glm::vec3(1.f, 1.f, -1.f), glm::vec3(0, 0, 0), glm::vec3(0, 1.f, 0));
    glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 0.01f, 1.f));

    Shader* shader = Shader::loadFromFiles(fopen("Shaders/color.vert", "r"), fopen("Shaders/color.frag", "r"));
    if (!shader)
    {
      std::cerr << "Could not compile Shader... exiting" << std::endl;
      return EXIT_FAILURE;
    }

    glUseProgram(shader->getProgramID());

    //Main application loop
    while(isOpened)
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

        //Fetch the SDL events
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                    switch(event.window.event)
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





        //TODO rendering
        
        glBindBuffer(GL_ARRAY_BUFFER, myBuffer);

          GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
          glVertexAttribPointer(vPosition, 3, GL_FLOAT, 0, 0, 0);
          glEnableVertexAttribArray(vPosition);

          GLint vNormal = glGetAttribLocation(shader->getProgramID(), "vNormal");
          glVertexAttribPointer(vNormal, 3, GL_FLOAT, 0, 0, INDICE_TO_PTR(3 * cube.getNbVertices() * sizeof(float)));
          glEnableVertexAttribArray(vNormal);
          
          std::stack<glm::mat4> matrices;
          matrices.push(projection * view); // Camera matrix

          matrices.push(matrices.top() * model);

          GLint v = glGetUniformLocation(shader->getProgramID(), "uMVP");
          glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(matrices.top()));
          glDrawArrays(GL_TRIANGLES, 0, cube.getNbVertices());
          glFinish();
          matrices.pop();

          float angle = M_PI / 2.0;
          glm::mat4 translateZ = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0.5f));
          glm::mat4 rotateX = glm::rotate(translateZ, angle, glm::vec3(1.f, 0, 0));

          matrices.push(matrices.top() * rotateX * model);
          glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(matrices.top()));
          glDrawArrays(GL_TRIANGLES, 0, cube.getNbVertices());
          glFinish();
          matrices.pop();
          // glm::mat4 translateX = glm::translate(rotateZ, glm::vec3(0.5f, 0, 0));
          glm::mat4 translateX = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0, 0));
         // glm::mat4 rotateZ = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 0, 1.f));
          
          glm::mat4 rotateZ = glm::rotate(translateX, angle, glm::vec3(0, 0, 1.f));

          matrices.push(matrices.top() * rotateZ* model);

          glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(matrices.top()));
          glDrawArrays(GL_TRIANGLES, 0, cube.getNbVertices());
          glFinish();
          matrices.pop();

          matrices.pop();       

        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
    }
    
    glUseProgram(0);
    delete shader;
    glDeleteBuffers(1, &myBuffer);

    //Free everything
    if(context != NULL)
        SDL_GL_DeleteContext(context);
    if(window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}
