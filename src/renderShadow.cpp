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
    ShadowMap shadowMap;

    // Generate framebuffer and depth texture
    glGenFramebuffers(1, &shadowMap.depthMapFBO);
    glGenTextures(1, &shadowMap.depthMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap.depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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
        std::cerr << "Shadow Map Framebuffer not complete for directional light " << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    // Add the shadow map to the vector
    m_shadowMap = shadowMap;
}

void Realtime::renderShadowMap(){
    for (size_t i = 0; i < m_metaData.lights.size(); ++i) {
        const SceneLightData& light = m_metaData.lights[i];
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            ShadowMap& shadowMap = m_shadowMap;
            glm::vec3 lightInvDir = -glm::normalize(glm::vec3(light.dir));

            // Compute the MVP matrix from the light's point of view
            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
            glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir * 9.f,
                                                    glm::vec3(0,0,0),
                                                    glm::vec3(0,1,0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);

            glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

            // Bias matrix
            glm::mat4 biasMatrix(
                0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 0.5, 0.0,
                0.5, 0.5, 0.5, 1.0
                );

            glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
            shadowMap.depthBiasMVP = depthBiasMVP;

            glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
            glViewport(0, 0, 1024, 1024);
            glClear(GL_DEPTH_BUFFER_BIT);
            glUseProgram(m_shadow_shader);
            GLint depthMVPLoc = glGetUniformLocation(m_shadow_shader, "depthMVP");
            glUniformMatrix4fv(depthMVPLoc, 1, GL_FALSE, &depthMVP[0][0]);

            for (size_t objIdx = 0; objIdx < m_allObjects.size(); ++objIdx) {
                const basicMapFile& object = m_allObjects[objIdx];

                GLint modelLoc = glGetUniformLocation(m_shadow_shader, "modelMatrix");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &object.modelMatrix[0][0]);

                switch (object.objectType){
                case 0: // Cube
                    glBindVertexArray(m_cube_vao);
                    glDrawArrays(GL_TRIANGLES,0,m_cube->generateShape().size()/8);
                    break;
                case 1: // Sphere
                    glBindVertexArray(m_sphere_vao);
                    glDrawArrays(GL_TRIANGLES,0,m_sphere->generateShape().size()/8);
                    break;
                }

                // Unbind VAO after drawing
                glBindVertexArray(0);
            }

            glUseProgram(0);

            glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_screen_width, m_screen_height);

    glUseProgram(m_shader);

    // Pass the depth bias MVP matrix to the shader
    GLint depthBiasMVPLoc = glGetUniformLocation(m_shader, "depthBiasMVP");
    glUniformMatrix4fv(depthBiasMVPLoc, 1, GL_FALSE, &m_shadowMap.depthBiasMVP[0][0]);

    // Bind shadow map texture
    glActiveTexture(GL_TEXTURE1); // Use texture unit 1 for shadow map
    glBindTexture(GL_TEXTURE_2D, m_shadowMap.depthMap);
    glUniform1i(glGetUniformLocation(m_shader, "shadowMap"), 1);
}


