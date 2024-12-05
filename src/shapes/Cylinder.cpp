#include "Cylinder.h"
#include "cmath"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(param1,1);
    m_param2 = fmax(param2,3);
    setVertexData();
}
glm::vec3 normalizeCylinderSide(glm::vec3 xyz){
    return normalize(glm::vec3(xyz.x,0,xyz.z));
}
void Cylinder::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalizeCylinderSide(topLeft));
    addUV(m_vertexData,topLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalizeCylinderSide(bottomLeft));
    addUV(m_vertexData,bottomLeft);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalizeCylinderSide(topRight));
    addUV(m_vertexData,topRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalizeCylinderSide(topRight));
    addUV(m_vertexData,topRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalizeCylinderSide(bottomLeft));
    addUV(m_vertexData,bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalizeCylinderSide(bottomRight));
    addUV(m_vertexData,bottomRight);

}
void Cylinder::makeTopBottomTile(glm::vec3 topLeft,
                                 glm::vec3 topRight,
                                 glm::vec3 bottomLeft,
                                 glm::vec3 bottomRight,
                                 float normal_y) {
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,topLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,bottomLeft);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,topRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,topRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,topLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,topRight);

}
void Cylinder::edgeTile(glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight, float normal_y){
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,topRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::vec3(0.0f,normal_y,0.0f));
    addUV(m_vertexData,bottomRight);
}
void Cylinder::makeCylinder(float currentTheta, float nextTheta) {
    float heightStep = 1.0f / m_param1;
    float x1 = m_radius*cos(currentTheta);
    float z1 = m_radius*sin(currentTheta);
    float x0 = m_radius*cos(nextTheta);
    float z0 = m_radius*sin(nextTheta);
    float radiusStepx1 = x1 / m_param1;
    float radiusStepz1 = z1 / m_param1;
    float radiusStepx0 = x0 / m_param1;
    float radiusStepz0 = z0 / m_param1;
    for (int i = 0; i < m_param1; i++){
        float h0 = i * heightStep - 0.5;
        float h1 = (i + 1) * heightStep -0.5;
        // Side
        glm::vec3 topLeft = glm::vec3(x0,h1,z0);
        glm::vec3 topRight = glm::vec3(x1,h1,z1);
        glm::vec3 bottomLeft = glm::vec3(x0,h0,z0);
        glm::vec3 bottomRight = glm::vec3(x1,h0,z1);
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
        // Top & bottom
        float dx1 = i * radiusStepx1;
        float dx1s = (i+1) * radiusStepx1;
        float dz1 = i * radiusStepz1;
        float dz1s = (i+1) * radiusStepz1;
        float dx0 = i * radiusStepx0;
        float dx0s = (i+1) * radiusStepx0;
        float dz0 = i * radiusStepz0;
        float dz0s = (i+1) * radiusStepz0;
        // top
        topLeft = glm::vec3(dx0,0.5f,dz0);
        topRight = glm::vec3(dx1,0.5f,dz1);
        bottomLeft = glm::vec3(dx0s,0.5f,dz0s);
        bottomRight = glm::vec3(dx1s,0.5f,dz1s);
        if (i == 0){edgeTile(topRight, bottomLeft, bottomRight,1.0f);}
        else {makeTopBottomTile(topLeft, topRight, bottomLeft, bottomRight,1.0f);}
        // bottom
        topLeft = glm::vec3(dx1,-0.5f,dz1);
        topRight = glm::vec3(dx0,-0.5f,dz0);
        bottomLeft = glm::vec3(dx1s,-0.5f,dz1s);
        bottomRight = glm::vec3(dx0s,-0.5f,dz0s);
        if (i == 0){edgeTile(topRight, bottomLeft, bottomRight,-1.0f);}
        else {makeTopBottomTile(topLeft, topRight, bottomLeft, bottomRight,-1.0f);}
    }
}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float thetaStep = 2 * M_PI / m_param2;
    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeCylinder(currentTheta, nextTheta);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cylinder::addUV(std::vector<float> &data, glm::vec3 p) {
    glm::vec2 uv = glm::vec2(0.0f,0.0f);
    if (glm::abs(p.y - 0.5f) < 1e-4f) uv = glm::vec2(0.5+p.x,0.5-p.z);
    else if (glm::abs(p.y + 0.5f) < 1e-4f) uv = glm::vec2(0.5+p.x,0.5+p.z);
    else {
        uv[1] = 0.5 + p.y;
        uv[0] = 0.5 + (atan2(p.z, -p.x) / (2.0f * M_PI));
    }
    data.push_back(uv[0]);
    data.push_back(uv[1]);
}
