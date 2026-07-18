#pragma once

#include <glm/glm.hpp>            
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>   
class Transform
{
	friend class Camera;
private:
	glm::vec4 x;
	glm::vec4 y;
	glm::vec4 z;
	glm::vec4 p;	//position
	glm::vec4 s;	//scale

public:
	Transform();

	void setPosition(const glm::vec3& pos);
	void move(const glm::vec3& delta);
	void rotate(float angle, const glm::vec3& axis);
	void setScale(const glm::vec3& scale);
	void scale(const glm::vec3& factor);

	glm::mat4 getModelMatrix() const;
	glm::mat4 getModelMatrixMyGLM() const;

	void moveForward(float amount);
	void moveRight(float amount);
	void moveUp(float amount);

	glm::vec3 getLocalX() const;
	glm::vec3 getLocalY() const;
	glm::vec3 getLocalZ() const;
	glm::vec3 getPosition() const;
};