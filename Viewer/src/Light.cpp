// light.cpp
#include "Light.h"
#include <iostream>
#include <glm\gtx\string_cast.hpp>


void Light::GetTransform(glm::mat4x4 transformMat) {
    localTranslateMat[3][0] = localTranslateArray[0];
    localTranslateMat[3][1] = localTranslateArray[1];
    localTranslateMat[3][2] = localTranslateArray[2];

    worldTranslateMat[3][0] = worldTranslateArray[0];
    worldTranslateMat[3][1] = worldTranslateArray[1];
    worldTranslateMat[3][2] = worldTranslateArray[2];

    this->updatedLocation = transformMat * localTranslateMat * glm::vec4(location, 1);
}