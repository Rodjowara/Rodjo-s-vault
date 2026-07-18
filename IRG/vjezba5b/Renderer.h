#pragma once

#include <vector>
#include "Object.h"
class Renderer
{
private:
	std::vector<Object*> objects;

public:
	Renderer();
	void addObject(Object* obj);
	void draw(const glm::mat4& view, const glm::mat4& projection) const;
	Object* getObject(int index);
};