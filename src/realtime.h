#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include <utils/scenedata.h>
#include <utils/sceneparser.h>
#include <camera/camera.h>
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "shapes/Mesh.h"
//Final Project

struct hitBox{
    glm::vec3 hitBoxA;
    glm::vec3 hitBoxB;
    glm::vec3 cood;
};


struct basicMapFile{
    glm::mat4 modelMatrix;
    glm::mat4 inverseModelMatrix;
    int objectType;
    GLuint vbo;
    GLuint vao;
    GLuint textureID;
    SceneMaterial material;
    hitBox hitboxObj;
};

struct ShadowMap {
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightSpaceMatrix;
};

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes


private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    RenderData m_metaData;
    Camera m_camera;

    Cube* m_cube = nullptr;
    Sphere* m_sphere = nullptr;
    Cylinder* m_cylinder = nullptr;
    Cone* m_cone = nullptr;
    GLuint m_shader;
    GLuint m_cube_vbo;
    GLuint m_sphere_vbo;
    GLuint m_cylinder_vbo;
    GLuint m_cone_vbo;
    GLuint m_cube_vao;
    GLuint m_sphere_vao;
    GLuint m_cylinder_vao;
    GLuint m_cone_vao;
    float preNear = 0.1f;
    float preFar = 100.0f;
    int preP1,preP2 = 5;
    void paintOriginal();
    void updateVaoVbo(int p1, int p2);
    void setVAO();

    // For extra credit mesh rendering
    GLuint m_mesh_vao;
    GLuint m_mesh_vbo;
    Mesh* m_mesh = nullptr;

    // Project 6 action
    void screenPostproSetup();
    void makeFBO();
    // Task 30: Update the paintTexture function signature
    void paintTexture(GLuint texture);
    int m_pixelSwitch, m_kernelSwitch;
    int m_postprocess = 0;
    int m_previous_postprocess = 0;
    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;
    GLuint m_texture_shader;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    // Project 6 extra credit shadowMap
    std::vector<ShadowMap> m_shadowMaps;
    GLuint m_shadow_shader;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    void renderShadowMap();
    void passShadowMap();
    void makeShadowFBO();

    // Final Project
    std::vector<basicMapFile> m_allObjects;
    GLuint m_cube_texture;
    GLuint m_mainCha_texture;
    GLuint m_background_texture;
    QImage m_background_texture_image;
    QImage m_mainCha_texture_image;
    QImage m_cube_texture_image;
    void bindTexture(GLuint& textureID, QImage* image);
    void createMap();
    void createMainCharacter();
    bool mapGeneratingFunction(glm::vec3 xyz);
    void paintBasicMap();
    void createBackground();
    // main character movement
    float m_mainChaX,m_mainChaY,m_mainChaZ = 0.0f;
    bool m_mainChaJumping = false;
    float m_mainChaSpeedHorizontal = 5.0f;
    float m_mainChaSpeedVertical = 5.0f;
    glm::mat4 m_mainChaOriginalCTM;


    hitBox m_mapHitbox[5][9][5];

    void initializeMapLayout();
    bool checkHorizontalCollision(const glm::vec3& position, float radius);
    bool isOnGround(const glm::vec3& position, float radius);
    bool checkBoxIntersection(const glm::vec3& box1Min, const glm::vec3& box1Max,
                              const glm::vec3& box2Min, const glm::vec3& box2Max);
};
