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
    Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, const float aspectRatio);
    ~Camera();

    float GetNear();
    void SetNear(const float zNear);

    float GetFar();
    void SetFar(const float zFar);
    
    float GetFovy();
    void SetFovy(const float fovy);
    
    float GetHeight();
    void SetHeight(const float height);
    
    bool IsPrespective();
	void SetOrthProj(const float height, const float aspectRatio, const float zNear, const float zFar);
	void SetPersProj(const float fovy, const float aspect, const float zNear, const float zFar);
    const glm::vec3& GetEye() const;

	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformation() const;
    void Zoom(const float factor);

    
    void SwitchToPresp();
    void SwitchToOrtho();
    void SphericalRotate(const glm::vec2& sphericalDelta);

    void UpdateProjectionMatrix();

    glm::mat4x4 GetTransform();
    void SetAspectRatio(float aspectRatio);
    int OrthoPerspective = 0;



private:
    glm::mat4x4 viewTransformation;
    glm::mat4x4 projectionTransformation;

    glm::vec3 eye;
    glm::vec3 up;
    glm::vec3 at;

    glm::vec3 x;
    glm::vec3 y;
    glm::vec3 z;

    float zoom;
    float fovy;
    float height;
    float zNear;
    float zFar;
    float aspectRatio;

    bool prespective;
   
	
};
