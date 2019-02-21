//
// Created by guillaume on 27/01/19.
//

#ifndef GRAPHICS_SQUELETTE_FIRE_H
#define GRAPHICS_SQUELETTE_FIRE_H

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

//geometric shapes imports
#include "Cube.h"
#include "Cylinder.h"
#include "Sphere.h"

#include "Camera.h"

//other native c++ library used in the program
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <tuple>

#define INDICE_TO_PTR(x) ((void*)(x))

class Fire
{
    private:

        //definition of shapes composing the fire
            Cylinder log ;
            Sphere rock ;
            Cube flameParticle ;

        //definition of the different buffers
            GLuint fireBuffer;

        //texture data used in the model
            GLuint logTexture;
            GLuint rockTexture;

        //shaderFiles
            Shader* colorShader;
            Shader* textureShader;

        //------- Variables related with the particle system ---------
        //definition of variables for particles parameters
            float beginPoint;
            float endPoint;
            float bigDistance;
            float smallDistance;

        //definition of the variable aossciated with wind simulation
            float windMaxOffset;
            float currentWindOffset;
            bool windDirection;

        //definition of a vector containing the current size of each fire particle default value is 0.3
            std::vector<float> particleSize ;

        //definition of the lateral offset of the fire particles
            std::vector<std::pair<float,float>> lateralOffset;

        //definition of the vertical offset of each particles
            std::vector<float> verticalParticleOffset ;

        //definition of each particles rotation
            std::vector<float> rotationOffset;

        //definition of the flame color
            std::vector<glm::vec4> particlesColor;

        //definition of the wind offset for the flame
            std::vector<float> windOffset;

        //definition of the global placement matrix
            glm::mat4 globalPlacementMatrix;

        //useful data
            uint32_t nbVerticesTot;

        //variables used for the light simulation
            glm::vec3 lightPosition;
            float lightIntensity;
            float ambientIntensity;
            glm::vec3 lightColor;
            int fireUpdateCount;

        //private separated functions for clarity
            void initBuffer();
            void initShaders();
            void initTextures();
            void initParticleSystem();

        void updateFlame();


    public:
        Fire(glm::mat4 placementMatrix);
        ~Fire();
        void draw(Camera const& currentCamera);

        std::tuple<glm::vec3,float,float,glm::vec3> getLightInfo()
        {
            return std::tuple<glm::vec3,float,float,glm::vec3>(lightPosition,lightIntensity,ambientIntensity,lightColor);
        }

};

#endif //GRAPHICS_SQUELETTE_FIRE_H
