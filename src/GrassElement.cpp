//
// Created by guillaume on 18/02/19.
//

#include <GrassElement.h>

void GrassElement::initBuffer()
{
    glGenBuffers(1,&grassBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,grassBuffer);

    glBufferData(GL_ARRAY_BUFFER, (3*2)*sizeof(float) * grassModel.getNbVertices() , NULL , GL_DYNAMIC_DRAW );

    //insertion of vertices data in the VBO
    glBufferSubData(GL_ARRAY_BUFFER, 0 , 3 * sizeof(float)* grassModel.getNbVertices() , grassModel.getVertices());


    //insertion of normal data in the VBO
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float)* grassModel.getNbVertices() , 3 * sizeof(float)* grassModel.getNbVertices() , grassModel.getNormals());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GrassElement::initShaders()
{
    FILE* fireVertFileC = fopen("./Shaders/GrassShader/colorized.vert","r");
    FILE* fireFragFileC = fopen("./Shaders/GrassShader/colorized.frag","r");

    colorShader =  Shader::loadFromFiles(fireVertFileC,fireFragFileC);

    fclose(fireVertFileC);
    fclose(fireFragFileC);

    if(colorShader == NULL)
    {
        std::cerr << "COULD NOT LOAD SHADERS" << std::endl;
        throw;
    }
}

void GrassElement::draw(Camera const& currentCamera,glm::mat4 globalDiplacementMatrix)
{

}

GrassElement::GrassElement():
grassModel{Cone(50,0.7f)},
color{glm::vec3(0.090f, 0.733f, 0.f)}
{
    initBuffer();
    initShaders();
}