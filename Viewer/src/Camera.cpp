#include "Camera.h"
#include "Utils.h"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm\gtx\string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Camera::Camera()
{

	this->view_transformation = glm::mat4x4(1.0f);

	nearVal = 30;
	farVal = -200;
	right = 100;
	left = -100;
	top = 100;
	bottom = -100;
	fov = 5;

	eye[0] = 0;
	eye[1] = 0;
	eye[2] = 10;

	at[0] = 0;
	at[1] = 0;
	at[2] = 0;
}

Camera::~Camera()
{

}

//const glm::mat4x4& Camera::GetProjectionTransformation() const{
//	// return proj_trans;
//}

const glm::mat4x4& Camera::GetViewTransformation() const{
	return view_transformation;
}


glm::mat4x4 Camera::GetTransform() {
	// calculate local transformations
	localTranslateMat[3][0] = localTranslateArray[0];
	localTranslateMat[3][1] = localTranslateArray[1];
	localTranslateMat[3][2] = localTranslateArray[2];

	localRotateXMat[1][1] = cos(glm::radians(localRotateArray[0]));
	localRotateXMat[1][2] = sin(glm::radians(localRotateArray[0]));
	localRotateXMat[2][1] = -sin(glm::radians(localRotateArray[0]));
	localRotateXMat[2][2] = cos(glm::radians(localRotateArray[0]));

	localRotateYMat[0][0] = cos(glm::radians(localRotateArray[1]));
	localRotateYMat[0][2] = sin(glm::radians(localRotateArray[1]));
	localRotateYMat[2][0] = -sin(glm::radians(localRotateArray[1]));
	localRotateYMat[2][2] = cos(glm::radians(localRotateArray[1]));

	localRotateZMat[0][0] = cos(glm::radians(localRotateArray[2]));
	localRotateZMat[0][1] = sin(glm::radians(localRotateArray[2]));
	localRotateZMat[1][0] = -sin(glm::radians(localRotateArray[2]));
	localRotateZMat[1][1] = cos(glm::radians(localRotateArray[2]));

	// calculate world transformations
	worldTranslateMat[3][0] = worldTranslateArray[0];
	worldTranslateMat[3][1] = worldTranslateArray[1];
	worldTranslateMat[3][2] = worldTranslateArray[2];

	worldRotateXMat[1][1] = cos(glm::radians(worldRotateArray[0]));
	worldRotateXMat[1][2] = sin(glm::radians(worldRotateArray[0]));
	worldRotateXMat[2][1] = -sin(glm::radians(worldRotateArray[0]));
	worldRotateXMat[2][2] = cos(glm::radians(worldRotateArray[0]));

	worldRotateYMat[0][0] = cos(glm::radians(worldRotateArray[1]));
	worldRotateYMat[0][2] = sin(glm::radians(worldRotateArray[1]));
	worldRotateYMat[2][0] = -sin(glm::radians(worldRotateArray[1]));
	worldRotateYMat[2][2] = cos(glm::radians(worldRotateArray[1]));

	worldRotateZMat[0][0] = cos(glm::radians(worldRotateArray[2]));
	worldRotateZMat[0][1] = sin(glm::radians(worldRotateArray[2]));
	worldRotateZMat[1][0] = -sin(glm::radians(worldRotateArray[2]));
	worldRotateZMat[1][1] = cos(glm::radians(worldRotateArray[2]));

	objectTransform = localTranslateMat * localRotateXMat * localRotateYMat * localRotateZMat;
	worldTransform = worldTranslateMat * worldRotateXMat * worldRotateYMat * worldRotateZMat;


	this->drawTransformation = worldTransform * objectTransform;

	glm::mat4x4 lookat = this->GetCameraLookAt();

	glm::mat4x4 orthoPerspMat;

	if (OrthoPerspective)
		orthoPerspMat = glm::ortho(left, right, bottom, top, nearVal, farVal);
	else
		orthoPerspMat = glm::perspective(glm::radians(fov), (float)half_height / (float)half_width, nearVal, farVal);

	return orthoPerspMat * lookat * glm::inverse(this->drawTransformation);
}

glm::mat4x4 Camera::GetCameraLookAt() {
	glm::vec3 cameraPos = glm::vec3(this->eye[0], this->eye[1], this->eye[2]);
	glm::vec3 cameraTarget = glm::vec3(this->at[0], this->at[1], this->at[2]);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	//glm::mat4x4 lookat = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	glm::mat4x4 lookat = glm::lookAt(cameraPos, cameraTarget, up);

	return lookat;
}

void Camera::IncrementalTrans(bool GoRightLeft) {
	glm::mat4x4 Translate = glm::mat4x4(1);
	if (GoRightLeft) {
		Translate[3][0] = 0.02;
		Translate[3][1] = localTranslateArray[1];
		Translate[3][2] = localTranslateArray[2];
		localTranslateArray[0] += 0.02;
	}
	else {
		Translate[3][0] = -0.02;
		Translate[3][1] = localTranslateArray[1];
		Translate[3][2] = localTranslateArray[2];
		localTranslateArray[0] -= 0.02;
	}
	Translate = glm::inverse(Translate);
	this->drawTransformation = glm::inverse(this->drawTransformation) * Translate;
}
