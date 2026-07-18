#pragma once

#include "Transform.h"
#include <glm/glm.hpp>

class Camera: public Transform
{
private:
	float fov;
	float nearPlane;
	float farPlane;

public:
	Camera(float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getPerspectiveMatrix(float width, float height) const;
};