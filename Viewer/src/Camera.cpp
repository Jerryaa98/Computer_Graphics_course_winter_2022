#include "Camera.h"
#include "Utils.h"
#include <iostream>
//#include <glm/gtx/transform.hpp>
//#include <glm/glm.hpp>
//#include <glm\gtx\string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const float aspectRatio) :
	zoom(1.0f),
	fovy(glm::pi<float>() / 4.0f),
	height(5),
	zNear(0.1f),
	zFar(200.0f),
	aspectRatio(aspectRatio),
	prespective(true),
	viewTransformation(1),
	eye(eye),
	at(at),
	up(up)
{
	UpdateProjectionMatrix();
	viewTransformation = glm::lookAt(eye, at, up);

}


Camera::~Camera(){

}

void Camera::SetOrthProj(
	const float height,
	const float aspectRatio,
	const float zNear,
	const float zFar)
{
	prespective = false;
	float width = aspectRatio * height;
	projectionTransformation = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, zNear, zFar);
}

void Camera::SetPersProj(
	const float fovy,
	const float aspectRatio,
	const float zNear,
	const float zFar)
{
	prespective = true;
	projectionTransformation = glm::perspective(fovy, aspectRatio, zNear, zFar);
}

bool Camera::IsPrespective()
{
	return prespective;
}

const glm::mat4x4& Camera::GetProjectionTransformation() const
{
	return projectionTransformation;
}

const glm::mat4x4& Camera::GetViewTransformation() const
{
	return viewTransformation;
}
void Camera::Zoom(const float factor)
{
	fovy = fovy * factor;
	if (fovy > glm::pi<float>()){
		fovy = glm::pi<float>();
	}
	UpdateProjectionMatrix();
}
void Camera::SphericalRotate(const glm::vec2& sphericalDelta)
{
}
void Camera::SetAspectRatio(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	UpdateProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
	if (prespective)
	{
		SetPersProj(fovy, aspectRatio, zNear, zFar);
	}
	else
	{
		SetOrthProj(height, aspectRatio, zNear, zFar);
	}
}
void Camera::SwitchToPresp()
{
	prespective = true;
	UpdateProjectionMatrix();
}

void Camera::SwitchToOrtho()
{
	prespective = false;
	UpdateProjectionMatrix();
}

float Camera::GetFar()
{
	return zFar;
}

void Camera::SetNear(const float zNear)
{
	this->zNear = zNear;
	UpdateProjectionMatrix();
}

float Camera::GetHeight()
{
	return height;
}

void Camera::SetFovy(const float fovy)
{
	this->fovy = fovy;
	UpdateProjectionMatrix();
}

float Camera::GetFovy()
{
	return fovy;
}

float Camera::GetNear()
{
	return zNear;
}

void Camera::SetFar(const float zFar)
{
	this->zFar = zFar;
	UpdateProjectionMatrix();
}

void Camera::SetHeight(const float height)
{
	this->height = height;
	UpdateProjectionMatrix();
}

const glm::vec3& Camera::GetEye() const
{
	return eye;
}
