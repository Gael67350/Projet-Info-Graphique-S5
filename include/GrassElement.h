//
// Created by guillaume on 18/02/19.
//

#ifndef GRAPHICS_SQUELETTE_GRASSELEMENT_H
#define GRAPHICS_SQUELETTE_GRASSELEMENT_H

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

#include "Geometry.h"
#include "Cone.h"
#include "Camera.h"

#include<tuple>

#define INDICE_TO_PTR(x) ((void*)(x))

class GrassElement {

private:
    Cone grassModel;
    glm::vec4 color;

    GLuint grassBuffer;

    Shader *colorShader;

    void initBuffer();

    void initShaders();

public:
    GrassElement();

    void draw(Camera const &currentCamera, glm::vec3 const &displacementVector,std::tuple<glm::vec3, float, float, glm::vec3> lightData);
};

#endif //GRAPHICS_SQUELETTE_GRASSELEMENT_H
