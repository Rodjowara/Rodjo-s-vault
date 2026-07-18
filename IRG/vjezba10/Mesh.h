#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>  // Za OpenGL funkcije

class Mesh
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> uvmap;

	GLuint VAO, VBO, EBO, NBO, UVBO;

public:
	Mesh(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds, 
		const std::vector<glm::vec3> norms, const std::vector<glm::vec2> uvs);
	~Mesh();
	void setupMesh();
	void draw() const;

	void getBoundingBox(glm::vec3& min, glm::vec3& max) const;

	void applyTransform(const glm::mat4& transform);
};