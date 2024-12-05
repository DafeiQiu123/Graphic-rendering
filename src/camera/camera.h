#pragma once

#include <glm/glm.hpp>
#include "utils/scenedata.h"

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    Camera();
    Camera(const SceneCameraData& cameraData,int width,int height);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec4 getCameraPos() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;
    float getWidthAngle() const;
    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;
    void updateNearFar(float near, float far);
    glm::vec3 getLook() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;
    void updateTranslation(glm::vec3 movement);
    void updateRotation(float deltaX, float deltaY);
    glm::mat3 createRotation(float angle, glm::vec3 axis);
    void updateWH(float w, float h);
private:
    SceneCameraData m_cameraData;
    glm::vec3 m_right;
    int screen_width;
    int screen_height;
    float nearPlane;
    float farPlane;
};
