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
    addUV(m_vertexData, topLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal1);
    addUV(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal1);
    addUV(m_vertexData, bottomRight);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal2);
    addUV(m_vertexData, bottomRight);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal2);
    addUV(m_vertexData, topRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal2);
    addUV(m_vertexData, topLeft);

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    int N = m_param1;
    float du = 1.0f / N; // vertical
    float dv = 1.0f / N; // horizontal

    // Loop over each subdivision in the vertical direction
    for (int i = 0; i < N; i++) {
        float s0 = i * du;
        float s1 = (i + 1) * du;

        // Interpolate points along the left and right edges
        glm::vec3 leftEdgeTop = glm::mix(topLeft, bottomLeft, s0);
        glm::vec3 leftEdgeBottom = glm::mix(topLeft, bottomLeft, s1);
        glm::vec3 rightEdgeTop = glm::mix(topRight, bottomRight, s0);
        glm::vec3 rightEdgeBottom = glm::mix(topRight, bottomRight, s1);

        // Loop over each subdivision in the horizontal direction
        for (int j = 0; j < N; j++) {
            float t0 = j * dv;
            float t1 = (j + 1) * dv;

            // Interpolate points between the left and right edges
            glm::vec3 p00 = glm::mix(leftEdgeTop, rightEdgeTop, t0);
            glm::vec3 p01 = glm::mix(leftEdgeTop, rightEdgeTop, t1);
            glm::vec3 p10 = glm::mix(leftEdgeBottom, rightEdgeBottom, t0);
            glm::vec3 p11 = glm::mix(leftEdgeBottom, rightEdgeBottom, t1);

            // Create a tile from the four corner points
            makeTile(p00, p01, p10, p11);
        }
    }
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

void Cube::addUV(std::vector<float> &data, glm::vec3 p) {
    glm::vec2 uv = glm::vec2(0.0f,0.0f);
    if (p.x == 0.5f) uv = glm::vec2(0.5-p.z,0.5+p.y);
    else if (p.x == -0.5f) uv = glm::vec2(0.5+p.z,0.5+p.y);
    else if (p.y == 0.5f) uv = glm::vec2(0.5+p.x,0.5-p.z);
    else if (p.y == -0.5f) uv = glm::vec2(0.5+p.x,0.5+p.z);
    else if (p.z == 0.5f) uv = glm::vec2(0.5+p.x,0.5+p.y);
    else uv = glm::vec2(0.5-p.x,0.5+p.y);
    data.push_back(glm::clamp(uv[0],0.f,1.f));
    data.push_back(glm::clamp(uv[1],0.f,1.f));
}

