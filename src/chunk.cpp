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

void Realtime::createMap2(int chunkX, int chunkZ) {
    clearMapHitbox();

    static const int CHUNK_SIZE = 5;
    static const int VIEW_DISTANCE = 1;

    int chunkOffsetX = chunkX * CHUNK_SIZE;
    int chunkOffsetZ = chunkZ * CHUNK_SIZE;

    // for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
    //     for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int worldX = chunkOffsetX + x;
            int worldZ = chunkOffsetZ + z;
            float height = getTerrainHeight(worldX, worldZ);



            if (chunkX == 0 && chunkZ == 0 && x == 0 && z == 0) {
                ini_Y = static_cast<int>(height) + 0.75f;
            }

            for (int y = 0; y <= static_cast<int>(height); y++) {
                basicMapFile block;
                glm::vec3 translate = glm::vec3(float(x),float(y),float(z));

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

void Realtime::handleChunkTransition(const glm::vec3& newWorldPos) {
    // Calculate which chunk we should be in
    const int CHUNK_SIZE = 5;
    int newChunkX = floor(newWorldPos.x / CHUNK_SIZE);
    int newChunkZ = floor(newWorldPos.z / CHUNK_SIZE);

    // Store current chunk
    static int currentChunkX = 0;
    static int currentChunkZ = 0;

    // Check if we've moved to a new chunk
    if (newChunkX != currentChunkX || newChunkZ != currentChunkZ) {
        std::cout << "Moving to chunk: " << newChunkX << ", " << newChunkZ << std::endl;

        // Clear and regenerate objects
        m_allObjects.clear();

        // Generate new chunk at (0,0)
        createMap2(newChunkX, newChunkZ);
        createBackground();
        createMainCharacter();

        // Update chunk tracking
        currentChunkX = newChunkX;
        currentChunkZ = newChunkZ;

        // Find new main character index
        for (size_t i = 0; i < m_allObjects.size(); i++) {
            if (m_allObjects[i].objectType == 1) mainChaIndex = i;
        }

        // Convert world coordinates to local chunk coordinates
        float localX = fmod(newWorldPos.x + CHUNK_SIZE, CHUNK_SIZE);
        float localZ = fmod(newWorldPos.z + CHUNK_SIZE, CHUNK_SIZE);

        if (localX < 0) localX += CHUNK_SIZE;
        if (localZ < 0) localZ += CHUNK_SIZE;

        // Update character position within the chunk
        m_mainChaX = localX;
        m_mainChaZ = localZ;

        // Update world coordinate tracking
        worldCood.x = newWorldPos.x;
        worldCood.z = newWorldPos.z;
    }
}

void Realtime::updateWorldPosition(float deltaX, float deltaZ) {
    worldCood.x += deltaX;
    worldCood.z += deltaZ;

    handleChunkTransition(worldCood);
}
