#pragma once

#include <vector>
#include "Object.h"

class Renderer
{
private:
	std::vector<Object*> objects;
	GLuint depthMapFBO;
	Shader* depthShader;

public:
	GLuint depthMap;
	Renderer();
	void addDepthShader(Shader* ds);
	void addObject(Object* obj);
	void draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3* eye, const Light* light, const Material* material) const;
	void drawDepth(Shader* depthShader, const glm::mat4& ligthSpaceMatrix) const;
	void initShadowMap(int width, int height);
	void beginShadowPass(int width, int height);
	void endShadowPass(int width, int height);
	Object* getObject(int index);
};