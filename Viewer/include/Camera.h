#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"

class Camera
{
public:
	Camera(const float aspectRatio, const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
	virtual ~Camera();

	void SetOrthProj(const float height, const float aspectRatio, const float zNear, const float zFar);
	void SetPersProj(const float fovy, const float aspect, const float zNear, const float zFar);

	const glm::mat4x4& GetProjTrans() const;
	const glm::mat4x4& GetViewTransformation() const;

	void UpdateProjMat();


	float GetNear();
	void SetNear(const float zNear);

	float GetFar();
	void SetFar(const float zFar);
	
	float GetFovy();
	void SetFovy(const float fovy);

	void PrespectiveOn();
	void OrthographicOn();
	float GetHeight();
	void SetHeight(const float height);

	bool IsPrespective();

	void Zoom(const float factor);
	const glm::vec3& GetEye() const;
	void SetAspectRatio(float x);




private:
	glm::mat4x4 view_trans;
	glm::mat4x4 proj_trans;
	glm::vec3 eye, up, at, x, y, z;
	float zoom, fovy, height, zNear, zFar, aspectR;
	bool prespective;

	
};
