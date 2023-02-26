#pragma once

#include "AmbientLight.h"
#include "PointLight.h"
#include "Camera.h"
#include <vector>
#include <string>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "MeshModel.h"

//using namespace std;
//#include "Utils.h"
//#include "Light.h"
//#include "Camera.h"
//#include <iostream>


class Scene {
public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& model);
	int GetModelCount() const;
	//MeshModel& GetModel(int index) const;
	//MeshModel& GetActiveModel() const;
	std::shared_ptr<MeshModel> GetModel(int index) const;
	
	void AddCamera(const Camera& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	//Camera& GetActiveCamera();
	const Camera& GetCamera(int index) const;


	//void SetActiveCameraIndex(int index);
	//int GetActiveCameraIndex() const;

	//void SetActiveModelIndex(int index);
	//int GetActiveModelIndex() const;

	std::vector<std::string> modelsList;
	std::vector<std::string> camerasList;
	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;
	const AmbientLight& GetAmbientLight();
	const Camera& GetActiveCamera() const;
	Camera& GetActiveCamera();
	void AddLight(const std::shared_ptr<PointLight>& light);
	int GetLightCount() const;
	std::shared_ptr<PointLight> GetLight(int index) const;
	const std::vector<std::shared_ptr<PointLight>>& GetActiveLights() const;
	//bool drawWorldAxis = false;
	const std::shared_ptr<MeshModel>& GetActiveModel() const;
	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;

	
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<std::shared_ptr<PointLight>> lights;
	AmbientLight ambientLight;
	std::vector<Camera> cameras;

	int activeCameraIndex;
	int activeModelIndex;
	/*
	* vector<shared_ptr<MeshModel>> mesh_models;
	vector<shared_ptr<Camera>> cameras;
	vector<shared_ptr<Light>> lights;

	int active_camera_index;
	int active_model_index;
	*/
};