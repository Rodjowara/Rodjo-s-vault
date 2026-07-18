#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>         
#include <glm/gtx/string_cast.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <iostream>

#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "Object.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"

Transform* controlledTransform = nullptr;

Camera camera;
Light light;
Material material;

float lastX = 640, lastY = 400;
bool firstMouse = true;

float sensitivity = 0.005f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!controlledTransform) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float dx = lastX - xpos;
    float dy = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    // lijevo-desno → globalna Y
    controlledTransform->rotate(dx * sensitivity, glm::vec3(0, 1, 0));

    // gore-dolje → lokalna X
    controlledTransform->rotate(dy * sensitivity, controlledTransform->getLocalX());
}

int main(int argc, char* argv[])
{
    const int mWidth = 1280;
    const int mHeight = 800;

    Assimp::Importer importer;

    std::string path(argv[0]);
    std::string dirPath(path, 0, path.find_last_of("\\/"));
    std::string resPath("C:/Users/ivrod/Documents/irg/irglab/build/irgLab/vjezba8b");
    //std::string objPath = resPath + "\\glava\\glava.obj";

    std::string objPath("C:/Users/ivrod/Documents/irg/irglab/build/primjerASSIMP/Debug/resources/glava/glava.obj");

    const aiScene* scene = importer.ReadFile(objPath.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
    );
    if (!scene) {
        std::cerr << importer.GetErrorString();
        return -1;
    }

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(mWidth, mHeight, "App", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Renderer renderer;
    camera.setScale(glm::vec3(1));
    light.setPosition(glm::vec3(0, 0, 2));

    material.ka = glm::vec3(0.05f);
    material.kd = glm::vec3(1.0f, 0.5f, 0.0f);
    material.ks = glm::vec3(0.5f);
    material.shininess = 16.0f;

    if (scene->HasMeshes()) {

        aiMesh* mesh = scene->mMeshes[0];

        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vertices.emplace_back(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
                indices.push_back(mesh->mFaces[i].mIndices[j]);
            }
        }

        std::vector<glm::vec3> dummy;
        glm::vec3 min, max;
        {
            Mesh* temp = new Mesh(vertices, indices, dummy);
            temp->getBoundingBox(min, max);
        }

        glm::vec3 center = (min + max) * 0.5f;
        float dx = max.x - min.x;
        float dy = max.y - min.y;
        float dz = max.z - min.z;
        float M = std::max(dx, std::max(dy, dz));

        for (auto& v : vertices) {
            v = (v - center) * (2.0f / M);
        }

        std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

        for (size_t i = 0; i < indices.size(); i += 3)
        {
            auto i0 = indices[i];
            auto i1 = indices[i + 1];
            auto i2 = indices[i + 2];

            glm::vec3 v0 = vertices[i0];
            glm::vec3 v1 = vertices[i1];
            glm::vec3 v2 = vertices[i2];

            glm::vec3 faceNormal = glm::cross(v1 - v0, v2 - v0);

            normals[i0] += faceNormal;
            normals[i1] += faceNormal;
            normals[i2] += faceNormal;
        }

        for (auto& n : normals)
            n = glm::normalize(n);

        Mesh* mymesh = new Mesh(vertices, indices, normals);

        std::string vPath = resPath + "\\gouraud.vert";
        std::string fPath = resPath + "\\gouraud.frag";

        Shader* shader = new Shader(vPath.c_str(), fPath.c_str());

        Transform t1;
        t1.move(glm::vec3(-1, 0, 0));

        Transform t2;
        t2.move(glm::vec3(1, 0, 0));
        t2.rotate(glm::radians(45.0f), glm::vec3(0, 1, 0));

        Object* obj1 = new Object(mymesh, shader, t1);
        Object* obj2 = new Object(mymesh, shader, t2);

        renderer.addObject(obj1);
        renderer.addObject(obj2);

        controlledTransform = &camera;
    }

    camera.setPosition(glm::vec3(3, 4, 1));

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, mWidth, mHeight);

        glm::mat4 view = glm::inverse(camera.getModelMatrix());

        glm::mat4 projection = glm::frustum(
            -0.5f, 0.5f,
            -0.5f, 0.5f,
            1.0f, 100.0f
        );

        //glm::mat4 view = glm::mat4(1);
        //glm::mat4 projection = glm::mat4(1);

        float speed = 0.05f;

        if (controlledTransform) {

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                controlledTransform->moveForward(speed);

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                controlledTransform->moveForward(-speed);

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                controlledTransform->moveRight(speed);

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                controlledTransform->moveRight(-speed);

            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                controlledTransform->moveUp(speed);

            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                controlledTransform->moveUp(-speed);

            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                Object* controlledObject = renderer.getObject(0);
                controlledTransform = &controlledObject->getTransform();
            }

            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                Object* controlledObject = renderer.getObject(1);
                controlledTransform = &controlledObject->getTransform();
            }

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                Object* controlledObject = nullptr;
                controlledTransform = static_cast<Transform*>(&camera);
            }
        }

        glm::vec3 eye = glm::vec3(camera.getPosition());
        renderer.draw(view, projection, &eye, &light, &material);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}