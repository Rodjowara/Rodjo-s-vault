#include "Renderer.h"

Renderer::Renderer() {}

void Renderer::addObject(Object* obj) {
	if (obj) {
		objects.push_back(obj);
	}
}

void Renderer::draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3* eye) const {
	for (const auto& object : objects) {
		if (object)
			object->draw(view, projection, eye);
	}
}

Object* Renderer::getObject(int index) {
	return objects[index];
}