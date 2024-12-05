#include "Sphere.h"
#include <cmath>
void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = 25;
    m_param2 = 25;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    addUV(m_vertexData, topLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    addUV(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    addUV(m_vertexData, bottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    addUV(m_vertexData, topLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    addUV(m_vertexData, bottomRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    addUV(m_vertexData, topRight);

}

glm::vec3 Sphere::getSpherePoint(float phi, float theta) {
    float x = m_radius * sin(phi) * cos(theta);
    float y = m_radius * cos(phi);
    float z = -m_radius * sin(phi) * sin(theta);
    return glm::vec3(x, y, z);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float phiStep =  M_PI / m_param1; // From 0 to pi
    float theta0 = currentTheta;
    float theta1 = nextTheta;

    for (int i = 0; i < m_param1; i++) {
        float phi0 = i * phiStep;
        float phi1 = (i + 1) * phiStep;
        glm::vec3 topLeft = getSpherePoint(phi0, theta0);
        glm::vec3 topRight = getSpherePoint(phi0, theta1);
        glm::vec3 bottomLeft = getSpherePoint(phi1, theta0);
        glm::vec3 bottomRight = getSpherePoint(phi1, theta1);
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = 2.0f * M_PI / m_param2;

    // Loop over each longitude division
    for (int i = 0; i < m_param2; i++) {
        float currentTheta = thetaStep * i;
        float nextTheta = thetaStep * (i + 1);

        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Sphere::addUV(std::vector<float> &data, glm::vec3 p) {
    glm::vec2 uv = glm::vec2(0.0f,0.0f);
    uv = glm::vec2(0.5f + (atan2(p.z, -p.x) / (2.0f * M_PI)) , 0.5f + (asin(2.0f * (p.y)) / M_PI));
    data.push_back(uv[0]);
    data.push_back(uv[1]);
}
