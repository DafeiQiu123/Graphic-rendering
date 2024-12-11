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
void Realtime::updateVaoVbo(int p1, int p2){
    m_cube->updateParams(p1);
    m_sphere->updateParams(p1,p2);

    glBindVertexArray(m_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_cube->generateShape().size()*sizeof(GLfloat),m_cube->generateShape().data(),GL_STATIC_DRAW);
    setVAO();

    glBindVertexArray(m_sphere_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_sphere_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_sphere->generateShape().size()*sizeof(GLfloat),m_sphere->generateShape().data(),GL_STATIC_DRAW);
    setVAO();

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}
void Realtime::setVAO(){
    glEnableVertexAttribArray(0);  // Attribute location 0 for vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);  // Attribute location 1 for normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);  // Attribute location 1 for normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
}

