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
    glDeleteVertexArrays(1, &m_cone_vao);
    glDeleteBuffers(1, &m_cone_vbo);
    glDeleteVertexArrays(1, &m_cylinder_vao);
    glDeleteBuffers(1, &m_cylinder_vbo);
    glDeleteProgram(m_shader);

    // for extra credit Adaptive level of detail
    glDeleteVertexArrays(1, &m_cube_vao_half);
    glDeleteBuffers(1, &m_cube_vbo_half);
    glDeleteVertexArrays(1, &m_sphere_vao_half);
    glDeleteBuffers(1, &m_sphere_vbo_half);
    glDeleteVertexArrays(1, &m_cone_vao_half);
    glDeleteBuffers(1, &m_cone_vbo_half);
    glDeleteVertexArrays(1, &m_cylinder_vao_half);
    glDeleteBuffers(1, &m_cylinder_vbo_half);
    glDeleteVertexArrays(1, &m_cube_vao_min);
    glDeleteBuffers(1, &m_cube_vbo_min);
    glDeleteVertexArrays(1, &m_sphere_vao_min);
    glDeleteBuffers(1, &m_sphere_vbo_min);
    glDeleteVertexArrays(1, &m_cone_vao_min);
    glDeleteBuffers(1, &m_cone_vbo_min);
    glDeleteVertexArrays(1, &m_cylinder_vao_min);
    glDeleteBuffers(1, &m_cylinder_vbo_min);

    // for extra credit Mesh Rendering
    glDeleteVertexArrays(1, &m_mesh_vao);
    glDeleteBuffers(1, &m_mesh_vbo);

    delete m_sphere;
    delete m_cone;
    delete m_cube;
    delete m_cylinder;
    delete m_mesh;

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

    // Project 6 extra credit shadow
    glDeleteProgram(m_shadow_shader);
    for (auto map : m_shadowMaps){
        glDeleteTextures(1,&map.depthMap);
        glDeleteFramebuffers(1, &map.depthMapFBO);
    }
    m_shadowMaps.clear();
    this->doneCurrent();
}

void Realtime::updateVaoVbo(int p1, int p2){
    m_cube->updateParams(p1);
    m_sphere->updateParams(p1,p2);
    m_cone->updateParams(p1,p2);
    m_cylinder->updateParams(p1,p2);

    glBindVertexArray(m_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_cube->generateShape().size()*sizeof(GLfloat),m_cube->generateShape().data(),GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_sphere_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_sphere_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_sphere->generateShape().size()*sizeof(GLfloat),m_sphere->generateShape().data(),GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_cone_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_cone_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_cone->generateShape().size()*sizeof(GLfloat),m_cone->generateShape().data(),GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_cylinder_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_cylinder_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_cylinder->generateShape().size()*sizeof(GLfloat),m_cylinder->generateShape().data(),GL_STATIC_DRAW);
    setVAO();

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}
void Realtime::setVAO(){
    glEnableVertexAttribArray(0);  // Attribute location 0 for vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);  // Attribute location 1 for normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);  // Attribute location 1 for normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
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
    m_cone = new Cone();
    m_cylinder = new Cylinder();

    glGenVertexArrays(1,&m_cube_vao);
    glGenBuffers(1, &m_cube_vbo);
    glGenVertexArrays(1,&m_sphere_vao);
    glGenBuffers(1, &m_sphere_vbo);
    glGenVertexArrays(1,&m_cone_vao);
    glGenBuffers(1, &m_cone_vbo);
    glGenVertexArrays(1,&m_cylinder_vao);
    glGenBuffers(1, &m_cylinder_vbo);


    // for extra credit Adaptive Level of Detal
    // ==================================================================== set enableAdaptive
    enableAdaptiveNumber = false;
    enableAdaptiveDistance = false;
    numberThreshold1 = 20;
    numberThreshold2 = 50;
    distanceThreshold1 = 10.f;
    distanceThreshold2 = 50.f;
    m_cube_half = new Cube();
    m_sphere_half = new Sphere();
    m_cone_half = new Cone();
    m_cylinder_half = new Cylinder();
    m_cube_min = new Cube();
    m_sphere_min = new Sphere();
    m_cone_min = new Cone();
    m_cylinder_min = new Cylinder();
    glGenVertexArrays(1,&m_cube_vao_half);
    glGenBuffers(1, &m_cube_vbo_half);
    glGenVertexArrays(1,&m_sphere_vao_half);
    glGenBuffers(1, &m_sphere_vbo_half);
    glGenVertexArrays(1,&m_cone_vao_half);
    glGenBuffers(1, &m_cone_vbo_half);
    glGenVertexArrays(1,&m_cylinder_vao_half);
    glGenBuffers(1, &m_cylinder_vbo_half);

    glGenVertexArrays(1,&m_cube_vao_min);
    glGenBuffers(1, &m_cube_vbo_min);
    glGenVertexArrays(1,&m_sphere_vao_min);
    glGenBuffers(1, &m_sphere_vbo_min);
    glGenVertexArrays(1,&m_cone_vao_min);
    glGenBuffers(1, &m_cone_vbo_min);
    glGenVertexArrays(1,&m_cylinder_vao_min);
    glGenBuffers(1, &m_cylinder_vbo_min);

    updateVaoVboHalf(settings.shapeParameter1/2, settings.shapeParameter2/2);
    updateVaoVbo(settings.shapeParameter1, settings.shapeParameter2);
    updateVaoVboMin(1,1);

    // for extra credit mesh rendering
    m_mesh = new Mesh();
    glGenVertexArrays(1,&m_mesh_vao);
    glGenBuffers(1,&m_mesh_vbo);
    glBindVertexArray(m_mesh_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_mesh->getVertexData().size() * sizeof(float),m_mesh->getVertexData().data(), GL_STATIC_DRAW);
    setVAO();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // project 6 action
    //===================================================================== set filter, set defaultFBO
    m_defaultFBO = 2;
    m_pixelSwitch = 1; // if switch == 0, invert feature; if switch == 1, grayscale feature; if switch == 2, top mapping feature
    m_kernelSwitch = 1; // if switch == 0, blur feature; if switch == 1, shapern feature; if switch == 3, edge detect feature
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/screen.vert", ":/resources/shaders/screen.frag");
    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f
        };
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    makeFBO();

    // Project 6 extra credit shadow
    // m_shadow_shader = ShaderLoader::createShaderProgram(":/resources/shaders/shadow.vert", ":/resources/shaders/shadow.frag");
    // makeShadowFBO();
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    // Project 6 extra credit shadow mapping, bind the shadowfbo, lights to depthMap;
    // renderShadowMap();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    int adaptiveDetail = 0;
    if (enableAdaptiveNumber) {
        if (m_metaData.shapes.size() <= numberThreshold1) adaptiveDetail = 0;
        else if (m_metaData.shapes.size() > numberThreshold2) adaptiveDetail = 2;
        else adaptiveDetail = 1;
    }
    // passShadowMap();
    for (const RenderShapeData& shape : m_metaData.shapes){

        GLint modelLoc = glGetUniformLocation(m_shader, "modelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &shape.ctm[0][0]);

        GLint inverseModelLoc = glGetUniformLocation(m_shader, "inverseModelMatrix");
        glUniformMatrix4fv(inverseModelLoc, 1, GL_FALSE, &shape.inverseCTM[0][0]);

        const SceneMaterial& material = shape.primitive.material;
        GLint ambientLoc = glGetUniformLocation(m_shader, "material.ambient");
        GLint diffuseLoc = glGetUniformLocation(m_shader, "material.diffuse");
        GLint specularLoc = glGetUniformLocation(m_shader, "material.specular");
        GLint shininessLoc = glGetUniformLocation(m_shader, "material.shininess");
        glUniform3fv(ambientLoc, 1, &material.cAmbient[0]);
        glUniform3fv(diffuseLoc, 1, &material.cDiffuse[0]);
        glUniform3fv(specularLoc, 1, &material.cSpecular[0]);
        glUniform1f(shininessLoc, material.shininess);

        if (enableAdaptiveDistance) {
            glm::vec3 worldShapePos = glm::vec3(shape.ctm * glm::vec4(0.0f,0.0f,0.0f,1.0f));
            float distance = glm::distance(glm::vec3(m_camera.getCameraPos()),worldShapePos);
            std::cout << distance << std::endl;
            if (distance <= distanceThreshold1) adaptiveDetail = 0;
            else if (distance > distanceThreshold2) adaptiveDetail = 2;
            else adaptiveDetail = 1;
        }

        // Project 6 texture related
        GLint isTextureLoc = glGetUniformLocation(m_shader,"isTexture");
        glUniform1i(isTextureLoc, material.textureMap.isUsed);
        GLint blendLoc = glGetUniformLocation(m_shader, "blend");
        glUniform1f(blendLoc, material.blend);
        if (material.textureMap.isUsed) {
            // Activate texture unit 0
            glActiveTexture(GL_TEXTURE0);
            // Bind the texture
            glBindTexture(GL_TEXTURE_2D, material.textureMap.textureID);
            // Set the sampler uniform to use texture unit 0
            GLint samplerLoc = glGetUniformLocation(m_shader, "textureSampler");
            glUniform1i(samplerLoc, 0);
        }

        switch (shape.primitive.type){
            case PrimitiveType::PRIMITIVE_CUBE:
                switch (adaptiveDetail){
                    case 0:
                        glBindVertexArray(m_cube_vao);
                        glDrawArrays(GL_TRIANGLES,0,m_cube->generateShape().size()/8);
                        break;
                    case 1:
                        glBindVertexArray(m_cube_vao_half);
                        glDrawArrays(GL_TRIANGLES,0,m_cube_half->generateShape().size()/8);
                        break;
                    case 2:
                        glBindVertexArray(m_cube_vao_min);
                        glDrawArrays(GL_TRIANGLES,0,m_cube_min->generateShape().size()/8);
                        break;
                    }
                break;
            case PrimitiveType::PRIMITIVE_SPHERE:
                switch (adaptiveDetail){
                case 0:
                    glBindVertexArray(m_sphere_vao);
                    glDrawArrays(GL_TRIANGLES,0,m_sphere->generateShape().size()/8);
                    break;
                case 1:
                    glBindVertexArray(m_sphere_vao_half);
                    glDrawArrays(GL_TRIANGLES,0,m_sphere_half->generateShape().size()/8);
                    break;
                case 2:
                    glBindVertexArray(m_sphere_vao_min);
                    glDrawArrays(GL_TRIANGLES,0,m_sphere_min->generateShape().size()/8);
                    break;
                }
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                switch (adaptiveDetail){
                case 0:
                    glBindVertexArray(m_cone_vao);
                    glDrawArrays(GL_TRIANGLES,0,m_cone->generateShape().size()/8);
                    break;
                case 1:
                    glBindVertexArray(m_cone_vao_half);
                    glDrawArrays(GL_TRIANGLES,0,m_cone_half->generateShape().size()/8);
                    break;
                case 2:
                    glBindVertexArray(m_cone_vao_min);
                    glDrawArrays(GL_TRIANGLES,0,m_cone_min->generateShape().size()/8);
                    break;
                }
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                switch (adaptiveDetail){
                case 0:
                    glBindVertexArray(m_cylinder_vao);
                    glDrawArrays(GL_TRIANGLES,0,m_cylinder->generateShape().size()/8);
                    break;
                case 1:
                    glBindVertexArray(m_cylinder_vao_half);
                    glDrawArrays(GL_TRIANGLES,0,m_cylinder_half->generateShape().size()/8);
                    break;
                case 2:
                    glBindVertexArray(m_cylinder_vao_min);
                    glDrawArrays(GL_TRIANGLES,0,m_cylinder_min->generateShape().size()/8);
                    break;
                }
                break;
            case PrimitiveType::PRIMITIVE_MESH:
                glBindVertexArray(m_mesh_vao);
                glDrawArrays(GL_TRIANGLES, 0, m_mesh->getVertexData().size() / 8);
                break;
        }

        if (material.textureMap.isUsed) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
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
    m_metaData = RenderData();
    bool success = SceneParser::parse(settings.sceneFilePath, m_metaData);
    if (!success) std::cout << "parsing failed";
    m_camera = Camera(m_metaData.cameraData,width(),height());
    m_camera.updateNearFar(0.1f,100.0f);
    for (RenderShapeData& shape : m_metaData.shapes) {
        if (shape.primitive.material.textureMap.isUsed){
            glGenTextures(1,&(shape.primitive.material.textureMap.textureID));
            glBindTexture(GL_TEXTURE_2D, shape.primitive.material.textureMap.textureID);
            QImage* image = &shape.primitive.material.textureMap.textureImage;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width(), image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,image->bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D,0);
        }
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            std::string meshFile = shape.primitive.meshfile;
            m_mesh->loadOBJ(meshFile);
            glBindVertexArray(m_mesh_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vbo);
            glBufferData(GL_ARRAY_BUFFER, m_mesh->getVertexData().size() * sizeof(float),m_mesh->getVertexData().data(), GL_STATIC_DRAW);
            setVAO();
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }
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
    if (settings.perPixelFilter) {
        if (m_pixelSwitch == 0) m_postprocess = 1;
        else if (m_pixelSwitch == 1) m_postprocess = 2;
        else m_postprocess = 5;
    }
    if (settings.kernelBasedFilter) {
        if (m_kernelSwitch == 0) m_postprocess = 3;
        else if (m_kernelSwitch == 1) m_postprocess = 4;
        else m_postprocess = 6;
    }
    if (settings.extraCredit1) m_postprocess = 7;
    if (!settings.perPixelFilter && !settings.kernelBasedFilter && !settings.extraCredit1){
        m_postprocess = 0;
    }
    if (m_previous_postprocess != m_postprocess){
        std::cout << "current filter: " << m_postprocess << std::endl;
        m_previous_postprocess = m_postprocess;
    }
    if (settings.nearPlane != preNear || settings.farPlane != preFar){
        if (m_sphere) { // if the m_sphere is nullptr, meaning uninitialized, the update will not work
        m_camera.updateNearFar(settings.nearPlane,settings.farPlane);
        glUseProgram(m_shader);
        glm::mat4 projectionMatrix = m_camera.getProjectionMatrix();
        GLint projLoc = glGetUniformLocation(m_shader, "projMatrix");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
        glUseProgram(0);
        preNear = settings.nearPlane;
        preFar = settings.farPlane;
        std::cout << "P1: " << settings.shapeParameter1 << " P2: " << settings.shapeParameter2
                  << " Near: " << settings.nearPlane << " Far: " << settings.farPlane << std::endl;
        }
    }
    if (settings.shapeParameter1 != preP1 || settings.shapeParameter2 != preP2){
        if (m_sphere) updateVaoVbo(settings.shapeParameter1,settings.shapeParameter2);
        // for extra credit Adaptive Detail
        if (m_sphere_half) updateVaoVboHalf(settings.shapeParameter1/2, settings.shapeParameter2/2);
        if (m_sphere_min) updateVaoVboMin(1,1);
        preP1 = settings.shapeParameter1;
        preP2 = settings.shapeParameter2;

        std::cout << "P1: " << settings.shapeParameter1 << " P2: " << settings.shapeParameter2
              << " Near: " << settings.nearPlane << " Far: " << settings.farPlane << std::endl;
    }
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
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
    if (m_keyMap[Qt::Key_Control]) movement += glm::vec3(0.f,-1.f,0.f) * movementSpeed * deltaTime;
    m_camera.updateTranslation(movement);
    glUseProgram(m_shader);
    glm::mat4 viewMatrix = m_camera.getViewMatrix();
    GLint viewLoc = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUseProgram(0);
    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
