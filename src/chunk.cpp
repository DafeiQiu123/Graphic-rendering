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
#include <random>
#include <numeric>

// Perlin noise implementation
float Realtime::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Realtime::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float Realtime::grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float Realtime::perlin(float x, float y, float z) {
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    int Z = static_cast<int>(floor(z)) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int A = m_permutation[X] + Y;
    int AA = m_permutation[A] + Z;
    int AB = m_permutation[A + 1] + Z;
    int B = m_permutation[X + 1] + Y;
    int BA = m_permutation[B] + Z;
    int BB = m_permutation[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(m_permutation[AA], x, y, z),
                                grad(m_permutation[BA], x-1, y, z)),
                        lerp(u, grad(m_permutation[AB], x, y-1, z),
                             grad(m_permutation[BB], x-1, y-1, z))),
                lerp(v, lerp(u, grad(m_permutation[AA+1], x, y, z-1),
                             grad(m_permutation[BA+1], x-1, y, z-1)),
                     lerp(u, grad(m_permutation[AB+1], x, y-1, z-1),
                          grad(m_permutation[BB+1], x-1, y-1, z-1))));
}

void Realtime::initializeNoise() {
    m_permutation.resize(256);
    std::iota(m_permutation.begin(), m_permutation.end(), 0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(m_permutation.begin(), m_permutation.end(), gen);
    m_permutation.insert(m_permutation.end(), m_permutation.begin(), m_permutation.end());
}

float Realtime::getTerrainHeight(int worldX, int worldZ) {
    float scale = 0.05f;
    float amplitude = 4.0f;
    float baseHeight = 4.0f;

    float x = worldX * scale;
    float z = worldZ * scale;

    float height = baseHeight;
    height += perlin(x, 0, z) * amplitude;
    height += perlin(x * 2, 0, z * 2) * amplitude * 0.5f;

    return height;
}

void Realtime::clearMapHitbox() {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 9; y++) {
            for (int z = 0; z < 5; z++) {
                m_mapHitbox[x][y][z] = hitBox(); // This sets to default values
                // Or explicitly set values:
                m_mapHitbox[x][y][z].hitBoxA = glm::vec3(0.0f);
                m_mapHitbox[x][y][z].hitBoxB = glm::vec3(0.0f);
                m_mapHitbox[x][y][z].cood = glm::vec3(0.0f);
            }
        }
    }
}

void Realtime::createMap2() {
    initializeNoise();
    clearMapHitbox();

    static const int CHUNK_SIZE = 5;
    static const int VIEW_DISTANCE = 1;

    // for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
    //     for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    int worldX = 0 * CHUNK_SIZE + x;
                    int worldZ = 0 * CHUNK_SIZE + z;
                    float height = getTerrainHeight(worldX, worldZ);

                    std::cout<<"height="<<height<<'\n';

                    if (worldX == 0 && worldZ == 0) {
                        ini_Y = height + 0.75f;
                    }

                    for (int y = 0; y <= static_cast<int>(height); y++) {
                        basicMapFile block;
                        glm::vec3 translate = glm::vec3(float(worldX),float(y),float(worldZ));

                        glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
                        float angle = 0;
                        glm::vec3 scale = glm::vec3(1.f,1.f,1.f);

                        block.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                                            * glm::rotate(glm::mat4(1.0f), angle, rotate)
                                            * glm::scale(glm::mat4(1.0f), scale);
                        block.inverseModelMatrix = glm::inverse(block.modelMatrix);
                        block.textureID = m_cube_texture;
                        block.vbo = m_cube_vbo;
                        block.vao = m_cube_vao;
                        block.objectType = 0;

                        // if (y == static_cast<int>(height)) {
                        //     block.material.cDiffuse = glm::vec4(0.2f, 0.6f, 0.2f, 1.0f);
                        // } else if (y > height - 2) {
                        //     block.material.cDiffuse = glm::vec4(0.5f, 0.35f, 0.05f, 1.0f);
                        // } else {
                        //     block.material.cDiffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
                        // }

                        // block.material.cAmbient = glm::vec4(0.2f);
                        // block.material.cSpecular = glm::vec4(0.5f);

                        block.material.cAmbient = glm::vec4(0.2,0.2,0.2,0);
                        block.material.cDiffuse = glm::vec4(0.5,0.1,0.5,0);
                        block.material.cSpecular = glm::vec4(0.5,0.5,0.5,0);
                        block.material.shininess = 1;
                        block.material.blend = 1;
                        block.material.textureMap.isUsed = true;

                        block.hitboxObj.hitBoxA = translate - glm::vec3(0.5f);
                        block.hitboxObj.hitBoxB = translate + glm::vec3(0.5f);
                        block.hitboxObj.cood = translate;

                        m_mapHitbox[x][y][z] = block.hitboxObj;

                        m_allObjects.push_back(block);
                    }
                }
            // }
        // }
    }
    m_currentChunkCoord = glm::ivec2(0, 0);
}

void Realtime::updateVisibleChunks() {
    static const int CHUNK_SIZE = 5;
    glm::ivec2 newChunkCoord(
        static_cast<int>(floor(m_mainChaX / CHUNK_SIZE)),
        static_cast<int>(floor(m_mainChaZ / CHUNK_SIZE))
        );

    if (newChunkCoord != m_currentChunkCoord) {
        // Remove all terrain blocks
        m_allObjects.erase(
            std::remove_if(m_allObjects.begin(), m_allObjects.end(),
                           [](const basicMapFile& obj) { return obj.objectType == 0; }),
            m_allObjects.end()
            );

        // Generate new chunks
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int z = 0; z < CHUNK_SIZE; z++) {
                        int worldX = (newChunkCoord.x + dx) * CHUNK_SIZE + x;
                        int worldZ = (newChunkCoord.y + dz) * CHUNK_SIZE + z;
                        float height = getTerrainHeight(worldX, worldZ);

                        for (int y = 0; y <= static_cast<int>(height); y++) {
                            basicMapFile block;
                            glm::vec3 translate(worldX, y, worldZ);

                            block.modelMatrix = glm::translate(glm::mat4(1.0f), translate) *
                                                glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
                            block.inverseModelMatrix = glm::inverse(block.modelMatrix);
                            block.textureID = m_cube_texture;
                            block.vbo = m_cube_vbo;
                            block.vao = m_cube_vao;
                            block.objectType = 0;

                            if (y == static_cast<int>(height)) {
                                block.material.cDiffuse = glm::vec4(0.2f, 0.6f, 0.2f, 1.0f);
                            } else if (y > height - 2) {
                                block.material.cDiffuse = glm::vec4(0.5f, 0.35f, 0.05f, 1.0f);
                            } else {
                                block.material.cDiffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
                            }

                            block.material.cAmbient = glm::vec4(0.2f);
                            block.material.cSpecular = glm::vec4(0.5f);
                            block.material.shininess = 1;
                            block.material.blend = 1;
                            block.material.textureMap.isUsed = true;

                            block.hitboxObj.hitBoxA = translate - glm::vec3(0.5f);
                            block.hitboxObj.hitBoxB = translate + glm::vec3(0.5f);
                            block.hitboxObj.cood = translate;

                            m_allObjects.push_back(block);
                        }
                    }
                }
            }
        }
        m_currentChunkCoord = newChunkCoord;
    }
}
