#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <utils/scenedata.h>
#include <utils/sceneparser.h>
#include <utils/shaderloader.h>
#include <camera/camera.h>
#include <glm/gtx/transform.hpp>
// For extra credit Adaptive level of Detail
bool Realtime::mapGeneratingFunction(glm::vec3 xyz){
    if (xyz.y > xyz.x + xyz.z) return false;
    if (xyz.y == 8) return false;
    return true;
}

void Realtime::createMap(){
    for (int z = 0; z < 5; z++){
        for (int y = 0; y < 9; y++){
            for (int x = 0; x < 5; x++){
                if(!mapGeneratingFunction(glm::vec3(x,y,z))) continue;
                basicMapFile oneCube;
                glm::vec3 translate = glm::vec3(float(x),float(y),float(z));
                glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
                float angle = 0;
                glm::vec3 scale = glm::vec3(1.f,1.f,1.f);
                oneCube.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                                      * glm::rotate(glm::mat4(1.0f), angle, rotate)
                                      * glm::scale(glm::mat4(1.0f), scale);
                oneCube.inverseModelMatrix = glm::inverse(oneCube.modelMatrix);
                oneCube.textureID = m_cube_texture;
                oneCube.vbo = m_cube_vbo;
                oneCube.vao = m_cube_vao;
                oneCube.objectType = 0;
                oneCube.material.cAmbient = glm::vec4(0.2,0.2,0.2,0);
                oneCube.material.cDiffuse = glm::vec4(0.5,0.1,0.5,0);
                oneCube.material.cSpecular = glm::vec4(0.5,0.5,0.5,0);
                oneCube.material.shininess = 1;
                oneCube.material.blend = 1;
                oneCube.material.textureMap.isUsed = true;

                oneCube.hitboxObj.hitBoxA = translate - glm::vec3(0.5f);  // min bounds
                oneCube.hitboxObj.hitBoxB = translate + glm::vec3(0.5f);  // max bounds
                oneCube.hitboxObj.cood = translate;

                m_mapHitbox[x][y][z] = oneCube.hitboxObj;

                m_allObjects.push_back(oneCube);
            }
        }
    }
}

void Realtime::createBackground(){
    basicMapFile background;
    glm::vec3 translate = glm::vec3(float(20),float(0),float(20));
    glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
    float angle = glm::radians(45.0f);
    glm::vec3 scale = glm::vec3(50,80,0.01);
    background.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                             * glm::rotate(glm::mat4(1.0f), angle, rotate)
                             * glm::scale(glm::mat4(1.0f), scale);
    background.inverseModelMatrix = glm::inverse(background.modelMatrix);
    background.textureID = m_background_texture;
    background.vbo = m_cube_vbo;
    background.vao = m_cube_vao;
    background.objectType = 2;
    background.material.cAmbient = glm::vec4(0.0f);
    background.material.cDiffuse = glm::vec4(0.1,0.1,0.1,0);
    background.material.cSpecular = glm::vec4(0.0f);
    background.material.shininess = 5;
    background.material.blend = 1;
    background.material.textureMap.isUsed = true;
    m_mainChaOriginalCTM = background.modelMatrix;
    m_allObjects.push_back(background);

}

void Realtime::createDragon(){
    basicMapFile background;
    glm::vec3 translate = glm::vec3(float(8),float(7),float(0));
    glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
    float angle = glm::radians(45.0f);
    glm::vec3 scale = glm::vec3(4,4,4);
    background.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                             * glm::rotate(glm::mat4(1.0f), angle, rotate)
                             * glm::scale(glm::mat4(1.0f), scale);
    background.inverseModelMatrix = glm::inverse(background.modelMatrix);
    background.textureID = m_background_texture;
    background.vbo = m_mesh_dragon_vbo;
    background.vao = m_mesh_dragon_vao;
    background.objectType = 3;
    background.material.cAmbient = glm::vec4(0.2f);
    background.material.cDiffuse = glm::vec4(0.5,0.5,0.5,0);
    background.material.cSpecular = glm::vec4(0.0f);
    background.material.shininess = 5;
    background.material.blend = 1;
    background.material.textureMap.isUsed = true;
    m_mainChaOriginalCTM = background.modelMatrix;
    m_allObjects.push_back(background);
}
void Realtime::createBunny(){
    basicMapFile background;
    glm::vec3 translate = glm::vec3(float(0),float(5),float(8.5));
    glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
    float angle = glm::radians(180.0f);
    glm::vec3 scale = glm::vec3(23,18,23);
    background.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                             * glm::rotate(glm::mat4(1.0f), angle, rotate)
                             * glm::scale(glm::mat4(1.0f), scale);
    background.inverseModelMatrix = glm::inverse(background.modelMatrix);
    background.textureID = m_background_texture;
    background.vbo = m_mesh_bunny_vbo;
    background.vao = m_mesh_bunny_vao;
    background.objectType = 4;
    background.material.cAmbient = glm::vec4(0.2f);
    background.material.cDiffuse = glm::vec4(0.5,0.5,0.5,0);
    background.material.cSpecular = glm::vec4(0.0f);
    background.material.shininess = 5;
    background.material.blend = 1;
    background.material.textureMap.isUsed = true;
    m_mainChaOriginalCTM = background.modelMatrix;
    m_allObjects.push_back(background);
}
void Realtime::createPortal1(){
    // bunny side portal
    basicMapFile background;
    glm::vec3 translate = glm::vec3(float(3),float(8.5),float(4.5));
    glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
    float angle = glm::radians(0.0f);
    glm::vec3 scale = glm::vec3(1,1,0.01);
    background.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                             * glm::rotate(glm::mat4(1.0f), angle, rotate)
                             * glm::scale(glm::mat4(1.0f), scale);
    background.inverseModelMatrix = glm::inverse(background.modelMatrix);
    background.textureID = m_portal_texture;
    background.vbo = m_cube_vbo;
    background.vao = m_cube_vao;
    background.objectType = 5;
    background.material.cAmbient = glm::vec4(0.0f);
    background.material.cDiffuse = glm::vec4(2,2,2,0);
    background.material.cSpecular = glm::vec4(0.0f);
    background.material.shininess = 5;
    background.material.blend = 1;
    background.material.textureMap.isUsed = true;
    m_mainChaOriginalCTM = background.modelMatrix;
    m_allObjects.push_back(background);
}
void Realtime::createPortal2(){
    // dragon side portal
    basicMapFile background;
    glm::vec3 translate = glm::vec3(float(4.5),float(8.5),float(3));
    glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
    float angle = glm::radians(0.0f);
    glm::vec3 scale = glm::vec3(0.01,1,1);
    background.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                             * glm::rotate(glm::mat4(1.0f), angle, rotate)
                             * glm::scale(glm::mat4(1.0f), scale);
    background.inverseModelMatrix = glm::inverse(background.modelMatrix);
    background.textureID = m_portal_texture;
    background.vbo = m_cube_vbo;
    background.vao = m_cube_vao;
    background.objectType = 6;
    background.material.cAmbient = glm::vec4(0.0f);
    background.material.cDiffuse = glm::vec4(2,2,2,0);
    background.material.cSpecular = glm::vec4(0.0f);
    background.material.shininess = 5;
    background.material.blend = 1;
    background.material.textureMap.isUsed = true;
    m_mainChaOriginalCTM = background.modelMatrix;
    m_allObjects.push_back(background);
}
void Realtime::createMainCharacter(){
    basicMapFile mainCha;
    glm::vec3 translate;
    if (phase2 < 2) translate = glm::vec3(float(0),float(0.76),float(0));
    else translate = glm::vec3(float(0),float(ini_Y),float(0));
    glm::vec3 rotate = glm::vec3(0.f,1.f,0.f);
    float angle = 0;
    glm::vec3 scale = glm::vec3(0.5f,0.5f,0.5f);
    mainCha.modelMatrix = glm::translate(glm::mat4(1.0f), translate)
                          * glm::rotate(glm::mat4(1.0f), angle, rotate)
                          * glm::scale(glm::mat4(1.0f), scale);
    mainCha.inverseModelMatrix = glm::inverse(mainCha.modelMatrix);
    mainCha.textureID = m_mainCha_texture;
    mainCha.vbo = m_sphere_vbo;
    mainCha.vao = m_sphere_vao;
    mainCha.objectType = 1;
    mainCha.material.cAmbient = glm::vec4(0.1,0.1,0.1,0);
    mainCha.material.cDiffuse = glm::vec4(0.3,0.3,0.3,0);
    mainCha.material.cSpecular = glm::vec4(0.5,0.5,0.5,0);
    mainCha.material.shininess = 1;
    mainCha.material.blend = 1;
    mainCha.material.textureMap.isUsed = true;
    m_mainChaOriginalCTM = mainCha.modelMatrix;
    m_allObjects.push_back(mainCha);
}
