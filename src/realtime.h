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
    glm::mat4 depthBiasMVP;
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
    GLuint m_shader;
    GLuint m_cube_vbo;
    GLuint m_sphere_vbo;
    GLuint m_cube_vao;
    GLuint m_sphere_vao;
    float preNear = 0.1f;
    float preFar = 100.0f;
    int preP1,preP2 = 5;
    void paintOriginal();
    void updateVaoVbo(int p1, int p2);
    void setVAO();

    // For extra credit mesh rendering
    GLuint m_mesh_dragon_vao;
    GLuint m_mesh_dragon_vbo;
    GLuint m_mesh_bunny_vao;
    GLuint m_mesh_bunny_vbo;
    Mesh* m_mesh_dragon = nullptr;
    Mesh* m_mesh_bunny = nullptr;

    // Project 6 action
    void screenPostproSetup();
    void makeFBO();
    // Task 30: Update the paintTexture function signature
    void paintTexture(GLuint texture);
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
    ShadowMap m_shadowMap;
    GLuint m_shadow_shader;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    void renderShadowMap();
    void makeShadowFBO();

    // Final Project
    std::vector<basicMapFile> m_allObjects;
    GLuint m_cube_texture;
    GLuint m_mainCha_texture;
    GLuint m_background_texture;
    GLuint m_portal_texture;
    QImage m_background_texture_image;
    QImage m_mainCha_texture_image;
    QImage m_portal_texture_image;
    QImage m_cube_texture_image;
    void bindTexture(GLuint& textureID, QImage* image);
    void createMap();
    void createDragon();
    void createBunny();
    void createPortal1();
    void createPortal2();
    void createMainCharacter();
    bool mapGeneratingFunction(glm::vec3 xyz);
    void paintBasicMap();
    void createBackground();
    size_t dragonIndex;
    size_t bunnyIndex;
    size_t mainChaIndex;
    size_t portal1Index;
    size_t portal2Index;
    bool dragonTrigger = false;
    bool bunnyTrigger = false;
    // main character movement
    float m_mainChaX,m_mainChaY,m_mainChaZ = 0.0f;
    bool m_mainChaJumping = false;
    float m_mainChaSpeedHorizontal = 5.0f;
    float m_mainChaSpeedVertical = 5.0f;
    float m_gravity = -9.8f;
    glm::mat4 m_mainChaOriginalCTM;
    hitBox m_mapHitbox[5][9][5];

    void initializeMapLayout();
    bool checkHorizontalCollision(const glm::vec3& position, float radius);
    bool isOnGround(const glm::vec3& position, float radius);
    bool checkBoxIntersection(const glm::vec3& box1Min, const glm::vec3& box1Max,
                              const glm::vec3& box2Min, const glm::vec3& box2Max);
    glm::vec3 getClosestPointOnBox(const glm::vec3& point,
                                   const glm::vec3& boxMin, const glm::vec3& boxMax);

    GLuint m_debug_shader;
    GLuint m_debug_quad_vao;
    GLuint m_debug_quad_vbo;
    void initDebugQuad();
    void renderDebugQuad();
    bool shadow_on = false;
    bool developer_on = false;

    std::vector<int> m_permutation;
    glm::ivec2 m_currentChunkCoord;

    // Add these new function declarations
    void initializeNoise();
    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y, float z);
    float perlin(float x, float y, float z);
    float getTerrainHeight(int worldX, int worldZ);

    void updateWorldPosition(float deltaX, float deltaZ);
    void createMap2(int chunkX, int chunkZ);
    void clearMapHitbox();
    void handleChunkTransition(const glm::vec3& newWorldPos);

    glm::vec3 worldCood {0, 0, 0};
    glm::vec2 chunkCood {0, 0};


    float ini_Y;
    // int phase2=2;
    int phase2=0;

    struct ChunkCoord {
        int x;
        int z;

        bool operator==(const ChunkCoord& other) const {
            return x == other.x && z == other.z;
        }

        bool operator!=(const ChunkCoord& other) const {
            return !(*this == other);
        }
    };

    const float CHUNK_SIZE_X = 5.0f;
    const float CHUNK_SIZE_Z = 5.0f;
    const float CHUNK_OFFSET = -0.5f;

    ChunkCoord calculateChunkCoord(const glm::vec3& worldPos) {
        ChunkCoord chunk;
        // Subtract offset and divide by chunk size to get chunk coordinates
        chunk.x = static_cast<int>(floor((worldPos.x - CHUNK_OFFSET) / CHUNK_SIZE_X));
        chunk.z = static_cast<int>(floor((worldPos.z - CHUNK_OFFSET) / CHUNK_SIZE_Z));
        return chunk;
    }

    glm::vec3 getChunkOrigin(const ChunkCoord& chunk) {
        return glm::vec3(
            chunk.x * CHUNK_SIZE_X + CHUNK_OFFSET,
            0.0f,  // Y coordinate remains unchanged
            chunk.z * CHUNK_SIZE_Z + CHUNK_OFFSET
            );
    }

};
