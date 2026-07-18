#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Camera.h"
#include "Shader.h"

class Curve
{
private:
	GLuint VAO, VBO;
	Shader* shader;
	float t = 0.0f;
	float speed = 0.5f;
	bool playing = false;
	Camera* camera = nullptr;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> curveAprox;
	std::vector<glm::vec3> curveInter;

	int binomial(int n, int k);
	std::vector<glm::vec3> approxBezier(int samples);
	std::vector<glm::vec3> interBezier(int samples);
	void uploadVertices(const std::vector<glm::vec3>& verts);
public:
	Curve(Shader* sjenchar);
	~Curve();
	void addVertices(const std::vector<glm::vec3>& points);
	void draw(glm::mat4 view, glm::mat4 projection);
	void animate(float deltaTime);
	void calcCurves();
	void setCamera(Camera* cam);
	void toggleAnimation();
};