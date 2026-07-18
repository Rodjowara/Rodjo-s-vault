#include "Material.h"

Material::Material() {
    ka = glm::vec3(0.2f);
    kd = glm::vec3(0.8f);
    ks = glm::vec3(1.0f);
    shininess = 32.0f;
}