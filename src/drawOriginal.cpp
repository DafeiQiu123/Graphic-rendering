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

// For extra credit Adaptive level of Detail
void Realtime::paintOriginal(){
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

        // Project 6 texture related
        GLint isTextureLoc = glGetUniformLocation(m_shader,"isTexture");
        glUniform1i(isTextureLoc, material.textureMap.isUsed);
        GLint blendLoc = glGetUniformLocation(m_shader, "blend");
        glUniform1f(blendLoc, material.blend);
        if (material.textureMap.isUsed) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.textureMap.textureID);
            GLint samplerLoc = glGetUniformLocation(m_shader, "textureSampler");
            glUniform1i(samplerLoc, 0);
        }
        switch (shape.primitive.type){
            case PrimitiveType::PRIMITIVE_CUBE:
                glBindVertexArray(m_cube_vao);
                glDrawArrays(GL_TRIANGLES,0,m_cube->generateShape().size()/8);
            case PrimitiveType::PRIMITIVE_SPHERE:
                glBindVertexArray(m_sphere_vao);
                glDrawArrays(GL_TRIANGLES,0,m_sphere->generateShape().size()/8);
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                glBindVertexArray(m_cone_vao);
                glDrawArrays(GL_TRIANGLES,0,m_cone->generateShape().size()/8);
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                glBindVertexArray(m_cylinder_vao);
                glDrawArrays(GL_TRIANGLES,0,m_cylinder->generateShape().size()/8);
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
}
