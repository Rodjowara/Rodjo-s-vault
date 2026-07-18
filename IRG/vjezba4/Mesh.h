#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>  // Za OpenGL funkcije

class Mesh
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	GLuint VAO, VBO, EBO;

	void setupMesh();

public:
	Mesh(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds);
	~Mesh();

	void draw() const;

	void getBoundingBox(glm::vec3& min, glm::vec3& max) const;

	void applyTransform(const glm::mat4& transform);
};


