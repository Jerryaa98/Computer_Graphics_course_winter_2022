#include "PointLight.h"
#include <algorithm>

PointLight::PointLight(const glm::vec3& position) :
	Light(),
	position(position)
{
	ambientColor = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
	diffuseColor = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
	specularColor = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
}

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color) :
	Light(color),
	position(position)
{

}

PointLight::~PointLight()
{

}

glm::vec3& PointLight::GetPosition()
{
	return position;
}