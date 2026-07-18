#pragma once
#include "Transform.h"
class Light: public Transform 
{
public:
    glm::vec3 Ia; // ambijentalna
    glm::vec3 Id; // difuzna
    glm::vec3 Is; // spekularna

    glm::mat4 view;
    glm::mat4 projection;

    Light();
};