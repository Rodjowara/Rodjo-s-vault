#include "Transform.h"
#include "MyGLM.h"

Transform::Transform() {
	x = glm::vec4(1, 0, 0, 0);
	y = glm::vec4(0, 1, 0, 0);
	z = glm::vec4(0, 0, 1, 0);

	p = glm::vec4(0, 0, 0, 1);
	s = glm::vec4(1, 1, 1, 0);
}

void Transform::setPosition(const glm::vec3& pos) {
	p = glm::vec4(pos, 1.0f);
}

void Transform::move(const glm::vec3& delta) {
	p += glm::vec4(delta, 0.0f);
}

void Transform::setScale(const glm::vec3& scale) {
	s = glm::vec4(scale, 0.0f);
}

void Transform::scale(const glm::vec3& factor) {
	s *= glm::vec4(factor, 0.0f);
}

void Transform::rotate(float angle, const glm::vec3& axis) {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);

	x = R * x;
	y = R * y;
	z = R * z;
}

glm::mat4 Transform::getModelMatrix() const {
	glm::mat4 model(1.0f);

	glm::vec3 sx = glm::vec3(x.x, x.y, x.z) * s.x;
	glm::vec3 sy = glm::vec3(y.x, y.y, y.z) * s.y;
	glm::vec3 sz = glm::vec3(z.x, z.y, z.z) * s.z;

	model[0] = glm::vec4(sx, 0.0f);
	model[1] = glm::vec4(sy, 0.0f);
	model[2] = glm::vec4(sz, 0.0f);
	model[3] = p;

	return model;
}

glm::mat4 Transform::getModelMatrixMyGLM() const {
	glm::mat4 T = MyGLM::translate3D(glm::vec3(p));

	glm::vec3 axis = glm::normalize(glm::vec3(x.x, x.y, x.z));
	float angle = 0.0f;
	glm::mat4 R = MyGLM::rotate3D(axis, angle);

	glm::mat4 S = MyGLM::scale3D(glm::vec3(s));

	return T * R * S;
}

void Transform::moveForward(float amount) {
	p += -z * amount; // negativna Z = smjer pogleda
}

void Transform::moveRight(float amount) {
	p += x * amount;
}

void Transform::moveUp(float amount) {
	p += y * amount;
}

glm::vec3 Transform::getLocalX() const {
	return glm::vec3(x.x, x.y, x.z);
}

glm::vec3 Transform::getLocalY() const {
	return glm::vec3(y.x, y.y, y.z);
}

glm::vec3 Transform::getLocalZ() const {
	return glm::vec3(z.x, z.y, z.z);
}