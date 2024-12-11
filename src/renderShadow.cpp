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
#include <glm/gtc/matrix_transform.hpp>

void Realtime::makeShadowFBO(){
    for (int i = 0; i < 8; i++){
        ShadowMap shadowMap;

        // Generate framebuffer and depth texture
        glGenFramebuffers(1, &shadowMap.depthMapFBO);
        glGenTextures(1, &shadowMap.depthMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap.depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Set texture wrapping to clamp to reduce shadow artifacts
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // Attach the depth texture to the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap.depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Shadow Map Framebuffer not complete for light " << i << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

        // Add the shadow map to the vector
        m_shadowMaps.push_back(shadowMap);
    }
}
void Realtime::renderShadowMap(){


    for (size_t i = 0; i < m_metaData.lights.size(); ++i) {
        const SceneLightData& light = m_metaData.lights[i];

        // Only process directional lights for shadow mapping
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            ShadowMap& shadowMap = m_shadowMaps[i];
            std::cout << "Rendering shadow map for directional light " << i << std::endl;

            // 1. Configure the light's orthographic projection matrix and view matrix
            glm::vec3 lightDir = glm::normalize(light.dir);
            float orthoSize = 50.0f; // Adjust based on your scene's size
            float near_plane = 0.1f, far_plane = 100.0f;
            glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
            glm::vec3 lightPos = -lightDir * 50.0f; // Position light far away in its direction
            glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

            shadowMap.lightSpaceMatrix = lightProjection * lightView;

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);


            glUseProgram(m_shadow_shader);


            GLint lightSpaceMatrixLoc = glGetUniformLocation(m_shadow_shader, "lightSpaceMatrix");


            for (size_t objIdx = 0; objIdx < m_allObjects.size(); ++objIdx) {
                const basicMapFile& object = m_allObjects[objIdx];


                // Set the model matrix uniform
                GLint modelLoc = glGetUniformLocation(m_shadow_shader, "modelMatrix");


                // Bind the appropriate VAO based on object type
                glBindVertexArray(object.vao);

                // Determine the number of vertices to draw based on object type
                int vertexCount = 0;
                switch (object.objectType){
                case 1: // Cube
                    vertexCount = m_cube->generateShape().size() / 8;
                    break;
                case 2: // Sphere
                    vertexCount = m_cube->generateShape().size() / 8;
                    break;
                case 3: //
                    vertexCount = m_sphere->generateShape().size() / 8;
                    break;

                case 4: // Mesh
                    vertexCount = m_mesh->getVertexData().size() / 8;
                    break;
                default:
                    std::cerr << "Error: Unknown objectType " << object.objectType << " for object " << objIdx << std::endl;
                    break;
                }

                // Unbind VAO after drawing
                glBindVertexArray(0);
            }

            glUseProgram(0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

    }

}
void Realtime::passShadowMap(){
    glUseProgram(m_shader);
    const int maxLightNumber = 8;
    const std::vector<SceneLightData>& allLights = m_metaData.lights;
    int numLights = std::min(static_cast<int>(allLights.size()), maxLightNumber);
    for (size_t i = 0; i < numLights; ++i) {
        ShadowMap& shadowMap = m_shadowMaps[i];

        // Set light space matrix
        std::string lightSpaceMatrixName = "lightSpaceMatrices[" + std::to_string(i) + "]";
        GLint lightSpaceMatrixLoc = glGetUniformLocation(m_shader, lightSpaceMatrixName.c_str());
        if (lightSpaceMatrixLoc != -1) {
            glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, &shadowMap.lightSpaceMatrix[0][0]);
        } else {
            std::cerr << "Uniform " << lightSpaceMatrixName << " not found." << std::endl;
        }

        // Bind shadow map texture
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, shadowMap.depthMap);
        std::string shadowMapName = "shadowMaps[" + std::to_string(i) + "]";
        GLint shadowMapLoc = glGetUniformLocation(m_shader, shadowMapName.c_str());
        if (shadowMapLoc != -1) {
            glUniform1i(shadowMapLoc, 1 + i);
        } else {
            std::cerr << "Uniform " << shadowMapName << " not found." << std::endl;
        }
    }
    glUseProgram(0);
}

