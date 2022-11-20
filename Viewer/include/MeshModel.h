#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include <math.h>

class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	glm::vec2 MeshModel::GetVertixPoint(int i);

    glm::mat4x4 GetTransform();
    const std::vector<glm::vec3> getTransformedVertices();


    float localTranslateVector[3] = { 0, 0, 0 };
    float localRotateVector[3] = { 0, 0, 0 };
    float localScaleVector[3] = { 1, 1, 1 };
    bool uniformLocalScale = false;
    float localScale = 1;

    float worldTranslateVector[3] = { 0, 0, 0 };
    float worldRotateVector[3] = { 0, 0, 0 };
    float worldScaleVector[3] = { 1, 1, 1 };
    bool uniformWorldScale = false;
    float worldScale = 1;

    int maxScale = 0;

private:
    // local matrices (init with identity matrix)
    glm::mat4x4 localTranslateMat = glm::mat4x4(1.0f);
    glm::mat4x4 localRotateXMat = glm::mat4x4(1.0f);
    glm::mat4x4 localRotateYMat = glm::mat4x4(1.0f);
    glm::mat4x4 localRotateZMat = glm::mat4x4(1.0f);
    glm::mat4x4 localScaleMat = glm::mat4x4(1.0f);

    // world matrices (init with identity matrix)
    glm::mat4x4 worldTranslateMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldRotateXMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldRotateYMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldRotateZMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldScaleMat = glm::mat4x4(1.0f);

	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::string model_name;
    glm::mat4x4 localTransform;
    glm::mat4x4 worldTransform;
};
