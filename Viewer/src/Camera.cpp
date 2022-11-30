#include "Camera.h"
#include "Utils.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(const float aspectR, const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up){
	
	this->height = 5;
	this->fovy = glm::pi<float>() / 4.0f;
	this->view_trans = glm::mat4x4(1.0);
	this->zoom = 1.0f;
	this->zNear = 0.1f;
	this->zFar = 200.0f;
	this->eye = eye;
	this->up = up;
	this->at = at;
	this->aspectR = aspectR;
	this->view_trans = glm::lookAt(eye, at, up);
	UpdateProjMat();

}

Camera::~Camera()
{
	
}

const glm::mat4x4& Camera::GetProjTrans() const{
	return proj_trans;
}

const glm::mat4x4& Camera::GetViewTransformation() const{
	return view_trans;
}

void Camera::SetPersProj(const float fovy, const float aspectR, const float zNear, const float zFar) {
	this->prespective = true;
	proj_trans = glm::perspective(fovy, aspectR, zNear, zFar);
}

void Camera::SetOrthProj(const float height, const float aspectR, const float zNear, const float zFar) {
	this->prespective = false;
	proj_trans = glm::ortho(-(aspectR * height) / 2, (aspectR * height) / 2, -height / 2, height / 2, zNear, zFar);
}


void Camera::UpdateProjMat(){
	if (this->prespective)
		SetPersProj(this->fovy, this->aspectR, this->zNear, this->zFar);
	else
		SetOrthProj(this->height, this->aspectR, this->zNear, this->zFar);
}

float Camera::GetNear() {
	return this->zNear;
}

void Camera::SetNear(const float x) {
	this->zNear = x;
	UpdateProjMat();
}


float Camera::GetHeight() {
	return this->height;
}

void Camera::SetHeight(const float x){
	this->height = x;
	UpdateProjMat();
}


const glm::vec3& Camera::GetEye() const{
	return this->eye;
}

void Camera::SetAspectRatio(float x){
	this->aspectR = x;
	UpdateProjMat();
}

bool Camera::IsPrespective() {
	return this->prespective;
}

void Camera::Zoom(const float factor) {
	float pi = glm::pi<float>();
	if (fovy * factor > pi)
		fovy = pi;
	UpdateProjMat();

}

void Camera::PrespectiveOn(){
	prespective = true;
	UpdateProjMat();
}

void Camera::OrthographicOn() {
	this->prespective = false;
	UpdateProjMat();
}


float Camera::GetFovy(){
	return this->fovy;
}

void Camera::SetFovy(const float x){
	this->fovy = x;
	UpdateProjMat();
}


float Camera::GetFar() {
	return this->zFar;
}

void Camera::SetFar(const float x) {
	this->zFar = x;
	UpdateProjMat();
}



