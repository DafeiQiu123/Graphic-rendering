#include "Cone.h"
#include "cmath"
void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = fmax(param1,1);
    m_param2 = fmax(param2,3);
    setVertexData();
}

glm::vec3 normalizeConeSide(glm::vec3 xyz){
    return normalize(glm::vec3(2.0f*xyz.x, -0.5*xyz.y + 0.25, 2.0f*xyz.z));
}
void Cone::makeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight){
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalizeConeSide(topLeft));
    addUV(m_vertexData, topLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalizeConeSide(bottomLeft));
    addUV(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalizeConeSide(bottomRight));
    addUV(m_vertexData, bottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalizeConeSide(topLeft));
    addUV(m_vertexData, topLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalizeConeSide(bottomRight));
    addUV(m_vertexData, bottomRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalizeConeSide(topRight));
    addUV(m_vertexData, topRight);

}
void Cone::makeBottomTile(glm::vec3 topLeft,
                          glm::vec3 topRight,
                          glm::vec3 bottomLeft,
                          glm::vec3 bottomRight){
    glm::vec3 normal = glm::vec3(0.0f,-1.0f,0.0f);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, topLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, topRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, topRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, bottomRight);
}

void Cone::edgeSideTile(glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight){
    glm::vec3 bln = bottomLeft;
    glm::vec3 brn = bottomRight;
    glm::vec3 average = glm::normalize(glm::vec3(bln[0]+brn[0], 0, bln[2] +brn[2]));
    glm::vec3 tn = glm::normalize(glm::vec3(average[0],0.5f,average[2]));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, tn);
    addUV(m_vertexData, topRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalizeConeSide(bottomLeft));
    addUV(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalizeConeSide(bottomRight));
    addUV(m_vertexData, bottomRight);
}
void Cone::edgeBottomTile(glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight){
    glm::vec3 normal = glm::vec3(0.0f,-1.0f,0.0f);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, topRight);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    addUV(m_vertexData, bottomRight);
}
void Cone::makeCone(float currentTheta, float nextTheta){
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
        float h0 = 0.5 - (i + 1)* heightStep;
        float h1 = 0.5 - i * heightStep;
        // Top & bottom
        float dx1 = i * radiusStepx1;
        float dx1s = (i+1) * radiusStepx1;
        float dz1 = i * radiusStepz1;
        float dz1s = (i+1) * radiusStepz1;
        float dx0 = i * radiusStepx0;
        float dx0s = (i+1) * radiusStepx0;
        float dz0 = i * radiusStepz0;
        float dz0s = (i+1) * radiusStepz0;
        // side
        glm::vec3 topLeft = glm::vec3(dx0,h1,dz0);
        glm::vec3 topRight = glm::vec3(dx1,h1,dz1);
        glm::vec3 bottomLeft = glm::vec3(dx0s,h0,dz0s);
        glm::vec3 bottomRight = glm::vec3(dx1s,h0,dz1s);
        if (i == 0){edgeSideTile(topRight,bottomLeft,bottomRight);}
        else {makeSideTile(topLeft, topRight, bottomLeft, bottomRight);}
        // bottom
        topLeft = glm::vec3(dx1,-0.5f,dz1);
        topRight = glm::vec3(dx0,-0.5f,dz0);
        bottomLeft = glm::vec3(dx1s,-0.5f,dz1s);
        bottomRight = glm::vec3(dx0s,-0.5f,dz0s);
        if (i == 0){edgeBottomTile(topRight, bottomLeft, bottomRight);}
        else {makeBottomTile(topLeft, topRight, bottomLeft, bottomRight);}
    }
}

void Cone::setVertexData() {
    float thetaStep = 2 * M_PI / m_param2;

    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        makeCone(currentTheta, nextTheta);
    }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cone::addUV(std::vector<float> &data, glm::vec3 p) {
    glm::vec2 uv = glm::vec2(0.0f,0.0f);
    if (p.y == -0.5f) uv = glm::vec2(0.5+p.x,0.5+p.z);
    else {
        uv[1] = 0.5 + p.y;
        uv[0] = 0.5 + (atan2(p.z, p.x) / (2.0f * M_PI));
    }
    data.push_back(uv[0]);
    data.push_back(uv[1]);
}
