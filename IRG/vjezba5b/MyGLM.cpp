#include "MyGLM.h"

glm::mat4 MyGLM::translate3D(glm::vec3 translateVector) {
    glm::mat4 result = glm::mat4(1.0f);           
    result[3][0] = translateVector[0];
    result[3][1] = translateVector[1];
    result[3][2] = translateVector[2];
    return result;
}

glm::mat4 MyGLM::scale3D(glm::vec3 scaleVector) {
    glm::mat4 result = glm::mat4(1.0f);
    result[0][0] = scaleVector[0];
    result[1][1] = scaleVector[1];
    result[2][2] = scaleVector[2];
    return result;
}

glm::mat4 MyGLM::rotate3D(glm::vec3 axis, float angle) {
    axis = glm::normalize(axis);
    float c = cos(angle);
    float s = sin(angle);
    float t = 1.0f - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    glm::mat4 result(1.0f);

    result[0][0] = t * x * x + c;
    result[1][0] = t * x * y - s * z;
    result[2][0] = t * x * z + s * y;

    result[0][1] = t * x * y + s * z;
    result[1][1] = t * y * y + c;
    result[2][1] = t * y * z - s * x;

    result[0][2] = t * x * z - s * y;
    result[1][2] = t * y * z + s * x;
    result[2][2] = t * z * z + c;

    return result;
}

glm::mat4 MyGLM::frustum(float l, float r, float b, float t, float n, float f) {
    glm::mat4 result = glm::mat4(1.0f);

    result[0][0] = 2 * n / (r - l);
    result[1][1] = 2 * n / (t - b);
    result[2][2] = -((f+n)/(f-n));
    result[0][2] = (r + l) / (r - l);
    result[1][2] = (t+b)/(t-b);
    result[2][3] = (-2 * f * n)/(f - n);
    result[3][2] = -1;
    result[3][3] = 0;

    return result;
}

glm::mat4 MyGLM::lookAtMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) {
    glm::vec3 f = glm::normalize(center - eye);      
    glm::vec3 s = glm::normalize(glm::cross(f, viewUp)); 
    glm::vec3 u = glm::cross(s, f);                 

    glm::mat4 result(1.0f);

    result[0][0] = s.x; result[1][0] = s.y; result[2][0] = s.z;
    result[0][1] = u.x; result[1][1] = u.y; result[2][1] = u.z;
    result[0][2] = -f.x; result[1][2] = -f.y; result[2][2] = -f.z;

    result[3][0] = -glm::dot(s, eye);
    result[3][1] = -glm::dot(u, eye);
    result[3][2] = glm::dot(f, eye);
    result[3][3] = 1.0f;

    return result;
}