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
    void setVAO();
    void renderShadowMap();
    void passShadowMap();
    void updateVaoVbo(int p1, int p2);
    void updateVaoVboHalf(int p1, int p2);
    void updateVaoVboMin(int p1, int p2);
    void makeShadowFBO();

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
    // For extra credit adaptive level of detail
    bool enableAdaptiveNumber = false;
    bool enableAdaptiveDistance = false;

    int numberThreshold1 = 50;
    int numberThreshold2 = 100;
    float distanceThreshold1 = 50.f;
    float distanceThreshold2 = 100.f;
    Cube* m_cube_half = nullptr;
    Sphere* m_sphere_half = nullptr;
    Cylinder* m_cylinder_half = nullptr;
    Cone* m_cone_half = nullptr;
    GLuint m_cube_vbo_half;
    GLuint m_sphere_vbo_half;
    GLuint m_cylinder_vbo_half;
    GLuint m_cone_vbo_half;
    GLuint m_cube_vao_half;
    GLuint m_sphere_vao_half;
    GLuint m_cylinder_vao_half;
    GLuint m_cone_vao_half;
    Cube* m_cube_min = nullptr;
    Sphere* m_sphere_min = nullptr;
    Cylinder* m_cylinder_min = nullptr;
    Cone* m_cone_min = nullptr;
    GLuint m_shader_min;
    GLuint m_cube_vbo_min;
    GLuint m_sphere_vbo_min;
    GLuint m_cylinder_vbo_min;
    GLuint m_cone_vbo_min;
    GLuint m_cube_vao_min;
    GLuint m_sphere_vao_min;
    GLuint m_cylinder_vao_min;
    GLuint m_cone_vao_min;

    // For extra credit mesh rendering
    GLuint m_mesh_vao;
    GLuint m_mesh_vbo;
    Mesh* m_mesh = nullptr;

    // Project 6 action
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

    // Project 6 extra credit
    std::vector<ShadowMap> m_shadowMaps;
    GLuint m_shadow_shader;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

};
