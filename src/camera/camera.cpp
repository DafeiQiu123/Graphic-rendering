#include <iostream>
#include <stdexcept>
#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera() : screen_width(800), screen_height(600), nearPlane(0.1f),farPlane(100.0f) // Default screen dimensions
{
    // Initialize m_cameraData with default values
    m_cameraData.pos = glm::vec4(0.0f, 0.0f, 5.0f, 1.0f);          // Camera positioned at (0, 0, 5)
    m_cameraData.look = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);        // Looking towards negative Z-axis
    m_cameraData.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);           // Up vector along positive Y-axis
    m_cameraData.heightAngle = glm::radians(45.0f);                // Field of view of 45 degrees
    m_cameraData.aperture = 0.0f;                                  // Default aperture
    m_cameraData.focalLength = 1.0f;        // Default focal length
    m_right = glm::normalize(glm::cross(glm::vec3(m_cameraData.look),glm::vec3(m_cameraData.up)));
}

Camera::Camera(const SceneCameraData& cameraData, int width, int height): m_cameraData(cameraData),screen_width(width),screen_height(height){
    m_cameraData.look = normalize(m_cameraData.look);
    m_cameraData.up = normalize(m_cameraData.up);
    m_right = glm::normalize(glm::cross(glm::vec3(m_cameraData.look),glm::vec3(m_cameraData.up)));
}
glm::mat4 Camera::getViewMatrix() const {
    // Optional TODO: implement the getter or make your own design
    glm::vec3 pos = glm::vec3(m_cameraData.pos);
    glm::vec3 look = glm::vec3(m_cameraData.look);
    glm::vec3 up = glm::vec3(m_cameraData.up);
    glm::mat4 T = glm::mat4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        -pos[0],-pos[1],-pos[2],1);
    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - glm::dot(up,w)*w);
    glm::vec3 u = glm::cross(v,w);
    glm::mat4 R = glm::mat4(
        u[0],v[0],w[0],0,
        u[1],v[1],w[1],0,
        u[2],v[2],w[2],0,
        0,0,0,1);
    return R*T;
}

glm::mat4 Camera::getProjectionMatrix() const {
    float fovy = getHeightAngle(); // Field of view in radians
    float aspect = getAspectRatio();
    float n = nearPlane;
    float f = farPlane;

    float tanHalfFovy = tan(fovy / 2.0f);

    glm::mat4 projection = glm::mat4(0.0f);

    projection[0][0] = 1.0f / (aspect * tanHalfFovy);
    projection[1][1] = 1.0f / tanHalfFovy;
    projection[2][2] = -(f + n) / (f - n);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * f * n) / (f - n);

    return projection;
}

glm::vec4 Camera::getCameraPos() const {
    return glm::vec4(m_cameraData.pos[0],m_cameraData.pos[1],m_cameraData.pos[2],1.0f);
}

float Camera::getAspectRatio() const {
    return static_cast<float>(screen_width)/ static_cast<float>(screen_height);
}

float Camera::getHeightAngle() const {
    return m_cameraData.heightAngle;
}

float Camera::getWidthAngle() const {
    float heightAngle = getHeightAngle();
    float aspectRatio = getAspectRatio();
    return 2.0f * atan(aspectRatio * tan(heightAngle / 2.0f));
}

float Camera::getFocalLength() const {
    return m_cameraData.focalLength;
}

float Camera::getAperture() const {
    return m_cameraData.aperture;
}

void Camera::updateNearFar(float near, float far){
    nearPlane = near;
    farPlane = far;
    return;
}

glm::vec3 Camera::getLook() const {
    return m_cameraData.look;
}

glm::vec3 Camera::getUp() const {
    return m_cameraData.up;
}

glm::vec3 Camera::getRight() const {
    return m_right;
}

void Camera::updateTranslation(glm::vec3 movement) {
    m_cameraData.pos += glm::vec4(movement,0.0f);
    return;
}

void Camera::updateRotation(float deltaX, float deltaY) {
    float sensitivity = 0.005f;
    float angleX = sensitivity * deltaX;
    float angleY = sensitivity * deltaY;
    glm::mat3 rotateX = createRotation(angleX, glm::vec3(0.0f,1.0f,0.0f));
    glm::mat3 rotateY = createRotation(angleY, m_right);
    m_cameraData.look = glm::normalize(glm::vec4(rotateX * rotateY * glm::vec3(m_cameraData.look), 0.f));
    m_cameraData.up = glm::normalize(glm::vec4(rotateX * rotateY * glm::vec3(m_cameraData.up), 0.f));
    m_right = glm::normalize(glm::cross(glm::vec3(m_cameraData.look),glm::vec3(m_cameraData.up)));

    // std::cout<<m_cameraData.look.x<<"===="<<m_cameraData.look.y<<"==="<<m_cameraData.look.z<<"===||";

    // std::cout<<m_cameraData.up.x<<"===="<<m_cameraData.up.y<<"==="<<m_cameraData.up.z<<"===||";
    return;
}

glm::mat3 Camera::createRotation(float angle, glm::vec3 axis){
    float cosA = glm::cos(angle);
    float sinA = glm::sin(angle);
    float icosA = 1.0f - cosA;
    glm::vec3 u = glm::normalize(axis);
    return glm::mat3(
        cosA + u.x*u.x*icosA, u.x*u.y*icosA - u.z*sinA, u.x*u.z*icosA + u.y*sinA,
        u.x*u.y*icosA + u.z*sinA, cosA + u.y*u.y*icosA, u.y*u.z*icosA - u.x*sinA,
        u.x*u.z*icosA - u.y*sinA, u.y*u.z*icosA + u.x*sinA, cosA + u.z*u.z*icosA
        );
}

void Camera::updateWH(float w, float h){
    screen_height = h;
    screen_width = w;
    return;
}
