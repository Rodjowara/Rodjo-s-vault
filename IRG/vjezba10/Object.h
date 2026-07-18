#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "Light.h"
#include "Material.h"

class Object
{
private:
	Mesh* mesh;
	Transform transform;
	Shader* shader;

public:
	Object(Mesh* mesh, Shader* shader, const Transform& transform);

	Transform& getTransform();

	void setMesh(Mesh* m);
	void setShader(Shader* s);

	void draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3* eye) const;
	void draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3* eye, const Light* light, const Material* material) const;
	void drawDepth(Shader* depthShader, const glm::mat4& lightSpaceMatrix) const;
};