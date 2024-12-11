#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include <utils/scenedata.h>
#include <utils/sceneparser.h>
#include <utils/shaderloader.h>
#include <camera/camera.h>
#include <glm/gtx/transform.hpp>

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
    m_camera = Camera();
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);
    glDeleteBuffers(1, &m_sphere_vbo);

    // for extra credit Mesh Rendering
    glDeleteVertexArrays(1, &m_mesh_dragon_vao);
    glDeleteBuffers(1, &m_mesh_dragon_vbo);
    glDeleteVertexArrays(1, &m_mesh_bunny_vao);
    glDeleteBuffers(1, &m_mesh_bunny_vbo);

    delete m_sphere;
    delete m_cube;
    delete m_mesh_dragon;
    delete m_mesh_bunny;

    // Project 6 action
    glDeleteProgram(m_texture_shader);
    glDeleteTextures(1,&m_fbo_texture);
    glDeleteRenderbuffers(1,&m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteBuffers(1,&m_fullscreen_vbo);
    glDeleteVertexArrays(1,&m_fullscreen_vao);

    for (const RenderShapeData& shape : m_metaData.shapes){
        if (shape.primitive.material.textureMap.isUsed){
            glDeleteTextures(1,&shape.primitive.material.textureMap.textureID);
        }
    }

    //Project 6 extra credit shadow
    glDeleteProgram(m_shadow_shader);
    glDeleteFramebuffers(1, &m_shadowMap.depthMapFBO);
    glDeleteTextures(1, &m_shadowMap.depthMap);

    // Final Project
    glDeleteTextures(1,&m_cube_texture);
    glDeleteTextures(1,&m_mainCha_texture);
    glDeleteTextures(1, &m_background_texture);
    m_allObjects.clear();
    this->doneCurrent();
}

void Realtime::bindTexture(GLuint& textureID, QImage* image){
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width(),
                 image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,image->bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
}


void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();
    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;
    glClearColor(0,0,0,1);
    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram("resources/shaders/default.vert", "resources/shaders/default.frag");
    m_cube = new Cube();
    m_sphere = new Sphere();

    glGenVertexArrays(1,&m_cube_vao);
    glGenBuffers(1, &m_cube_vbo);
    glGenVertexArrays(1,&m_sphere_vao);
    glGenBuffers(1, &m_sphere_vbo);
    updateVaoVbo(1, 1);

    // for extra credit mesh rendering
    m_mesh_dragon = new Mesh();
    m_mesh_bunny = new Mesh();
    m_mesh_dragon->loadOBJ("asset/dragon.obj");
    m_mesh_bunny->loadOBJ("asset/bunny.obj");
    glGenVertexArrays(1,&m_mesh_dragon_vao);
    glGenBuffers(1,&m_mesh_dragon_vbo);
    glBindVertexArray(m_mesh_dragon_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_mesh_dragon_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_mesh_dragon->getVertexData().size() * sizeof(float),m_mesh_dragon->getVertexData().data(), GL_STATIC_DRAW);
    setVAO();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1,&m_mesh_bunny_vao);
    glGenBuffers(1,&m_mesh_bunny_vbo);
    glBindVertexArray(m_mesh_bunny_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_mesh_bunny_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_mesh_bunny->getVertexData().size() * sizeof(float),m_mesh_bunny->getVertexData().data(), GL_STATIC_DRAW);
    setVAO();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // project 6 action
    // set the screen FBO, set postprocess parameters
    screenPostproSetup();

    // Final Project
    std::string filepathString = "asset/bark.png";
    QString filepath = QString(filepathString.c_str());
    std::cout << "update texture " << filepath.toStdString() << std::endl;
    m_cube_texture_image = QImage(filepath).convertToFormat(QImage::Format_RGBA8888).mirrored();
    bindTexture(m_cube_texture, &m_cube_texture_image);

    std::string filepathString2 = "asset/mainCha.png";
    QString filepath2 = QString(filepathString2.c_str());
    std::cout << "update texture " << filepath2.toStdString() << std::endl;
    m_mainCha_texture_image = QImage(filepath2).convertToFormat(QImage::Format_RGBA8888).mirrored();
    bindTexture(m_mainCha_texture, &m_mainCha_texture_image);

    std::string filepathString3 = "asset/star.png";
    QString filepath3 = QString(filepathString3.c_str());
    std::cout << "update texture " << filepath3.toStdString() << std::endl;
    m_background_texture_image = QImage(filepath3).convertToFormat(QImage::Format_RGBA8888).mirrored();
    bindTexture(m_background_texture, &m_background_texture_image);

    std::string filepathString4 = "asset/portal.png";
    QString filepath4 = QString(filepathString4.c_str());
    std::cout << "update texture " << filepath4.toStdString() << std::endl;
    m_portal_texture_image = QImage(filepath4).convertToFormat(QImage::Format_RGBA8888).mirrored();
    bindTexture(m_portal_texture, &m_portal_texture_image);

    createMap();
    createBackground();
    createDragon();
    createBunny();
    createPortal1();
    createPortal2();
    createMainCharacter();
    // objectType 0: cube map; 1: main character; 2: background; 3: dragon; 4: bunny; 5: portal1; 6: portal2
    for (size_t i = 0; i < m_allObjects.size(); i++){
        if (m_allObjects[i].objectType == 1) mainChaIndex = i;
        if (m_allObjects[i].objectType == 3) dragonIndex = i;
        if (m_allObjects[i].objectType == 4) bunnyIndex = i;
        if (m_allObjects[i].objectType == 5) portal1Index = i;
        if (m_allObjects[i].objectType == 6) portal2Index = i;
    }

    // Project 6 extra credit shadow
    m_shadow_shader = ShaderLoader::createShaderProgram(":/resources/shaders/shadow.vert", ":/resources/shaders/shadow.frag");
    makeShadowFBO();

    // Debug shadow
    m_debug_shader = ShaderLoader::createShaderProgram(
        ":/resources/shaders/debug.vert",
        ":/resources/shaders/debug.frag"
        );
    initDebugQuad();
}

void Realtime::paintBasicMap(){
    // added in paintGL, when m_shader is used;
    for (basicMapFile& oneCube : m_allObjects){
        GLint modelLoc = glGetUniformLocation(m_shader, "modelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &oneCube.modelMatrix[0][0]);
        GLint inverseModelLoc = glGetUniformLocation(m_shader, "inverseModelMatrix");
        glUniformMatrix4fv(inverseModelLoc, 1, GL_FALSE, &oneCube.inverseModelMatrix[0][0]);

        const SceneMaterial& material = oneCube.material;
        GLint ambientLoc = glGetUniformLocation(m_shader, "material.ambient");
        GLint diffuseLoc = glGetUniformLocation(m_shader, "material.diffuse");
        GLint specularLoc = glGetUniformLocation(m_shader, "material.specular");
        GLint shininessLoc = glGetUniformLocation(m_shader, "material.shininess");
        glUniform3fv(ambientLoc, 1, &material.cAmbient[0]);
        glUniform3fv(diffuseLoc, 1, &material.cDiffuse[0]);
        glUniform3fv(specularLoc, 1, &material.cSpecular[0]);
        glUniform1f(shininessLoc, material.shininess);
        GLint isTextureLoc = glGetUniformLocation(m_shader,"isTexture");
        glUniform1i(isTextureLoc, 1);
        GLint blendLoc = glGetUniformLocation(m_shader, "blend");
        glUniform1f(blendLoc, material.blend);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, oneCube.textureID);
        GLint samplerLoc = glGetUniformLocation(m_shader, "textureSampler");
        glUniform1i(samplerLoc, 0);
        if (oneCube.objectType == 0 || oneCube.objectType == 2 || oneCube.objectType == 5 || oneCube.objectType == 6){
            glBindVertexArray(oneCube.vao);
            glDrawArrays(GL_TRIANGLES,0,m_cube->generateShape().size()/8);
        } else if (oneCube.objectType == 1){
            glBindVertexArray(oneCube.vao);
            glDrawArrays(GL_TRIANGLES,0,m_sphere->generateShape().size()/8);
        } else if (oneCube.objectType == 3) {
            glBindVertexArray(oneCube.vao);
            glDrawArrays(GL_TRIANGLES,0,m_mesh_dragon->getVertexData().size()/8);
        } else if (oneCube.objectType == 4) {
            glBindVertexArray(oneCube.vao);
            glDrawArrays(GL_TRIANGLES,0,m_mesh_bunny->getVertexData().size()/8);
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    // Project 6 extra credit shadow mapping, bind the shadowfbo, lights to depthMap;
    renderShadowMap();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);
    // Final Project
    paintBasicMap();

    // Original Drawing for Project 5 and 6
    // paintOriginal();

    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintTexture(m_fbo_texture);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    // Students: anything requiring OpenGL calls when the program starts should be done here
    // update projection matrix
    m_camera.updateWH(w,h);
    glUseProgram(m_shader);
    glm::mat4 projectionMatrix = m_camera.getProjectionMatrix();
    GLint projLoc = glGetUniformLocation(m_shader, "projMatrix");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
    glUseProgram(0);
    glDeleteTextures(1,&m_fbo_texture);
    glDeleteRenderbuffers(1,&m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    makeFBO();
}

void Realtime::sceneChanged() {
    makeCurrent();
    m_mainChaX = 0;
    m_mainChaY = 0;
    m_mainChaZ = 0;
    bunnyTrigger = false;
    dragonTrigger = false;
    m_allObjects[dragonIndex].textureID = m_background_texture;
    m_allObjects[bunnyIndex].textureID = m_background_texture;
    m_gravity = -9.8f;
    m_mainChaSpeedHorizontal = 5.0f;
    m_metaData = RenderData();
    bool success = SceneParser::parse(settings.sceneFilePath, m_metaData);
    if (!success) std::cout << "parsing failed";
    m_camera = Camera(m_metaData.cameraData,width(),height());
    m_camera.updateNearFar(0.1f,100.0f);
    glUseProgram(m_shader);
    glm::mat4 viewMatrix = m_camera.getViewMatrix();
    glm::mat4 projectionMatrix = m_camera.getProjectionMatrix();
    GLint viewLoc = glGetUniformLocation(m_shader, "viewMatrix");
    GLint projLoc = glGetUniformLocation(m_shader, "projMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

    glm::vec3 camPos = m_camera.getCameraPos();
    GLint camPosLoc = glGetUniformLocation(m_shader, "camPos");
    glUniform3fv(camPosLoc, 1, &camPos[0]);

    GLint kaLoc = glGetUniformLocation(m_shader, "globalData.ka");
    GLint kdLoc = glGetUniformLocation(m_shader, "globalData.kd");
    GLint ksLoc = glGetUniformLocation(m_shader, "globalData.ks");
    GLint ktLoc = glGetUniformLocation(m_shader, "globalData.kt");
    glUniform1f(kaLoc,m_metaData.globalData.ka);
    glUniform1f(kdLoc,m_metaData.globalData.kd);
    glUniform1f(ksLoc,m_metaData.globalData.ks);
    glUniform1f(ktLoc,m_metaData.globalData.kt);

    const int maxLightNumber = 8;
    std::vector<SceneLightData> allLights = m_metaData.lights;
    int numLights = std::min(static_cast<int>(allLights.size()), maxLightNumber);
    GLint numLightsLoc = glGetUniformLocation(m_shader, "numLights");
    glUniform1i(numLightsLoc, numLights);
    // GLint numLightsVLoc = glGetUniformLocation(m_shader, "numLightsV");
    // glUniform1i(numLightsVLoc, numLights);

    for (int i = 0; i < numLights; ++i) {
        const SceneLightData& light = allLights[i];
        GLint typeLoc = glGetUniformLocation(m_shader, ("lightType[" + std::to_string(i) + "]").c_str());
        int lightType;
        if (light.type == LightType::LIGHT_DIRECTIONAL) lightType = 0;
        if (light.type == LightType::LIGHT_POINT) lightType = 1;
        if (light.type == LightType::LIGHT_SPOT) lightType = 2;
        glUniform1i(typeLoc, lightType);
        GLint dirLoc = glGetUniformLocation(m_shader, ("lightDirection[" + std::to_string(i) + "]").c_str());
        glUniform3f(dirLoc, light.dir[0],light.dir[1], light.dir[2]);
        GLint colorLoc = glGetUniformLocation(m_shader, ("lightColor[" + std::to_string(i) + "]").c_str());
        glUniform3f(colorLoc, light.color[0], light.color[1], light.color[2]);
        GLint functionLoc = glGetUniformLocation(m_shader, ("lightFunction[" + std::to_string(i) + "]").c_str());
        glUniform3f(functionLoc, light.function[0],light.function[1], light.function[2]);
        GLint posLoc = glGetUniformLocation(m_shader, ("lightPos[" + std::to_string(i) + "]").c_str());
        glUniform3f(posLoc, light.pos[0],light.pos[1], light.pos[2]);
        GLint angleLoc = glGetUniformLocation(m_shader, ("lightAngle[" + std::to_string(i) + "]").c_str());
        glUniform1f(angleLoc, light.angle);
        GLint penumbraLoc = glGetUniformLocation(m_shader, ("lightPenumbra[" + std::to_string(i) + "]").c_str());
        glUniform1f(penumbraLoc, light.penumbra);
    }
    glUseProgram(0);
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    makeCurrent();
    if (settings.toggle1) {
        m_postprocess = 1;
    }
    if (settings.toggle2) {
        m_postprocess = 2;
    }
    if (!settings.toggle1 && !settings.toggle2){
        m_postprocess = 0;
    }
    if (m_previous_postprocess != m_postprocess){
        m_previous_postprocess = m_postprocess;
    }
    update(); // asks for a PaintGL() call to occur
}


// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
    if (event->key() == Qt::Key_G && !m_mainChaJumping) {
        m_mainChaJumping = true;
        m_mainChaSpeedVertical = 5.0f; // Initial upward velocity (adjust as needed)
    }
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_camera.updateRotation(deltaX,deltaY);
        glUseProgram(m_shader);
        glm::mat4 viewMatrix = m_camera.getViewMatrix();
        GLint viewLoc = glGetUniformLocation(m_shader, "viewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
        glUseProgram(0);
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float movementSpeed = 5.0f;
    glm::vec3 movement(0.0f);
    glm::vec3 look = m_camera.getLook();
    glm::vec3 right = m_camera.getRight();
    if (m_keyMap[Qt::Key_W]) movement += look * movementSpeed * deltaTime;
    if (m_keyMap[Qt::Key_S]) movement -= look * movementSpeed * deltaTime;
    if (m_keyMap[Qt::Key_A]) movement -= right * movementSpeed * deltaTime;
    if (m_keyMap[Qt::Key_D]) movement += right * movementSpeed * deltaTime;
    if (m_keyMap[Qt::Key_Space]) movement += glm::vec3(0.f,1.f,0.f) * movementSpeed * deltaTime;
    if (m_keyMap[Qt::Key_Control]){
        movement += glm::vec3(0.f,-1.f,0.f) * movementSpeed * deltaTime;
        // std::cout<<m_camera.getCameraPos().x<<"===="<<m_camera.getCameraPos().y<<"==="<<m_camera.getCameraPos().z<<"===";
    }
    m_camera.updateTranslation(movement);
    glUseProgram(m_shader);
    glm::mat4 viewMatrix = m_camera.getViewMatrix();
    GLint viewLoc = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUseProgram(0);
    if (m_allObjects.size() != 0) {
        if (bunnyTrigger) m_gravity = -4.9f;
        if (dragonTrigger) m_mainChaSpeedHorizontal = 7.5f;
        auto& mainCha = m_allObjects[mainChaIndex];

        // Handle horizontal movement
        glm::vec3 horizontalMovement(0.0f);
        if (m_keyMap[Qt::Key_I]) horizontalMovement.z += 1.0f; // Forward
        if (m_keyMap[Qt::Key_K]) horizontalMovement.z -= 1.0f; // Backward
        if (m_keyMap[Qt::Key_J]) horizontalMovement.x += 1.0f; // Left
        if (m_keyMap[Qt::Key_L]) horizontalMovement.x -= 1.0f; // Right

        // Normalize and scale horizontal movement
        if (glm::length(horizontalMovement) > 0.0f) {
            horizontalMovement = glm::normalize(horizontalMovement);
            horizontalMovement *= m_mainChaSpeedHorizontal * deltaTime;
        }

        // Try horizontal movement
        float newX = m_mainChaX + horizontalMovement.x;
        float newZ = m_mainChaZ + horizontalMovement.z;

        // Check X collision
        float worldY = m_mainChaY + 0.76f;
        glm::vec3 newPositionCheckX(newX, worldY, m_mainChaZ);
        if (!checkHorizontalCollision(newPositionCheckX, 0.25f)) {
            m_mainChaX = newX;
        }

        // Check Z collision
        glm::vec3 newPositionCheckZ(m_mainChaX, worldY, newZ);
        if (!checkHorizontalCollision(newPositionCheckZ, 0.25f)) {
            m_mainChaZ = newZ;
        }

        // Handle vertical movement (gravity and jumping)
        if (m_mainChaJumping) {
            m_mainChaSpeedVertical += m_gravity * deltaTime;
        }
        m_mainChaY += m_mainChaSpeedVertical * deltaTime;

        // Check ground collision
        glm::vec3 newPosition(m_mainChaX, m_mainChaY + 0.76f, m_mainChaZ);
        if (isOnGround(newPosition, 0.28f)) {
            if (m_mainChaJumping) {
                m_mainChaJumping = false;
            }
            m_mainChaSpeedVertical = 0.0f;
        } else if (!m_mainChaJumping) {
            m_mainChaSpeedVertical = m_gravity * deltaTime;
            m_mainChaJumping = true;
        }

        //std::cout << m_mainChaX << " " << m_mainChaY << " " << m_mainChaZ << std::endl;
        // Update the sphere's transformation matrix
        if (m_mainChaY >= 7.25 && m_mainChaY <= 8.25 &&
            m_mainChaZ >= 2.5 && m_mainChaZ <= 3.5 &&
            m_mainChaX >= 4.5){
            m_allObjects[dragonIndex].textureID = m_cube_texture;
            dragonTrigger = true;
            m_mainChaX = 0;
            m_mainChaY = 0;
            m_mainChaZ = 0;
        }
        if (m_mainChaY >= 7.25 && m_mainChaY <= 8.25 &&
            m_mainChaX >= 2.5 && m_mainChaX <= 3.5 &&
            m_mainChaZ >= 4.5){
            m_allObjects[bunnyIndex].textureID = m_cube_texture;
            bunnyTrigger = true;
            m_mainChaX = 0;
            m_mainChaY = 0;
            m_mainChaZ = 0;
        }
        if (m_mainChaY < 0) {
            m_mainChaX = 0;
            m_mainChaY = 0;
            m_mainChaZ = 0;
        }

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(m_mainChaX, m_mainChaY, m_mainChaZ));
        mainCha.modelMatrix = translation * m_mainChaOriginalCTM;
        mainCha.inverseModelMatrix = glm::inverse(mainCha.modelMatrix);
    }
    update(); // asks for a PaintGL() call to occur
}

void Realtime::initDebugQuad() {
    // Create a simple quad for debugging
    float quadVertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_debug_quad_vao);
    glGenBuffers(1, &m_debug_quad_vbo);
    glBindVertexArray(m_debug_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_debug_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Realtime::renderDebugQuad() {
    glBindVertexArray(m_debug_quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
