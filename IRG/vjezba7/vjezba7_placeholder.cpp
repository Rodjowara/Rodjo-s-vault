#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>         

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
#include "Curve.h"

Transform* controlledTransform = nullptr;

Camera camera;

std::vector<glm::vec3> controlPoints;
bool finished = false;
float lastX = 640, lastY = 400;
bool firstMouse = true;
bool pPressed = false;
bool spacePressed = false;

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
    std::string resPath("C:/Users/ivrod/Documents/irg/irglab/build/irgLab/vjezba7");
    //std::string objPath = resPath + "\\glava\\glava.obj";

    std::string objPath("C:/Users/ivrod/Documents/irg/irglab/build/primjerASSIMP/Debug/resources/glava/glava.obj");

    const aiScene* scene = importer.ReadFile(objPath.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Renderer renderer;
    camera.setScale(glm::vec3(1));

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

        Mesh* mymesh = new Mesh(vertices, indices);

        glm::vec3 min, max;
        mymesh->getBoundingBox(min, max);

        glm::vec3 center = (min + max) * 0.5f;
        float dx = max.x - min.x;
        float dy = max.y - min.y;
        float dz = max.z - min.z;
        float M = std::max(dx, std::max(dy, dz));

        for (auto& v : vertices) {
            v = (v - center) * (2.0f / M);
        }

        mymesh = new Mesh(vertices, indices);

        std::string vPathScene = resPath + "\\vertexScene.vert";
        std::string vPathProjection = resPath + "\\vertexProjection.vert";
        std::string fPath = resPath + "\\fragment.frag";
        std::string gPathScene = resPath + "\\scene.geom";
        std::string gPathProjection = resPath + "\\projection.geom";

        std::cout << vPathScene << std::endl;
        std::cout << vPathProjection << std::endl;
        std::cout << gPathScene << std::endl;
        std::cout << gPathProjection << std::endl;
        std::cout << fPath << std::endl;

        Shader* shaderScene = new Shader(vPathScene.c_str(), fPath.c_str(), gPathScene.c_str());
        Shader* shaderProjection = new Shader(vPathProjection.c_str(), fPath.c_str(), gPathProjection.c_str());

        Transform t1;
        t1.move(glm::vec3(-1, 0, 0));

        Transform t2;
        t2.move(glm::vec3(1, 0, 0));
        t2.rotate(glm::radians(45.0f), glm::vec3(0, 1, 0));

        Object* obj1 = new Object(mymesh, shaderProjection, t1);
        Object* obj2 = new Object(mymesh, shaderProjection, t2);

        renderer.addObject(obj1);
        renderer.addObject(obj2);

        controlledTransform = &camera;
    }

    Shader* curveShader = new Shader((resPath + "\\curve.vert").c_str(), (resPath + "\\curve.frag").c_str(), nullptr);

    Curve bezier = Curve(curveShader);
    bezier.setCamera(&camera);

    camera.setPosition(glm::vec3(3, 4, 1));

    float lastTime = 0.0f;
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

            bool pIsPressed = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;

            if (pIsPressed && !pPressed)
            {
                controlPoints.push_back(camera.getPosition());
            }

            pPressed = pIsPressed;

            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
                if(!finished){
                    bezier.addVertices(controlPoints);
                    bezier.calcCurves();
                    finished = true;
                }
            }

            bool spaceIsPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

            if (spaceIsPressed && !spacePressed)
            {
                bezier.toggleAnimation();
            }

            spacePressed = spaceIsPressed;
        }
        bezier.draw(view, projection);
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        bezier.animate(deltaTime);

        glm::vec3 eye = glm::vec3(camera.getModelMatrix()[3]);
        renderer.draw(view, projection, &eye);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}