#include "Object.h"

Object::Object(Mesh* mesh, Shader* shader, const Transform& transform)
	: mesh(mesh), shader(shader), transform(transform) {
}

Transform& Object::getTransform() {
	return transform;
}

void Object::setMesh(Mesh* m) {
	mesh = m;
}

void Object::setShader(Shader* s) {
	shader = s;
}

void Object::draw(const glm::mat4& view, const glm::mat4& projection) const {
	if (!mesh || !shader) return;

	shader->use();

	glm::mat4 model = transform.getModelMatrix();
	//glm::mat4 model = glm::mat4(1);

	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, &projection[0][0]);

	mesh->draw();
}