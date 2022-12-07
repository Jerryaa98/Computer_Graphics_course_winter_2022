#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"
#include <iostream>
#include "vector"
#include "Face.h"

class Camera
{
public:
    Camera();
	virtual ~Camera();

	glm::mat4x4 GetCameraLookAt();

	void SetOrthProj(const float height, const float aspectRatio, const float zNear, const float zFar);
	void SetPersProj(const float fovy, const float aspect, const float zNear, const float zFar);

	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformation() const;
	void  Camera::IncrementalTrans(bool GoRightLeft);

    float localTranslateArray[3] = { 0, 0, 0 };
    float localRotateArray[3] = { 0, 0, 0 };

    float worldTranslateArray[3] = { 0, 0, 0 };
    float worldRotateArray[3] = { 0, 0, 0 };


    int half_width;
    int half_height;

    glm::mat4x4 GetTransform();

    int OrthoPerspective = 0;

    float nearVal;
    float farVal;
    float right;
    float left;
    float top;
    float bottom;
    float fov;

    float eye[3];
    float at[3];

    glm::mat4x4 drawTransformation;

private:

    std::vector<Face> faces;
    std::vector<glm::vec3> vertices;


    // local matrices (init with identity matrix)
    glm::mat4x4 localTranslateMat = glm::mat4x4(1.0f);
    glm::mat4x4 localRotateXMat = glm::mat4x4(1.0f);
    glm::mat4x4 localRotateYMat = glm::mat4x4(1.0f);
    glm::mat4x4 localRotateZMat = glm::mat4x4(1.0f);

    // world matrices (init with identity matrix)
    glm::mat4x4 worldTranslateMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldRotateXMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldRotateYMat = glm::mat4x4(1.0f);
    glm::mat4x4 worldRotateZMat = glm::mat4x4(1.0f);

    glm::mat4x4 objectTransform;
    glm::mat4x4 worldTransform;

    glm::mat4x4 orthographic;

    glm::mat4x4 view_transformation;
    glm::mat4x4 projection_transformation;
	
};
