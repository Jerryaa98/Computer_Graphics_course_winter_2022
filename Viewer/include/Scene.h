#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "Camera.h"
#include "MeshModel.h"
#include "Utils.h"
#include "Light.h"

using namespace std;

class Scene {
public:
	Scene();

	void AddModel(const shared_ptr<MeshModel>& mesh_model);
	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;
	
	void AddCamera(const shared_ptr<Camera>& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	Camera& GetActiveCamera();

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;

	std::vector<std::string> modelsList;
	std::vector<std::string> camerasList;

	void Scene::AddLight(const std::shared_ptr<Light>& light);
	int GetLightCount() const;
	Light& GetLight(int index);

	bool drawWorldAxis = false;
	
private:
	vector<shared_ptr<MeshModel>> mesh_models;
	vector<shared_ptr<Camera>> cameras;
	vector<shared_ptr<Light>> lights;

	int active_camera_index;
	int active_model_index;
};