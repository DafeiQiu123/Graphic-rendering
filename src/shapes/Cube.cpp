#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(param1,1);
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal1 = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft));
    glm::vec3 normal2 = glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal1);
    m_vertexData.push_back(0);
    m_vertexData.push_back(1);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal1);
    m_vertexData.push_back(0);
    m_vertexData.push_back(0);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal1);
    m_vertexData.push_back(1);
    m_vertexData.push_back(0);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal2);
    m_vertexData.push_back(1);
    m_vertexData.push_back(0);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal2);
    m_vertexData.push_back(1);
    m_vertexData.push_back(1);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal2);
    m_vertexData.push_back(0);
    m_vertexData.push_back(1);

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    makeTile(topLeft,topRight,bottomLeft,bottomRight);
}

void Cube::setVertexData() {
    // Back face (z = 0.5)
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
    // Back face (z = -0.5)
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),  // top left
             glm::vec3(-0.5f,  0.5f, -0.5f),  // top right
             glm::vec3( 0.5f, -0.5f, -0.5f),  // bottom left
             glm::vec3(-0.5f, -0.5f, -0.5f)); // bottom right

    // Left face (x = -0.5)
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),  // top left
             glm::vec3(-0.5f,  0.5f,  0.5f),  // top right
             glm::vec3(-0.5f, -0.5f, -0.5f),  // bottom left
             glm::vec3(-0.5f, -0.5f,  0.5f)); // bottom right

    // Right face (x = 0.5)
    makeFace(glm::vec3(0.5f,  0.5f,  0.5f),   // top left
             glm::vec3(0.5f,  0.5f, -0.5f),   // top right
             glm::vec3(0.5f, -0.5f,  0.5f),   // bottom left
             glm::vec3(0.5f, -0.5f, -0.5f));  // bottom right

    // Top face (y = 0.5)
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),   // top left
             glm::vec3( 0.5f, 0.5f, -0.5f),   // top right
             glm::vec3(-0.5f, 0.5f,  0.5f),   // bottom left
             glm::vec3( 0.5f, 0.5f,  0.5f));  // bottom right

    // Bottom face (y = -0.5)
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),  // top left
             glm::vec3( 0.5f, -0.5f,  0.5f),  // top right
             glm::vec3(-0.5f, -0.5f, -0.5f),  // bottom left
             glm::vec3( 0.5f, -0.5f, -0.5f)); // bottom right
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}


