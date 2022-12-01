#include "Scene.h"
#include "MeshModel.h"
#include <string>

Scene::Scene() :
	active_camera_index(0),
	active_model_index(0)
{

}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models.push_back(mesh_model);
	active_model_index = mesh_models.size() - 1;
	std::string name = "Model" + std::to_string(active_model_index) + " " + (*mesh_model).GetModelName();
	this->modelsList.push_back(name);

}

int Scene::GetModelCount() const
{
	return mesh_models.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models[active_model_index];
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras.push_back(camera);
	active_camera_index = cameras.size() - 1;
	std::string name = "Camera" + std::to_string(active_camera_index); 
	SetActiveCameraIndex(active_camera_index);
}

int Scene::GetCameraCount() const
{
	return cameras.size();
}

Camera& Scene::GetCamera(int index)
{
	return *cameras[index];
}

Camera& Scene::GetActiveCamera()
{
	return *cameras[active_camera_index];
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index;
}