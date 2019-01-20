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

//other native c++ library used in the program
#include<vector>

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

    //definition of the lateral offset
    std::vector<std::pair<float,float>> lateralOffset = std::vector<std::pair<float,float>>();

    //middle point
    lateralOffset.push_back(std::make_pair(0.f,0.f));

    //first layer
    lateralOffset.push_back(std::make_pair(0.f,0.3f));
    lateralOffset.push_back(std::make_pair(0.3f,0.f));
    lateralOffset.push_back(std::make_pair(0.3f,0.3f));
    lateralOffset.push_back(std::make_pair(0.f,-0.3f));
    lateralOffset.push_back(std::make_pair(-0.3f,0.f));
    lateralOffset.push_back(std::make_pair(-0.3f,-0.3f));
    lateralOffset.push_back(std::make_pair(-0.3f,0.3f));
    lateralOffset.push_back(std::make_pair(0.3f,-0.3f));

    //second layer
    lateralOffset.push_back(std::make_pair(0.3f,0.6f));
    lateralOffset.push_back(std::make_pair(0.6f,0.6f));
    lateralOffset.push_back(std::make_pair(0.6f,0.3f));
    lateralOffset.push_back(std::make_pair(0.6f,0.f));
    lateralOffset.push_back(std::make_pair(0.6f,-0.3f));
    lateralOffset.push_back(std::make_pair(0.6f,-0.6f));
    lateralOffset.push_back(std::make_pair(0.3f,-0.6f));
    lateralOffset.push_back(std::make_pair(0.0f,-0.6f));
    lateralOffset.push_back(std::make_pair(-0.3f,-0.6f));
    lateralOffset.push_back(std::make_pair(-0.6f,-0.6f));
    lateralOffset.push_back(std::make_pair(-0.6f,-0.3f));
    lateralOffset.push_back(std::make_pair(-0.6f,0.f));
    lateralOffset.push_back(std::make_pair(-0.6f,0.3f));
    lateralOffset.push_back(std::make_pair(-0.6f,0.6f));
    lateralOffset.push_back(std::make_pair(-0.3f,0.6f));
    lateralOffset.push_back(std::make_pair(0.0f,0.6f));

    //definition of the vertical offset of each particles
    std::vector<float> verticalParticleOffset = std::vector<float>() ;

    //addition of the departure point of each particle

    verticalParticleOffset.push_back(0.7);
    verticalParticleOffset.push_back(0.76);
    verticalParticleOffset.push_back(0.82);
    verticalParticleOffset.push_back(0.88);
    verticalParticleOffset.push_back(0.94);
    verticalParticleOffset.push_back(1.06);
    verticalParticleOffset.push_back(1.12);
    verticalParticleOffset.push_back(1.18);
    verticalParticleOffset.push_back(1.24);
    verticalParticleOffset.push_back(1.3);
    verticalParticleOffset.push_back(1.36);
    verticalParticleOffset.push_back(1.42);
    verticalParticleOffset.push_back(1.48);
    verticalParticleOffset.push_back(1.54);
    verticalParticleOffset.push_back(1.6);
    verticalParticleOffset.push_back(1.66);
    verticalParticleOffset.push_back(1.72);
    verticalParticleOffset.push_back(1.78);
    verticalParticleOffset.push_back(1.84);
    verticalParticleOffset.push_back(1.9);
    verticalParticleOffset.push_back(1.96);
    verticalParticleOffset.push_back(2.02);
    verticalParticleOffset.push_back(2.08);
    verticalParticleOffset.push_back(2.14);
    verticalParticleOffset.push_back(2.2);

    //definition of each particles rotation
    std::vector<float> rotationOffset = std::vector<float>();

    //addition of each original rotation status for the flame

    for(int i = 0 ; i < 25 ; i++)
        rotationOffset.push_back(0.0);

    //definition of the flame color

    std::vector<glm::vec4> particlesColor = std::vector<glm::vec4>();

    for(int i = 0 ; i < 25 ; i++)
        particlesColor.push_back(glm::vec4(1.0f,1.0f,0.0f,0.1f));

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

        glm::vec3 cameraPos(-4.f,3.f, 0);
        glm::vec3 cameraTarget(0, 0, 0);
        glm::vec3 cameraUp(1.f, 1.f, 0.f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(60.f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);

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
        shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(0.0f,0.75f,0.0f));
        shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.3f,0.3f,1.5f));

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
        shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(-1.17f,0.45f,0.f));
        shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.4f,0.4f,0.4f));

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

        //end of the rock drawing section

        //begin of the flame drawing section

        for(int i = 0 ;i<25 ; i++ )
        {
            //color selection
            glUniform4fv(uColor,1,glm::value_ptr(particlesColor.at(i)));

            //reinitialisation of the transformation matrix and adaptation to the current particle

            shapeTransformationMatrix = glm::mat4(1.f);

            shapeTransformationMatrix = glm::rotate(shapeTransformationMatrix,rotationOffset.at(0),glm::vec3(0.0f,1.0f,0.0f));

            //lateral offset
            shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(lateralOffset.at(i).first,0.f,lateralOffset.at(i).second));
            //addition of the vertical offset
            shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(0.f,verticalParticleOffset.at(i),0.f));

            shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.25f,0.25f,0.25f));

            GLint uTransfo = glGetUniformLocation(colorShader->getProgramID(),"uTransfo");
            glUniformMatrix4fv(uTransfo,1,false,glm::value_ptr(shapeTransformationMatrix));

            //figure draw

            glDrawArrays(GL_TRIANGLES, log.getNbVertices()+rock.getNbVertices() , flameParticle.getNbVertices());

            //rotation to draw the next log
        }

        //update of vertical position and rotation status for each fire particles

        for(int i = 0 ; i<verticalParticleOffset.size() ; i++)
        {
            //updating position

            if(verticalParticleOffset.at(i) < 2.2)
                verticalParticleOffset.at(i) += 0.045;
            else
                verticalParticleOffset.at(i) = 0.7;

            //updating rotation
            rotationOffset.at(i) =  2*(verticalParticleOffset.at(i)-0.7);

            //updating color
            if(((verticalParticleOffset.at(i)/2)-1.1)>0)
                particlesColor.at(i) = glm::vec4((2.2f - (verticalParticleOffset.at(i)/2)-1.1) ,1.1f-(verticalParticleOffset.at(i)/2),0.f,0.1f);
            else
                particlesColor.at(i) = glm::vec4(1.0f,1.1f-(verticalParticleOffset.at(i)/2),0.f,0.1f);

        }

        //end of the flame drawing section

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
