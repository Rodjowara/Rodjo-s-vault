#pragma once
#include <glm/glm.hpp>            
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

class Material
{
public:
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;

    float shininess;
    Material();
};