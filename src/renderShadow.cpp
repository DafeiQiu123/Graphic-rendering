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

void Realtime::makeShadowFBO() {
    // Create and bind the framebuffer
    glGenFramebuffers(1, &m_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);

    // Create and set up the depth texture
    glGenTextures(1, &m_shadowMapDepthTexture);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Set border color to white to avoid artifacts
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, m_shadowMapDepthTexture, 0);

    // Disable color buffer since we only need depth
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow Map Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Realtime::renderShadowMap() {

    const SceneLightData& dirLight = m_metaData.lights[dir_index];

    // Set up light's view-projection matrix
    glm::vec3 lightDir = glm::normalize(dirLight.dir);
    float orthoSize = 15.0f;  // Adjust based on your scene size

    // Light projection matrix
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize,
                                           -orthoSize, orthoSize,
                                           0.1f, 30.0f);

    // Light view matrix: position light above scene looking down
    glm::vec3 lightPos = -lightDir * 10.0f;
    glm::mat4 lightView = glm::lookAt(lightPos,
                                      glm::vec3(0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    // Complete light space matrix
    m_lightSpaceMatrix = lightProjection * lightView;

    // First pass: Render depth from light's perspective
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable front face culling for shadow pass
    glCullFace(GL_FRONT);

    glUseProgram(m_shadow_shader);

    GLint depthMVPLoc = glGetUniformLocation(m_shadow_shader, "depthMVP");
    glUniformMatrix4fv(depthMVPLoc, 1, GL_FALSE, &m_lightSpaceMatrix[0][0]);

    // Get the model matrix location ONCE before the loop
    GLint modelMatrixLoc = glGetUniformLocation(m_shadow_shader, "modelMatrix");

    // Render all objects in m_allObjects
    for (const basicMapFile& object : m_allObjects) {
        // Update model matrix for each object
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &object.modelMatrix[0][0]);

        glBindVertexArray(object.vao);

        // Draw based on object type
        switch (object.objectType) {
        case 0: // Cube
        case 2:
        case 5:
        case 6:
            glDrawArrays(GL_TRIANGLES, 0, m_cube->generateShape().size()/8);
            break;
        case 1: // Sphere
            glDrawArrays(GL_TRIANGLES, 0, m_sphere->generateShape().size()/8);
            break;
        case 3: // Dragon mesh
            glDrawArrays(GL_TRIANGLES, 0, m_mesh_dragon->getVertexData().size()/8);
            break;
        case 4: // Bunny mesh
            glDrawArrays(GL_TRIANGLES, 0, m_mesh_bunny->getVertexData().size()/8);
            break;
        }
    }

    // Reset states
    glBindVertexArray(0);
    glCullFace(GL_BACK);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}
