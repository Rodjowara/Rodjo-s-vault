#pragma once
#include <glm/glm.hpp>            
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

class Material
{
public:
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;

    GLuint diffTex;
    bool hasTex;

    float shininess;
    Material();
};