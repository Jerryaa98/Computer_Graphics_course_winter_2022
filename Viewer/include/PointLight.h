#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight(const glm::vec3& position);
	PointLight(const glm::vec3& position, const glm::vec3& color);
	virtual ~PointLight();
	glm::vec3& GetPosition();

	glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;

private:
	glm::vec3 position;
};