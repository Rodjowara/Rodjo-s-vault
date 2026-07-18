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

void Object::draw(const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3* eye) const
{
    if (!mesh || !shader) return;

    shader->use();

    glm::mat4 model = transform.getModelMatrix();

    GLint modelLoc = glGetUniformLocation(shader->ID, "model");
    if (modelLoc != -1)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    GLint viewLoc = glGetUniformLocation(shader->ID, "view");
    if (viewLoc != -1)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    GLint projLoc = glGetUniformLocation(shader->ID, "projection");
    if (projLoc != -1)
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    if (eye)
    {
        GLint eyeLoc = glGetUniformLocation(shader->ID, "viewPos");
        if (eyeLoc != -1)
            glUniform3fv(eyeLoc, 1, glm::value_ptr(*eye));
    }

    mesh->draw();
}

void Object::draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3* eye, const Light* light, const Material* material) const {
    if (!mesh || !shader) return;

    shader->use();

    glm::mat4 model = transform.getModelMatrix();

    GLint modelLoc = glGetUniformLocation(shader->ID, "model");
    if (modelLoc != -1)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    GLint viewLoc = glGetUniformLocation(shader->ID, "view");
    if (viewLoc != -1)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    GLint projLoc = glGetUniformLocation(shader->ID, "projection");
    if (projLoc != -1)
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    if (eye)
    {
        GLint eyeLoc = glGetUniformLocation(shader->ID, "viewPos");
        if (eyeLoc != -1)
            glUniform3fv(eyeLoc, 1, glm::value_ptr(*eye));
    }

    if (light)
    {
        glUniform3fv(glGetUniformLocation(shader->ID, "lightPos"), 1,
            glm::value_ptr(light->getPosition()));

        glUniform3fv(glGetUniformLocation(shader->ID, "Ia"), 1,
            glm::value_ptr(light->Ia));

        glUniform3fv(glGetUniformLocation(shader->ID, "Id"), 1,
            glm::value_ptr(light->Id));

        glUniform3fv(glGetUniformLocation(shader->ID, "Is"), 1,
            glm::value_ptr(light->Is));
    }

    if (material)
    {
        glUniform3fv(glGetUniformLocation(shader->ID, "ka"), 1,
            glm::value_ptr(material->ka));

        glUniform3fv(glGetUniformLocation(shader->ID, "kd"), 1,
            glm::value_ptr(material->kd));

        glUniform3fv(glGetUniformLocation(shader->ID, "ks"), 1,
            glm::value_ptr(material->ks));

        glUniform1f(glGetUniformLocation(shader->ID, "shininess"),
            material->shininess);
    }

    mesh->draw();
}