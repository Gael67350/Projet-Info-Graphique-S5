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
    //selection of the texture display program
    glUseProgram(colorShader->getProgramID());
    glBindBuffer(GL_ARRAY_BUFFER,grassBuffer);

    GLint uMVP = glGetUniformLocation(colorShader->getProgramID(), "uMvp");
    GLint vPosition = glGetAttribLocation(colorShader->getProgramID(),"vPosition");
    GLint uColor = glGetUniformLocation(colorShader->getProgramID(), "uColor");
    GLint vNormals = glGetAttribLocation(colorShader->getProgramID(), "vNormals");

    //inserting verticesData in the vbo vertexes
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(0));
    glEnableVertexAttribArray(vPosition);

    //normals
    glVertexAttribPointer(vNormals, 3,GL_FLOAT , GL_FALSE, 0,INDICE_TO_PTR(3 * sizeof(float) * grassModel.getNbVertices()));
    glEnableVertexAttribArray(vNormals);

    //definition and affectation of the transformation matrix

    glm::mat4 uMvpMat = globalDiplacementMatrix;

    uMvpMat = glm::rotate(uMvpMat,3.14f/2.f,glm::vec3(1.0f,0.0f,0.0f));
    uMvpMat = glm::scale(uMvpMat,glm::vec3(0.4f,3.6f,0.4f));

    uMvpMat = currentCamera.lookAt() * uMvpMat;

    glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(uMvpMat));
    glUniform4fv(uColor,1,glm::value_ptr(color));


    //figure draw

    glDrawArrays(GL_TRIANGLES,0, grassModel.getNbVertices());

}

GrassElement::GrassElement():
grassModel{Cone(50,0.0f)},
color{glm::vec4(0.090f, 0.733f, 0.f,1.0f)}
{
    initBuffer();
    initShaders();
}