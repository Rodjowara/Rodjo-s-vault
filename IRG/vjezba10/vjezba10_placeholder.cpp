#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>         
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    std::string resPath("C:/Users/ivrod/Documents/irg/irglab/build/irgLab/vjezba10");
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
    light.setPosition(glm::vec3(0, 2, 2));

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
        std::vector<glm::vec2> dummy2;
        glm::vec3 min, max;
        {
            Mesh* temp = new Mesh(vertices, indices, dummy, dummy2);
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
        std::vector<glm::vec2> uvs;

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

        if (mesh->mTextureCoords[0]) {
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                uvs.push_back(glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                ));
            }
        }

        Mesh* mymesh = new Mesh(vertices, indices, normals, uvs);

        std::string vPath = resPath + "\\phong.vert";
        std::string fPath = resPath + "\\phong.frag";

        Shader* shader = new Shader(vPath.c_str(), fPath.c_str());

        Transform t1;
        t1.move(glm::vec3(-1, 0, 0));

        Transform t2;
        t2.move(glm::vec3(1, 0, 0));
        t2.rotate(glm::radians(45.0f), glm::vec3(0, 1, 0));

        Transform t3;
        t3.move(glm::vec3(-4, 0, 0));

        Transform t4;
        t4.move(glm::vec3(4, 0, 0));

        Transform t5;
        t5.move(glm::vec3(-6, 0, 0));

        Transform t6;
        t6.move(glm::vec3(6, 0, 0));

        Transform t7;
        t7.move(glm::vec3(-8, 0, 0));

        Transform t8;
        t8.move(glm::vec3(8, 0, 0));

        Transform t9;
        t9.move(glm::vec3(-10, 0, 0));

        Transform t10;
        t10.move(glm::vec3(10, 0, 0));

        Transform t11;
        t11.move(glm::vec3(-12, 0, 0));

        Transform t12;
        t12.move(glm::vec3(12, 0, 0));

        Object* obj1 = new Object(mymesh, shader, t1);
        Object* obj2 = new Object(mymesh, shader, t2);
        Object* obj3 = new Object(mymesh, shader, t3);
        Object* obj4 = new Object(mymesh, shader, t4);
        Object* obj5 = new Object(mymesh, shader, t5);
        Object* obj6 = new Object(mymesh, shader, t6);
        Object* obj7 = new Object(mymesh, shader, t7);
        Object* obj8 = new Object(mymesh, shader, t8);
        Object* obj9 = new Object(mymesh, shader, t9);
        Object* obj10 = new Object(mymesh, shader, t10);
        Object* obj11 = new Object(mymesh, shader, t11);
        Object* obj12 = new Object(mymesh, shader, t12);

        renderer.addObject(obj1);
        renderer.addObject(obj2);
        renderer.addObject(obj3);
        renderer.addObject(obj4);
        renderer.addObject(obj5);
        renderer.addObject(obj6);
        renderer.addObject(obj7);
        renderer.addObject(obj8);
        renderer.addObject(obj9);
        renderer.addObject(obj10);
        renderer.addObject(obj11);
        renderer.addObject(obj12);

        controlledTransform = &camera;
    }

    std::string dvPath = resPath + "\\depth.vert";
    std::string dfPath = resPath + "\\depth.frag";
    Shader* depthShader = new Shader(dvPath.c_str(), dfPath.c_str());
    renderer.addDepthShader(depthShader);
    renderer.initShadowMap(2048, 2048);

    if (scene->HasMaterials()) {
        std::cout << "ucitana svojstva materijala" << std::endl;

        for (int i = 0; i < scene->mNumMaterials; i++) {

            aiString texturePosition;
            int width, height, nrChannels;
            unsigned char* data;
            if (AI_SUCCESS == scene->mMaterials[i]->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePosition)) {
                std::cout << texturePosition.C_Str() << std::endl;
                std::string texPath = "C:/Users/ivrod/Documents/irg/irglab/build/primjerASSIMP/Debug/resources";

                texPath.append("\\glava\\");
                texPath.append(texturePosition.C_Str());

                std::cout << "Assimp texture: " << texturePosition.C_Str() << std::endl;

                stbi_set_flip_vertically_on_load(true); //flipana y koordinata kod robota, obavezno provjeriti
                GLuint textureID;
                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);

                data = stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

                if (!data) {
                    std::cout << "STBI LOAD FAILED: " << texPath << std::endl;
                    continue;
                }

                GLenum format;
                if (nrChannels == 1) format = GL_RED;
                else if (nrChannels == 3) format = GL_RGB;
                else if (nrChannels == 4) format = GL_RGBA;
                else {
                    std::cout << "Unsupported nrChannels: " << nrChannels << std::endl;
                }

                glTexImage2D(GL_TEXTURE_2D,
                    0,
                    format,
                    width,
                    height,
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    data);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                material.diffTex = textureID;
                material.hasTex = true;

                stbi_image_free(data);
            }

            glm::vec3 ambientColor;
            aiColor3D ambientK, diffuseK, specularK;
            float shininessK;

            scene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
            material.ka = glm::vec3(ambientK.r, ambientK.g, ambientK.b);

            scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
            material.kd = glm::vec3(diffuseK.r, diffuseK.g, diffuseK.b);

            scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
            material.ks = glm::vec3(specularK.r, specularK.g, specularK.b);

            scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, shininessK);
            material.shininess = shininessK;
        }
    }

    camera.setPosition(glm::vec3(3, 4, 1));

    while (!glfwWindowShouldClose(window)) {

        glm::mat4 view = glm::inverse(camera.getModelMatrix());

        glm::mat4 projection = glm::frustum(
            -0.5f, 0.5f,
            -0.5f, 0.5f,
            1.0f, 100.0f
        );

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
        
        renderer.beginShadowPass(2048, 2048);

        glm::mat4 lightProjection =
            glm::perspective(
                glm::radians(90.0f),
                1.0f,
                1.0f,
                20.0f
            );

        glm::mat4 lightView =
            glm::lookAt(
                light.getPosition(),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0)
            );

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        light.view = lightView;
        light.projection = lightProjection;

        renderer.drawDepth(depthShader, lightSpaceMatrix);
        renderer.endShadowPass(mWidth, mHeight);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 eye = glm::vec3(camera.getPosition());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderer.depthMap);
        renderer.draw(view, projection, &eye, &light, &material);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}