#define NOMINMAX
#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include "iostream"

Mesh::Mesh(const std::vector<glm::vec3>& verts, const std::vector<unsigned int>& inds, std::vector<glm::vec3> norms)
    : vertices(verts), indices(inds), normals(norms)
{
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::setupMesh()
{
    std::cout << "normals size: " << normals.size() << std::endl;
    std::cout << "first normal: "
        << normals[0].x << " "
        << normals[0].y << " "
        << normals[0].z << std::endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO_normal);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,                  
        3,                  
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
    glBufferData(
        GL_ARRAY_BUFFER,
        normals.size() * sizeof(glm::vec3),
        normals.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        1,                 
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::getBoundingBox(glm::vec3& min, glm::vec3& max) const {
    if (vertices.empty()) return;

    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto& v : vertices) {
        min = glm::min(min, v);
        max = glm::max(max, v);
    }
}

void Mesh::applyTransform(const glm::mat4& transform) {
    for (auto& v : vertices) {
        glm::vec4 temp(v, 1.0f);
        temp = transform * temp;
        v = glm::vec3(temp);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}