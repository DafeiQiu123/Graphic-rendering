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

void Realtime::updateVaoVboHalf(int p1, int p2){
    m_cube_half->updateParams(p1);
    m_sphere_half->updateParams(p1, p2);
    m_cone_half->updateParams(p1, p2);
    m_cylinder_half->updateParams(p1, p2);

    glBindVertexArray(m_cube_vao_half);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo_half);
    glBufferData(GL_ARRAY_BUFFER, m_cube_half->generateShape().size() * sizeof(GLfloat), m_cube_half->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_sphere_vao_half);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo_half);
    glBufferData(GL_ARRAY_BUFFER, m_sphere_half->generateShape().size() * sizeof(GLfloat), m_sphere_half->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_cone_vao_half);
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo_half);
    glBufferData(GL_ARRAY_BUFFER, m_cone_half->generateShape().size() * sizeof(GLfloat), m_cone_half->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_cylinder_vao_half);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo_half);
    glBufferData(GL_ARRAY_BUFFER, m_cylinder_half->generateShape().size() * sizeof(GLfloat), m_cylinder_half->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::updateVaoVboMin(int p1, int p2){
    m_cube_min->updateParams(p1);
    m_sphere_min->updateParams(p1, p2);
    m_cone_min->updateParams(p1, p2);
    m_cylinder_min->updateParams(p1, p2);

    glBindVertexArray(m_cube_vao_min);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo_min);
    glBufferData(GL_ARRAY_BUFFER, m_cube_min->generateShape().size() * sizeof(GLfloat), m_cube_min->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_sphere_vao_min);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo_min);
    glBufferData(GL_ARRAY_BUFFER, m_sphere_min->generateShape().size() * sizeof(GLfloat), m_sphere_min->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_cone_vao_min);
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo_min);
    glBufferData(GL_ARRAY_BUFFER, m_cone_min->generateShape().size() * sizeof(GLfloat), m_cone_min->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_cylinder_vao_min);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo_min);
    glBufferData(GL_ARRAY_BUFFER, m_cylinder_min->generateShape().size() * sizeof(GLfloat), m_cylinder_min->generateShape().data(), GL_STATIC_DRAW);
    setVAO();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
