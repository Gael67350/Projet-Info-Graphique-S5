//SDL Libraries
#include <SDL2/SDL.h>
//#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "logger.h"

//geometric shapes imports
#include "Cube.h"
#include "Cylinder.h"
#include "Sphere.h"

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

    //definition of the different shapes used in the fire and definition of the associated VBO
    Cylinder log = Cylinder(50);
    Sphere rock = Sphere(50,50);
    Cube flameParticle = Cube();

    GLuint fireBuffer;
    glGenBuffers(1,&fireBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,fireBuffer);

    int nbVerticesTot = rock.getNbVertices() + flameParticle.getNbVertices() + log.getNbVertices();

    glBufferData(GL_ARRAY_BUFFER, (3*2 + 2)*sizeof(float) * nbVerticesTot , NULL , GL_DYNAMIC_DRAW );

    int currentOffset = 0;

    //insertion of vertices data in the VBO
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 3 * sizeof(float)* log.getNbVertices() , log.getVertices());
    currentOffset += 3 * sizeof(float)* log.getNbVertices();
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 3 * sizeof(float)*rock.getNbVertices() , rock.getVertices());
    currentOffset += 3 * sizeof(float)* rock.getNbVertices();
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 3*sizeof(float)*flameParticle.getNbVertices(),flameParticle.getVertices());
    currentOffset += 3 * sizeof(float)* flameParticle.getNbVertices();


    //insertion of normal data in the VBO
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 3 * sizeof(float)* log.getNbVertices() , log.getNormals());
    currentOffset += 3 * sizeof(float)* log.getNbVertices();
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 3 * sizeof(float)* rock.getNbVertices() , rock.getNormals());
    currentOffset += 3 * sizeof(float)* rock.getNbVertices();
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 3 * sizeof(float) * flameParticle.getNbVertices() , flameParticle.getNormals());
    currentOffset += 3 * sizeof(float)* flameParticle.getNbVertices();


    //insertion of UV data in the VBO
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 2 * sizeof(float)* log.getNbVertices() , log.getUVs());
    currentOffset += 2 * sizeof(float)* log.getNbVertices();
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 2 * sizeof(float)* rock.getNbVertices() , rock.getUVs());
    currentOffset += 2 * sizeof(float)* rock.getNbVertices();
    glBufferSubData(GL_ARRAY_BUFFER, currentOffset , 2 * sizeof(float)* flameParticle.getNbVertices() , flameParticle.getUVs());


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //loading the shaders

    FILE* fireVertFileC = fopen("./Shaders/FireShaders/colorized.vert","r");
    FILE* fireFragFileC = fopen("./Shaders/FireShaders/colorized.frag","r");

    FILE* fireVertFileT = fopen("./Shaders/FireShaders/textured.vert","r");
    FILE* fireFragFileT = fopen("./Shaders/FireShaders/textured.frag","r");

    Shader* colorShader = Shader::loadFromFiles(fireVertFileC,fireFragFileC);
    Shader* texureShader = Shader::loadFromFiles(fireVertFileT,fireFragFileT);

    fclose(fireVertFileC);
    fclose(fireFragFileC);
    fclose(fireVertFileT);
    fclose(fireFragFileT);

    if(colorShader == NULL || texureShader == NULL)
        return EXIT_FAILURE;

    bool isOpened = true;

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

        //*** TODO add other parts of the program

        //Modification and display of the Camp fire in the scene

        glUseProgram(colorShader->getProgramID());
        glBindBuffer(GL_ARRAY_BUFFER,fireBuffer);

        //definition of the vars passed to the shader

        GLint vPosition = glGetAttribLocation(colorShader->getProgramID(),"vPosition");
        GLint vNormals = glGetAttribLocation(colorShader->getProgramID(),"vNormals");
        GLint vUV = glGetAttribLocation(colorShader->getProgramID(),"vUV");
        GLint uColor = glGetUniformLocation(colorShader->getProgramID(),"uColor");

        //definition of the global modification matrix used

        //definition of the projection matrix same for all painted elements

        glm::vec3 cameraPos(-4.f,2.f, 0);
        glm::vec3 cameraTarget(0, 0, 0);
        glm::vec3 cameraUp(1.f, 1.f, 0.f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);

        //inputing the mvp matrix into the shader
        glm::mat4 mvp = proj*view;

        GLint uMVP = glGetUniformLocation(colorShader->getProgramID(), "uMvp");
        glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(mvp));

        //shape transformation matrix to move the different objects

        glm::mat4 shapeTransformationMatrix(1.0f);

        //definition of a variable to store the current reading offset (reusing the one used before for the insertion)
        currentOffset = 0;


        //logs display
        //selection of datas in the VBOS


        glVertexAttribPointer(vPosition,3,GL_FLOAT,GL_FALSE,0,INDICE_TO_PTR(0));
        glEnableVertexAttribArray(vPosition);

        currentOffset += 3 * sizeof(float)* nbVerticesTot;

        glVertexAttribPointer(vNormals,3,GL_FALSE,GL_FLOAT,0,INDICE_TO_PTR((void*)(currentOffset)));
        glEnableVertexAttribArray(vNormals);

        currentOffset += 3 * sizeof(float)* nbVerticesTot;

        glVertexAttribPointer(vUV,2,GL_FALSE,GL_FLOAT,0,INDICE_TO_PTR((void*)(currentOffset)));
        glEnableVertexAttribArray(vUV);

        currentOffset += 2 * sizeof(float)* nbVerticesTot;

        //color affectation :

        glm::vec4 shapeColor(0.34f,0.06f,0.f,1.f);
        glUniform4fv(uColor,1,glm::value_ptr(shapeColor));

        //modification of the transformation matrix to place the logs
        shapeTransformationMatrix = glm::rotate(shapeTransformationMatrix,3.14f/8.f,glm::vec3(1.f,0.f,0.f));
        shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.3f,0.3f,1.5f));
        shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(0.0f,2.f,0.0f));

        //log drawing loop

        glm::mat4 preRotation(1.f);
        for(int i = 0 ;i<9 ; i++ )
        {
            glm::mat4 finalCylinderTranform = preRotation*shapeTransformationMatrix;
            GLint uTransfo = glGetUniformLocation(colorShader->getProgramID(),"uTransfo");
            glUniformMatrix4fv(uTransfo,1,false,glm::value_ptr(finalCylinderTranform));

            //figure draw

            glDrawArrays(GL_TRIANGLES, 0, log.getNbVertices());

            //rotation to draw the next log

            preRotation = glm::rotate(preRotation,(2.f*3.14f/8.f),glm::vec3(0.f,1.f,0.f));
        }

        //end of the log drawing section

        //reinitialisation of the transformation matrix

        shapeTransformationMatrix = glm::mat4(1.f);

        //begin of the rocks drawing section

        //color affectation for the rock (reusage of the precedent one and temporary before texture).

        shapeColor = glm::vec4(0.46f,0.46f,0.46f,1.f);
        glUniform4fv(uColor,1,glm::value_ptr(shapeColor));

        //modification of the transformation matrix to place the rocks
        shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.4f,0.4f,0.4f));
        shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(-2.9f,0.7f,0.f));

        //rocks drawing loop and reset of the pre rotation matrix
        preRotation = glm::mat4(1.f);
        for(int i = 0 ;i<=19 ; i++ )
        {
            glm::mat4 finalCylinderTranform = preRotation*shapeTransformationMatrix;
            GLint uTransfo = glGetUniformLocation(colorShader->getProgramID(),"uTransfo");
            glUniformMatrix4fv(uTransfo,1,false,glm::value_ptr(finalCylinderTranform));

            //figure draw

            glDrawArrays(GL_TRIANGLES, log.getNbVertices(), rock.getNbVertices());

            //rotation to draw the next log

            preRotation = glm::rotate(preRotation,(2.f*3.14f/19.f),glm::vec3(0.f,1.f,0.f));
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);

        //end of the fire drawing section

        //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
    }

    //Free everything
    if(context != NULL)
        SDL_GL_DeleteContext(context);
    if(window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}
