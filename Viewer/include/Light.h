// light.h
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <math.h>

class Light
{
public:

    glm::vec3 ambientColor = glm::vec3(1, 1, 1);


    glm::vec3 diffuseColor = glm::vec3(1, 1, 1);


    glm::vec3 specularColor = glm::vec3(1, 1, 1);

    float localTranslateArray[3] = { 0, 0, 0 };
    float worldTranslateArray[3] = { 0, 0, 0 };
    glm::mat4x4 localTranslateMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldTranslateMat = glm::mat4x4(1.0f);

    /*
    reflectionType flags:
    0 - Ambient
    1 - Diffuse
    2 - Specular
    */
    int reflectionType = 0;

    glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 updatedLocation = glm::vec3(location);

    void GetTransform(glm::mat4x4 transformMat);
};