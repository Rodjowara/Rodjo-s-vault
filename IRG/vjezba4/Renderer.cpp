#include "Renderer.h"

Renderer::Renderer() {}

void Renderer::addObject(Object* obj) {
	if (obj) {
		objects.push_back(obj);
	}
}

void Renderer::draw() const{
	for (const auto& object : objects) {
		if(object)
			object->draw();
	}
}