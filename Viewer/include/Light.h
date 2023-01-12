// light.h
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <math.h>

class Light
{
public:

    float alpha = 1;

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

    float localTranslateArray[3] = { 0, 0, 0 };
    float worldTranslateArray[3] = { 0, 0, 0 };
    glm::mat4x4 localTranslateMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldTranslateMat = glm::mat4x4(1.0f);

    float highValue1 = 1.0f;
    float highValue2 = 1.0f;
    float highValue3 = 1.0f;

    /*
    0 - flat shading
    1 - gouraud
    2 - phong
    */
    int lightingType = 0;


    glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 updatedLocation = glm::vec3(location);

    void GetTransform(glm::mat4x4 transformMat);
};