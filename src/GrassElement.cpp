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

void GrassElement::draw(Camera const& currentCamera,glm::vec3 const& displacementVector,std::tuple<glm::vec3, float, float, glm::vec3> lightData)
{
    //selection of the texture display program
    glUseProgram(colorShader->getProgramID());
    glBindBuffer(GL_ARRAY_BUFFER,grassBuffer);

    GLint uMVP = glGetUniformLocation(colorShader->getProgramID(), "uMvp");

    GLint vPosition = glGetAttribLocation(colorShader->getProgramID(),"vPosition");
    GLint uColor = glGetUniformLocation(colorShader->getProgramID(), "uColor");
    GLint vNormals = glGetAttribLocation(colorShader->getProgramID(), "vNormals");

    //light fire datas
    GLint uAmbientIntensity = glGetUniformLocation(colorShader->getProgramID(),"uAmbientIntensity");
    GLint uLightColor = glGetUniformLocation(colorShader->getProgramID(),"uLightColor");
    GLint uLightIntesity = glGetUniformLocation(colorShader->getProgramID(),"uLightIntensity");

    //data related with diffuse light

    GLint uWorldProj = glGetUniformLocation(colorShader->getProgramID(), "uWorldProj");
    GLint uWorldProjInv = glGetUniformLocation(colorShader->getProgramID(), "uWorldProjInv");

    GLint uLightPosition = glGetUniformLocation(colorShader->getProgramID(),"uLightPosition");

    //definition of the parameters related to specular reflection

    GLint uCameraPosition = glGetUniformLocation(colorShader->getProgramID(),"uCameraPosition");
    GLint specularConstant = glGetUniformLocation(colorShader->getProgramID(),"specularConstant");
    GLint shine = glGetUniformLocation(colorShader->getProgramID(),"shine");

    //affecting shader variables with the data from light defined in class
    glUniform1f(uAmbientIntensity,std::get<2>(lightData));
    glUniform3fv(uLightColor,1,glm::value_ptr(std::get<3>(lightData)));
    glUniform1f(uLightIntesity,std::get<1>(lightData));

    glUniform3fv(uLightPosition,1,glm::value_ptr(std::get<0>(lightData)));

    glm::vec3 camPos = currentCamera.getPosition();

    glUniform3fv(uCameraPosition,1,glm::value_ptr(camPos));

    //inserting verticesData in the vbo vertexes
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(0));
    glEnableVertexAttribArray(vPosition);

    //normals
    glVertexAttribPointer(vNormals, 3,GL_FLOAT , GL_FALSE, 0,INDICE_TO_PTR(3 * sizeof(float) * grassModel.getNbVertices()));
    glEnableVertexAttribArray(vNormals);

    //definition and affectation of the transformation matrix

    glm::mat4 uMvpMat(1.f);
    glm::mat4 globalPlacementMatrix(1.f);

    uMvpMat = glm::scale(uMvpMat,glm::vec3(0.25f,1.6f,0.25f));
    uMvpMat = glm::rotate(uMvpMat,-3.14f/2.f,glm::vec3(1.0f,0.0f,0.0f));

    //invert scale of the matrix to compensate the model's shink
    globalPlacementMatrix = glm::translate(globalPlacementMatrix,glm::vec3(-displacementVector.x*3.33,-12.5*0.8,displacementVector.z*3.33));

    glm::mat4 finalTransform = globalPlacementMatrix * uMvpMat;
    uMvpMat = currentCamera.lookAt() * finalTransform;

    //affecting specular variables specific to the log material
    glUniform1f(specularConstant,0.9f);
    glUniform1f(shine,0.1f);


    glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(uMvpMat));

    //passing the light render related matrix to the shader

    glUniformMatrix4fv(uWorldProj , 1 , false , glm::value_ptr(currentCamera.getViewMatrix()*finalTransform));
    glUniformMatrix4fv(uWorldProjInv , 1 , true , glm::value_ptr(glm::inverse(currentCamera.getViewMatrix()* finalTransform)));


    glUniform4fv(uColor,1,glm::value_ptr(color));

    //figure draw
    glDrawArrays(GL_TRIANGLES,0, grassModel.getNbVertices());

}

GrassElement::GrassElement():
grassModel{Cone(50,0.0f)},
color{glm::vec4(0.176f, 0.584f, 0.f,1.0f)}
{
    initBuffer();
    initShaders();
}