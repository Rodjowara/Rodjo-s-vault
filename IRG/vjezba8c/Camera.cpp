#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float nearPlane, float farPlane) :
	fov(fov), nearPlane(nearPlane), farPlane(farPlane) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(
        glm::vec3(p),
        glm::vec3(p) + getLocalZ(),
        getLocalY()
    );
}

glm::mat4 Camera::getPerspectiveMatrix(float width, float height) const {
    return glm::perspective(
        glm::radians(fov),
        width / height,
        nearPlane,
        farPlane
    );
}