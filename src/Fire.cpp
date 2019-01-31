#include "Fire.h"

void Fire::initBuffer()
{
    glGenBuffers(1,&fireBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,fireBuffer);

    glBufferData(GL_ARRAY_BUFFER, ((3*2) + 2)*sizeof(float) * nbVerticesTot , NULL , GL_DYNAMIC_DRAW );

    int currentOffset = 0;

    //insertion of the objects data in the buffer

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
}

void Fire::initTextures()
{
    //loading the texture images related with the fire model
    if (!(IMG_Init(IMG_INIT_JPG)&IMG_INIT_JPG))
    {
        ERROR("could not load SDL2_image with JPG files \n");
        throw EXIT_FAILURE;
    }

    SDL_Surface* imgLog = IMG_Load("Ressources/log.jpg");
    SDL_Surface* imgRock = IMG_Load("Ressources/rock.jpg");

    SDL_Surface* logRGB = SDL_ConvertSurfaceFormat(imgLog, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_Surface* rockRGB = SDL_ConvertSurfaceFormat(imgRock, SDL_PIXELFORMAT_RGBA8888, 0);

    SDL_FreeSurface(imgLog);
    SDL_FreeSurface(imgRock);

    //log texture definition
    glGenTextures(1, &logTexture);
    glBindTexture(GL_TEXTURE_2D, logTexture);

    //loading textures

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, logRGB->w, logRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)logRGB->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    //rock texture definition
    glGenTextures(1, &rockTexture);
    glBindTexture(GL_TEXTURE_2D, rockTexture);

    //loading textures

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rockRGB->w, rockRGB->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rockRGB->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void Fire::initShaders()
{
    //two different programs are defined respectively to display colors for the flame
    //and to display textures on the rocks and logs

    FILE* fireVertFileC = fopen("./Shaders/FireShaders/colorized.vert","r");
    FILE* fireFragFileC = fopen("./Shaders/FireShaders/colorized.frag","r");

    FILE* fireVertFileT = fopen("./Shaders/FireShaders/textured.vert","r");
    FILE* fireFragFileT = fopen("./Shaders/FireShaders/textured.frag","r");

    colorShader =  Shader::loadFromFiles(fireVertFileC,fireFragFileC);
    textureShader =  Shader::loadFromFiles(fireVertFileT,fireFragFileT);

    fclose(fireVertFileC);
    fclose(fireFragFileC);
    fclose(fireVertFileT);
    fclose(fireFragFileT);

    if(colorShader == NULL || textureShader == NULL)
    {
        std::cerr << "COULD NOT LOAD SHADERS" << std::endl;
        throw;
    }

}

void Fire::initParticleSystem()
{

    //init flame parameters
    beginPoint = 0.9;
    endPoint = 3.9;
    bigDistance = 0.2;
    smallDistance = 0.4;

    currentWindOffset = 0.0;
    windMaxOffset = 0.03;
    windDirection = true;

    //definition of a vector containing the current size of each fire particle default value is 0.3

    for(int i = 0 ; i<50 ; i++)
    {
        particleSize.push_back(0.3);
    }

    //creation of the particle set composed of two sets of 25 particles in square

    //first particle set

    //middle point
    lateralOffset.push_back(std::make_pair(0.f,0.f));

    //first layer
    lateralOffset.push_back(std::make_pair(0.f,smallDistance));
    lateralOffset.push_back(std::make_pair(smallDistance,0.f));
    lateralOffset.push_back(std::make_pair(smallDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(0.f,-smallDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,0.f));
    lateralOffset.push_back(std::make_pair(-smallDistance,-smallDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(smallDistance,-smallDistance));

    //second layer
    lateralOffset.push_back(std::make_pair(smallDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,0.f));
    lateralOffset.push_back(std::make_pair(bigDistance,-smallDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(smallDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(0.0f,-bigDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,-smallDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,0.f));
    lateralOffset.push_back(std::make_pair(-bigDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(0.0f,bigDistance));

    //second particle set

    //middle point
    lateralOffset.push_back(std::make_pair(0.f,0.f));

    //first layer
    lateralOffset.push_back(std::make_pair(0.f,smallDistance));
    lateralOffset.push_back(std::make_pair(smallDistance,0.f));
    lateralOffset.push_back(std::make_pair(smallDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(0.f,-smallDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,0.f));
    lateralOffset.push_back(std::make_pair(-smallDistance,-smallDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(smallDistance,-smallDistance));

    //second layer
    lateralOffset.push_back(std::make_pair(smallDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,0.f));
    lateralOffset.push_back(std::make_pair(bigDistance,-smallDistance));
    lateralOffset.push_back(std::make_pair(bigDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(smallDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(0.0f,-bigDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,-bigDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,-smallDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,0.f));
    lateralOffset.push_back(std::make_pair(-bigDistance,smallDistance));
    lateralOffset.push_back(std::make_pair(-bigDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(-smallDistance,bigDistance));
    lateralOffset.push_back(std::make_pair(0.0f,bigDistance));

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
    verticalParticleOffset.push_back(2.26);
    verticalParticleOffset.push_back(2.32);
    verticalParticleOffset.push_back(2.38);
    verticalParticleOffset.push_back(2.44);
    verticalParticleOffset.push_back(2.5);
    verticalParticleOffset.push_back(2.56);
    verticalParticleOffset.push_back(2.62);
    verticalParticleOffset.push_back(2.68);
    verticalParticleOffset.push_back(2.74);
    verticalParticleOffset.push_back(2.8);
    verticalParticleOffset.push_back(2.86);
    verticalParticleOffset.push_back(2.92);
    verticalParticleOffset.push_back(2.98);
    verticalParticleOffset.push_back(3.04);
    verticalParticleOffset.push_back(3.1);
    verticalParticleOffset.push_back(3.16);
    verticalParticleOffset.push_back(3.22);
    verticalParticleOffset.push_back(3.28);
    verticalParticleOffset.push_back(3.34);
    verticalParticleOffset.push_back(3.4);
    verticalParticleOffset.push_back(3.46);
    verticalParticleOffset.push_back(3.52);
    verticalParticleOffset.push_back(3.58);
    verticalParticleOffset.push_back(3.64);
    verticalParticleOffset.push_back(3.7);

    //randomisation of the particles order
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::shuffle(verticalParticleOffset.begin(),verticalParticleOffset.end(),std::default_random_engine(seed));

    //addition of each original rotation status for the flame

    for(int i = 0 ; i < 50 ; i++)
        rotationOffset.push_back(0.0);

    //definition of the flame color

    for(int i = 0 ; i < 50 ; i++)
        particlesColor.push_back(glm::vec4(1.0f,1.0f,0.0f,0.1f));

    //definition of the wind offset for the flame

    for(int i = 0 ; i<50 ; i++)
        windOffset.push_back(0.0);

}

void Fire::updateFlame()
{

    //windVariation
    if(windDirection)
    {
        currentWindOffset += 0.0002;
        if(currentWindOffset >= windMaxOffset)
            windDirection = false;
    }
    else
    {
        currentWindOffset -= 0.0002;
        if(currentWindOffset <= 0)
            windDirection = true;
    }

    for(int i = 0 ; i<verticalParticleOffset.size() ; i++)
    {
        //updating position
        if(verticalParticleOffset.at(i) <= endPoint)
            verticalParticleOffset.at(i) += 0.06;
        else
            verticalParticleOffset.at(i) = beginPoint;

        //wind application to the flame
        if(verticalParticleOffset.at(i) > beginPoint)
            windOffset.at(i) += currentWindOffset*(verticalParticleOffset.at(i)/endPoint);
        else
            windOffset.at(i) = 0.0;

        //updating rotation
        rotationOffset.at(i) = 3.14*(verticalParticleOffset.at(i)/endPoint);

        //updating color
        //blue color at the flame's basis
        if(verticalParticleOffset.at(i) < beginPoint+0.1)
            particlesColor.at(i) = glm::vec4(0.f,0.f,1.f,0.65f);
        //clear yellow to orange
        else if(verticalParticleOffset.at(i) < endPoint/1.3)
            particlesColor.at(i) = glm::vec4(1.f,1.f-(verticalParticleOffset.at(i)/(endPoint/1.3)),0.f,0.65f);
        //fading to black then grey for smoke
        else
            particlesColor.at(i) = (1.f+(1.f/3.f)-verticalParticleOffset.at(i)/(endPoint-1.18f)) ?
                                   glm::vec4((1.f+(1.f/3.f)-verticalParticleOffset.at(i)/(endPoint-1.18f)),0.0f,0.0f,0.65f)
                                   : glm::vec4(0.1f,0.1f,0.1f,0.65f);

        //updatingFlameSize
        if (verticalParticleOffset.at(i) < endPoint-0.8)
        {
            particleSize.at(i) = 0.3;
        }
        else
        {
            particleSize.at(i) -= 0.014;
        }

    }
}

Fire::Fire():
        log(Cylinder(50)),
        rock(Sphere(50,50)),
        flameParticle(Cube())
{
    nbVerticesTot = rock.getNbVertices() + flameParticle.getNbVertices() + log.getNbVertices();

    initBuffer();
    initTextures();
    initShaders();
    initParticleSystem();
}

void Fire::draw(Camera const& currentCamera)
{
    //selection of the texture display program
    glUseProgram(textureShader->getProgramID());
    glBindBuffer(GL_ARRAY_BUFFER,fireBuffer);

    //definition of a variable to store the current reading offset (reusing the one used before for the insertion)
    int currentOffset = 0;

    //definition of the vars passed to the shader
    GLint uMVP = glGetUniformLocation(textureShader->getProgramID(), "uMvp");
    GLint uTexture = glGetAttribLocation(textureShader->getProgramID(), "uTexture");
    GLint vPosition = glGetAttribLocation(textureShader->getProgramID(),"vPosition");
    GLint vNormals = glGetAttribLocation(textureShader->getProgramID(), "vNormals");
    GLint vUV = glGetAttribLocation(textureShader->getProgramID(), "vUV");

    //selection of datas in the VBOS
    //vertexes
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(currentOffset));
    glEnableVertexAttribArray(vPosition);

    currentOffset += 3 * sizeof(float) * nbVerticesTot;

    //normals
    glVertexAttribPointer(vNormals, 3,GL_FLOAT , GL_FALSE, 0,INDICE_TO_PTR(currentOffset));
    glEnableVertexAttribArray(vNormals);

    currentOffset += 3 * sizeof(float) * nbVerticesTot;

    //uvDatas
    glVertexAttribPointer(vUV, 2, GL_FLOAT , GL_FALSE , 0, INDICE_TO_PTR(currentOffset));
    glEnableVertexAttribArray(vUV);

    //definition of the global modification matrix used

    //shape transformation matrix to move the different objects

    glm::mat4 shapeTransformationMatrix(1.0f);


    //logs display

    //texture affectation

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, logTexture);
    glUniform1i(uTexture, 0);

    //modification of the transformation matrix to place the logs
    shapeTransformationMatrix = glm::rotate(shapeTransformationMatrix,3.14f/8.f,glm::vec3(1.f,0.f,0.f));
    shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(0.0f,0.9f,0.0f));
    shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.4f,0.4f,1.9f));

    //log drawing loop

    glm::mat4 preRotation(1.f);
    for(int i = 0 ;i<9 ; i++ )
    {
        glm::mat4 finalCylinderTranform = preRotation*shapeTransformationMatrix;
        glm::mat4 uMvpMat = currentCamera.lookAt()*finalCylinderTranform;

        //reafectation of the projection matrix
        glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(uMvpMat));
        //figure draw

        glDrawArrays(GL_TRIANGLES, 0, log.getNbVertices());

        //rotation to draw the next log

        preRotation = glm::rotate(preRotation,(2.f*3.14f/8.f),glm::vec3(0.f,1.f,0.f));
    }

    //end of the log drawing section

    //reinitialisation of the transformation matrix

    shapeTransformationMatrix = glm::mat4(1.f);

    //begin of the rocks drawing section

    //texture affectation for the rock
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glUniform1i(uTexture, 0);

    //modification of the transformation matrix to place the rocks
    shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(-1.5f,0.6f,0.f));
    shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(0.53f,0.53f,0.53f));

    //rocks drawing loop and reset of the pre rotation matrix
    preRotation = glm::mat4(1.f);

    for(int i = 0 ;i<=19 ; i++ )
    {

        glm::mat4 finalRockTranform = preRotation*shapeTransformationMatrix;
        glm::mat4 uMvpMat = currentCamera.lookAt()*finalRockTranform;

        //reafectation of the projection matrix
        glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(uMvpMat));

        //figure draw

        glDrawArrays(GL_TRIANGLES, log.getNbVertices(), rock.getNbVertices());

        //rotation to draw the next log

        preRotation = glm::rotate(preRotation,(2.f*3.14f/19.f),glm::vec3(0.f,1.f,0.f));
    }

    //end of the rock drawing section

    //changing used program to the color display for flame
    glUseProgram(colorShader->getProgramID());

    //definition of a variable to store the current reading offset (reusing the one used before for the insertion)
    currentOffset = 0;

    //redefinition of the vars passed to the shader
    //to bind it on the right program

    vPosition = glGetAttribLocation(colorShader->getProgramID(), "vPosition");
    vNormals = glGetAttribLocation(colorShader->getProgramID(), "vNormals");
    vUV = glGetAttribLocation(colorShader->getProgramID(), "vUV");
    uMVP = glGetUniformLocation(colorShader->getProgramID(), "uMvp");

    //re-selection of datas in the VBOS
    //vertexes
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(currentOffset));
    glEnableVertexAttribArray(vPosition);

    currentOffset += 3 * sizeof(float) * nbVerticesTot;

    //normals
    glVertexAttribPointer(vNormals, 3,GL_FLOAT , GL_FALSE, 0,INDICE_TO_PTR(currentOffset));
    glEnableVertexAttribArray(vNormals);

    currentOffset += 3 * sizeof(float) * nbVerticesTot;

    //uvDatas
    glVertexAttribPointer(vUV, 2, GL_FLOAT , GL_FALSE , 0, INDICE_TO_PTR(currentOffset));
    glEnableVertexAttribArray(vUV);

    //definition of new parameter which were not in the texture shader
    GLint uColor = glGetUniformLocation(colorShader->getProgramID(), "uColor");

    //begin of the flame drawing section

    for(int i = 0 ;i<verticalParticleOffset.size() ; i++ )
    {
        //color selection
        glUniform4fv(uColor,1,glm::value_ptr(particlesColor.at(i)));

        //reinitialisation of the transformation matrix and adaptation to the current particle

        shapeTransformationMatrix = glm::mat4(1.f);

        //lateral offset
        shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(lateralOffset.at(i).first,0.f,lateralOffset.at(i).second));
        //addition of the vertical and wind offset
        shapeTransformationMatrix = glm::translate(shapeTransformationMatrix,glm::vec3(0.f,verticalParticleOffset.at(i),windOffset.at(i)));
        //setting the current flame particle size
        shapeTransformationMatrix = glm::scale(shapeTransformationMatrix,glm::vec3(particleSize.at(i),particleSize.at(i),particleSize.at(i)));
        //affectation of the rotation to the flame
        shapeTransformationMatrix = glm::rotate(shapeTransformationMatrix,rotationOffset.at(i),glm::vec3(0.0f,1.0f,0.0f));

        glm::mat4 uMvpMat = currentCamera.lookAt()*shapeTransformationMatrix;

        //reafectation of the projection matrix
        glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(uMvpMat));

        //figure draw

        glDrawArrays(GL_TRIANGLES, log.getNbVertices()+rock.getNbVertices() , flameParticle.getNbVertices());

        //rotation to draw the next log
    }

    //end of the flame drawing section

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    //end of the fire drawing section

    updateFlame();
}