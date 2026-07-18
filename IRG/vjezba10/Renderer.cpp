#include "Renderer.h"

Renderer::Renderer() {}

void Renderer::addDepthShader(Shader* ds){
	depthShader = ds;
}

void Renderer::addObject(Object* obj) {
	if (obj) {
		objects.push_back(obj);
	}
}

void Renderer::draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3* eye, const Light* light, const Material* material) const {
	for (const auto& object : objects) {
		if (object)
			object->draw(view, projection, eye, light, material);
	}
}

void Renderer::drawDepth(Shader* depthShader, const glm::mat4& lightSpaceMatrix) const {
	for (auto o : objects) {
		o->drawDepth(depthShader, lightSpaceMatrix);
	}
}

void Renderer::initShadowMap(int width, int height) {
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float border[] = { 1,1,1,1 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthMap,
        0
    );

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::beginShadowPass(int width, int height) {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::endShadowPass(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

Object* Renderer::getObject(int index) {
	return objects[index];
}