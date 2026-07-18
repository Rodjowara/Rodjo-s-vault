#define NOMINMAX
#include "Curve.h"
#include "Shader.h"
#include <algorithm>
#include <iostream>

Curve::Curve(Shader* sjenchar) {
    shader = sjenchar;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Curve::~Curve() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Curve::addVertices(const std::vector<glm::vec3>& points) {
    std::vector<glm::vec3> temp;
    temp = points;
    temp.push_back(points[0]);
    vertices = temp;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(glm::vec3),
        vertices.data(),
        GL_DYNAMIC_DRAW);
}

void Curve::calcCurves() {
    curveAprox = approxBezier(100);
    curveInter = interBezier(100);
}

void Curve::draw(glm::mat4 view, glm::mat4 projection) {
    //std::cout << "verts: " << vertices.size()
        //<< " approx: " << curveAprox.size()
        //<< " inter: " << curveInter.size() << std::endl;

    if (vertices.empty())
        return;

    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    shader->setUniform("model", glm::mat4(1.0f));

    glBindVertexArray(VAO);
    uploadVertices(vertices);
    shader->setUniform("uColor", glm::vec3(0.5,0.5,0.5));
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    shader->setUniform("model", glm::mat4(1.0f));
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

    uploadVertices(curveAprox);
    shader->setUniform("uColor", glm::vec3(1, 0, 0));
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    shader->setUniform("model", glm::mat4(1.0f));
    glDrawArrays(GL_LINE_STRIP, 0, curveAprox.size());

    uploadVertices(curveInter);
    shader->setUniform("uColor", glm::vec3(0, 1, 0));
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    shader->setUniform("model", glm::mat4(1.0f));
    glDrawArrays(GL_LINE_STRIP, 0, curveInter.size());

    glBindVertexArray(0);
}

void Curve::animate(float deltaTime) {
    if (!playing || curveInter.empty() || camera == nullptr)
        return;

    t += speed * deltaTime;

    if (t > 1.0f)
        t = 0.0f;

    int n = curveInter.size();

    float scaled = t * (n - 1);

    int i = (int)scaled;
    int i2 = std::min(i + 1, n - 1);

    float localT = scaled - i;

    glm::vec3 position =
        (1 - localT) * curveInter[i] +
        localT * curveInter[i2];

    camera->setPosition(position);
}

int Curve::binomial(int n, int k) {
    int res = 1;

    if (k > n - k) k = n - k;

    for (int i = 0; i < k; i++) {
        res *= (n - i);
        res /= (i+1);
    }

    return res;
}

std::vector<glm::vec3> Curve::approxBezier(int samples) {
    std::vector<glm::vec3> curve;
    int n = vertices.size() - 1;

    if (n < 1) return curve;

    for (int s = 0; s <= samples; s++) {
        float t = (float)s / samples;

        glm::vec3 point(0.0f);

        for (int i = 0; i <= n; i++) {
            float bernstein = binomial(n, i) * pow(1 - t, n - i) * pow(t, i);
            point += bernstein * vertices[i];
        }

        curve.push_back(point);
    }

    return curve;
}

std::vector<glm::vec3> Curve::interBezier(int samples) {
    std::vector<glm::vec3> curve;
    std::vector<glm::vec3> points;

    if (vertices.size() < 4) return curve;

    for (int i = vertices.size() - 4; i < vertices.size(); i++) {
        points.push_back(vertices[i]);
    }

    glm::mat4 matrix(
        -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 3, 0, 0,
        1, 0, 0, 0
    );

    for (int s = 0; s <= samples; s++) {
        float t = (float)s / samples;

        glm::vec4 T(
            t*t*t,
            t*t,
            t,
            1.0f
        );

        float b0 = glm::dot(T, glm::vec4(matrix[0]));
        float b1 = glm::dot(T, glm::vec4(matrix[1]));
        float b2 = glm::dot(T, glm::vec4(matrix[2]));
        float b3 = glm::dot(T, glm::vec4(matrix[3]));

        glm::vec3 point = b0 * points[0] + b1 * points[1] + b2 * points[2] + b3 * points[3];

        curve.push_back(point);
    }
    return curve;
}

void Curve::uploadVertices(const std::vector<glm::vec3>& verts)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(glm::vec3),
        verts.data(),
        GL_DYNAMIC_DRAW
    );
}

void Curve::setCamera(Camera* cam) {
    camera = cam;
}

void Curve::toggleAnimation() {
    playing = !playing;
}