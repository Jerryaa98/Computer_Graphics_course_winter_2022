/*#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
using namespace std;

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"
#include "Light.h"

/**
 * Fields
 
bool show_demo_window = false;
bool show_another_window = false;
int windowWidth = 1280;
int windowHeight = 720;
glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);
ImVec2 oldCoordinates;

int LocalWorldEditCamera = 1;
bool cameraT = true;
std::shared_ptr<MeshModel> cameraModel;

/**
 * Function declarations
 
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer);
/**
 * Function implementation
 
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char **argv)
{
	int windowWidth = 1280, windowHeight = 720;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();

	shared_ptr<Camera>& camera = std::make_shared<Camera>();
	scene.AddCamera(camera);
	cameraModel = Utils::LoadMeshModel("..\\Data\\camera.obj");
	
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		StartFrame();
	    DrawImguiMenus(io, scene, renderer);
		RenderFrame(window, scene, renderer, io);
    }

	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
		Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);

	}

	if (!io.WantCaptureKeyboard){
		// TODO: Handle keyboard events here
		if (io.KeysDown[49]) //1
				scene.GetActiveModel().localRotateVector[0] += 10;

		if (io.KeysDown[50]) //2
			scene.GetActiveModel().localRotateVector[0] -= 10;
		
		if (io.KeysDown[51]) //3
			scene.GetActiveModel().localRotateVector[1] +=10 ;
		
		if (io.KeysDown[52]) //4
			scene.GetActiveModel().localRotateVector[1] -=10;
		
		if (io.KeysDown[53]) //5
			scene.GetActiveModel().localRotateVector[2] +=10;
		
		if(io.KeysDown[54]) //6
			scene.GetActiveModel().localRotateVector[2] -=10;
		if (io.KeysDown[45])// -
		{
			scene.GetActiveModel().uniformLocalScale = true;
			if (scene.GetActiveModel().localScale > 175)
				scene.GetActiveModel().localScale -= 175;
		}

		if (io.KeysDown[61])// +
		{
			scene.GetActiveModel().uniformLocalScale = true;
			scene.GetActiveModel().localScale += 175;
		}

		if (io.KeysDown[65])//a
			scene.GetActiveModel().localTranslateVector[0] -= 10;
		
		if (io.KeysDown[68])//d
			scene.GetActiveModel().localTranslateVector[0] += 10;
		
		if (io.KeysDown[83]) // s
			scene.GetActiveModel().localTranslateVector[1] -= 10;
		
		if (io.KeysDown[87]) // w
			scene.GetActiveModel().localTranslateVector[1] += 10;

		if (io.KeysDown[74] || io.KeysDown[106]) // j
		{
			if (scene.GetCameraCount() > 0) {
				Camera& camera = scene.GetActiveCamera();
				camera.IncrementalTrans(true);
			}
		}


		if (io.KeysDown[76] || io.KeysDown[108]) // l
		{
			if (scene.GetCameraCount() > 0) {
				Camera& camera = scene.GetActiveCamera();
				camera.IncrementalTrans(false);
			}
		}

		// A key is down
			// Use the ASCII table for more key codes (https://www.asciitable.com/)
	}

	if (!io.WantCaptureMouse)
	{
		// TODO: Handle mouse events here
		if (io.MouseDown[0])
		{
			// Left mouse button is down

			ImVec2 newCoordinates = io.MousePos;
			if (scene.GetModelCount() > 0) {
				MeshModel& model = scene.GetActiveModel();
				model.localRotateVector[1] += (newCoordinates[0] - oldCoordinates[0]) / 10;
				model.localRotateVector[0] += (newCoordinates[1] - oldCoordinates[1]) / 10;
				
			}
		}
		if (io.MouseDown[1])
		{
			// Right mouse button is down

			ImVec2 newCoordinates = io.MousePos;
			if (scene.GetModelCount() > 0) {
				scene.GetActiveModel().localRotateVector[1] += newCoordinates[0] - oldCoordinates[0];
				scene.GetActiveModel().localRotateVector[0] += newCoordinates[1] - oldCoordinates[1];
			}
		}
		if (io.MouseDown[2])
		{
			// Scroll Wheel button is down

			ImVec2 newCoordinates = io.MousePos;
			if (scene.GetModelCount() > 0) {
				scene.GetActiveModel().uniformLocalScale = true;
				scene.GetActiveModel().localScale += ((oldCoordinates[1] - newCoordinates[1]) +  (newCoordinates[0] - oldCoordinates[0]))* 5;
			}
		}

		oldCoordinates = io.MousePos;
	}

	renderer.ClearColorBuffer(clear_color);
	renderer.Render(scene, cameraModel);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}


void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer) {
	/**
	 * MeshViewer menu
	 
	ImGui::Begin("MeshViewer Menu");

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load Mesh", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene.AddModel(Utils::LoadMeshModel(outPath));
					free(outPath);
				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);
	ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);
	// TODO: Add more controls as needed

	ImGui::End();

	int modelCount = scene.GetModelCount();
	int cameraCount = scene.GetCameraCount();

	ImGui::Begin("Transformation Window");


		if (ImGui::CollapsingHeader("Camera")) {
			if (ImGui::Button("Add Camera")) {
				shared_ptr<Camera>& newCam = std::make_shared<Camera>();
				scene.AddCamera(newCam);
			}
			ImGui::Text("Select Camera:");
			int numCameras = scene.GetCameraCount();
			static int selectedCamra = 0;
			for (int n = 0; n < numCameras; n++) {
				std::string camName = "Camera ";
				camName.append(std::to_string(n));
				char bufc[64];
				sprintf(bufc, camName.c_str(), n);
				if (ImGui::Selectable(bufc, selectedCamra == n))
					selectedCamra = n;
			}
			scene.SetActiveCameraIndex(selectedCamra);
			Camera& camera = scene.GetCamera(selectedCamra);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::SliderFloat("Camera Scale", &(*cameraModel).localScale, 0.01, 10);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("--- CAMERA LOCATION (EYE) ---");

			ImGui::SliderFloat("X Axis - eye", &camera.eye[0], -30, 30);
			ImGui::SliderFloat("Y Axis - eye", &camera.eye[1], -30, 30);
			ImGui::SliderFloat("Z Axis - eye", &camera.eye[2], -10, 10);

			ImGui::Separator();

			ImGui::Text("--- CAMERA TARGET (AT) ---");

			ImGui::SliderFloat("X Axis - at", &camera.at[0], -100, 100);
			ImGui::SliderFloat("Y Axis - at", &camera.at[1], -100, 100);
			ImGui::SliderFloat("Z Axis - at", &camera.at[2], -100, 100);

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("Transformation Type:");
			ImGui::SameLine();
			ImGui::RadioButton("Local", &LocalWorldEditCamera, 1);
			ImGui::SameLine();
			ImGui::RadioButton("world", &LocalWorldEditCamera, 0);

			if (LocalWorldEditCamera) {
				ImGui::Text("--- LOCAL TRANSFORMATIONS ---");

				ImGui::Text("Local Translate:");
				ImGui::SliderFloat("Local Translate X", &camera.localTranslateArray[0], -0.3, 0.3);
				ImGui::SliderFloat("Local Translate Y", &camera.localTranslateArray[1], -0.3, 0.3);
				ImGui::SliderFloat("Local Translate Z", &camera.localTranslateArray[2], -0.3, 0.3);

				ImGui::Separator();

				ImGui::Text("Local Rotate:");
				ImGui::SliderFloat("Local Rotate X", &camera.localRotateArray[0], 0.0, 10);
				ImGui::SliderFloat("Local Rotate Y", &camera.localRotateArray[1], 0.0, 30);
				ImGui::SliderFloat("Local Rotate Z", &camera.localRotateArray[2], 0.0, 10);

				ImGui::Separator();
				ImGui::Separator();

			}
			else {
				ImGui::Text("--- WORLD TRANSFORMATIONS ---");
				ImGui::Text("World Translate:");
				ImGui::SliderFloat("World Translate X", &camera.worldTranslateArray[0], 0.1, 10);
				ImGui::SliderFloat("World Translate Y", &camera.worldTranslateArray[1], 0.1, 30);
				ImGui::SliderFloat("World Translate Z", &camera.worldTranslateArray[2], 0.1, 10);

				ImGui::Separator();

				ImGui::Text("World Rotate:");
				ImGui::SliderFloat("Rotate X", &camera.worldRotateArray[0], 0.1, 10);
				ImGui::SliderFloat("Rotate Y", &camera.worldRotateArray[1], 0.1, 10);
				ImGui::SliderFloat("Rotate Z", &camera.worldRotateArray[2], 0.1, 10);

				ImGui::Separator();
				ImGui::Separator();

			}

			ImGui::Text("View Type:");
			ImGui::SameLine();
			ImGui::RadioButton("Orthographic", &(camera.OrthoPerspective), 1);
			ImGui::SameLine();
			ImGui::RadioButton("Perspective", &(camera.OrthoPerspective), 0);

			if (camera.OrthoPerspective) {
				ImGui::Text("Orthographic View:");
				ImGui::SliderFloat("Camera top", &(camera.top), -200, 200, "%.3f");
				ImGui::SliderFloat("Camera bottom", &(camera.bottom), -200, 200, "%.3f");
				ImGui::SliderFloat("Camera right", &(camera.right), -200, 200, "%.3f");
				ImGui::SliderFloat("Camera left", &(camera.left), -200, 200, "%.3f");
				ImGui::SliderFloat("Camera near", &(camera.nearVal), -200, 200, "%.3f");
				ImGui::SliderFloat("Camera far", &(camera.farVal), -200, 200, "%.3f");
			}
			else {
				ImGui::Text("Perspective View:");
				ImGui::SliderFloat("Camera near", &(camera.nearVal), 0.01f, 1.0f, "%.3f");
				ImGui::SliderFloat("Camera far", &(camera.farVal), 0.01f, 1.0f, "%.3f");
				ImGui::SliderFloat("Camera FOV", &(camera.fov), 1, 1, "%.0f");
			}

			ImGui::Separator();

			ImGui::Text("Addons:");
			ImGui::Checkbox("Draw World Axis", &(scene.drawWorldAxis));

		}

		if (ImGui::CollapsingHeader("Object"))
		{
			// display transformations window if at least one model is loaded
			if (modelCount) {
				ImGui::Text("Select Model:");
				static int selected = 0;
				for (int n = 0; n < modelCount; n++) {
					char buf[64];
					sprintf(buf, scene.modelsList[n].c_str(), n);
					if (ImGui::Selectable(buf, selected == n))
						selected = n;
				}

				scene.SetActiveModelIndex(selected);
				MeshModel& model = scene.GetActiveModel();

				ImGui::Text("Transformation Type:");
				ImGui::SameLine();
				ImGui::RadioButton("Local", &(model.LocalWorldEditObject), 1);
				ImGui::SameLine();
				ImGui::RadioButton("world", &(model.LocalWorldEditObject), 0);


				if (model.LocalWorldEditObject) {
					ImGui::Text("--- LOCAL TRANSFORMATIONS ---");
					ImGui::Text("Local Scale:");
					ImGui::SliderFloat("Local Scale X", &model.localScaleVector[0], -1, 5);
					ImGui::SliderFloat("Local Scale Y", &model.localScaleVector[1], -1, 5);
					ImGui::SliderFloat("Local Scale Z", &model.localScaleVector[2], -1, 5);

					ImGui::Separator();

					ImGui::Checkbox("Lock All Local Axis", &(model.uniformLocalScale));
					ImGui::SliderFloat("Local Scale Locked", &(model.localScale), 0.1, 1000);

					ImGui::Separator();
					ImGui::Separator();


					ImGui::Text("Local Translate:");
					ImGui::SliderFloat("Local Translate X", &model.localTranslateVector[0], -1, 1);
					ImGui::SliderFloat("Local Translate Y", &model.localTranslateVector[1], -1, 1);
					ImGui::SliderFloat("Local Translate Z", &model.localTranslateVector[2], -10, 10);

					ImGui::Separator();
					ImGui::Separator();


					ImGui::Text("Local Rotate:");
					ImGui::SliderFloat("Local Rotate X", &model.localRotateVector[0],1 ,360);
					ImGui::SliderFloat("Local Rotate Y", &model.localRotateVector[1],1 ,360);
					ImGui::SliderFloat("Local Rotate Z", &model.localRotateVector[2],1 ,360);

					ImGui::Separator();
					ImGui::Separator();

				}
				else {
					ImGui::Text("--- WORLD TRANSFORMATIONS ---");
					ImGui::Text("World Scale:");
					ImGui::SliderFloat("World Scale X", &model.worldScaleVector[0], 0.1, 10);
					ImGui::SliderFloat("World Scale Y", &model.worldScaleVector[1], 0.1, 10);
					ImGui::SliderFloat("World Scale Z", &model.worldScaleVector[2], 0.1, 10);
					ImGui::Separator();
					ImGui::Checkbox("Lock All World Axis", &(model.uniformWorldScale));
					ImGui::SliderFloat("World Scale Locked", &(model.worldScale), 0.1, 10);

					ImGui::Separator();
					ImGui::Separator();


					ImGui::Text("World Translate:");
					ImGui::SliderFloat("World Translate X", &model.worldTranslateVector[0], -1, 1);
					ImGui::SliderFloat("World Translate Y", &model.worldTranslateVector[1], -1, 1);
					ImGui::SliderFloat("World Translate Z", &model.worldTranslateVector[2], -1, 1);

					ImGui::Separator();
					ImGui::Separator();


					ImGui::Text("World Rotate:");
					ImGui::SliderFloat("World Rotate X", &model.worldRotateVector[0], 0, 400);
					ImGui::SliderFloat("World Rotate Y", &model.worldRotateVector[1], 0, 400);
					ImGui::SliderFloat("World Rotate Z", &model.worldRotateVector[2], 0, 400);

					ImGui::Separator();
					ImGui::Separator();


				}
				ImGui::Text("Object Addons:");
				ImGui::Checkbox("Draw Axis", &(model.drawAxis));
				ImGui::Separator();
				ImGui::Checkbox("Draw Bounding Box", &(model.drawBoundingBox));
				ImGui::Separator();
				ImGui::Checkbox("Draw Vertex Normals", &(model.drawVertexNormals));
				ImGui::SliderFloat("Vertex Normals Scale", &(model.vertexNormalsScale), 0, 1);
				ImGui::Separator();
				ImGui::Checkbox("Draw Face Normals", &(model.drawFaceNormals));
				ImGui::SliderFloat("Face Normals Scale", &(model.faceNormalsScale),0, 30);
				ImGui::Separator();
				ImGui::Checkbox("Triangles Bounding Boxes", &(model.trianglesBoundingBoxes));
				ImGui::Separator();
				ImGui::Checkbox("Colored Triangles", &(model.coloredTriangles));
				ImGui::Separator();
				ImGui::Checkbox("Triangles Outline", &(model.trianglesOutlines));
				ImGui::Separator();
				ImGui::ColorEdit3("Object ambient", (float*)&model.ambientColor);
				ImGui::Separator();
				ImGui::ColorEdit3("Object diffuse", (float*)&model.diffuseColor);
				ImGui::Separator();
				ImGui::ColorEdit3("Object specular", (float*)&model.specularColor);

			}
		}

		if (ImGui::CollapsingHeader("Light"))
		{
			if (ImGui::Button("Add Light")) {
				shared_ptr<Light>& newLight = std::make_shared<Light>();
				scene.AddLight(newLight);
			}
			// display transformations window if at least one model is loaded
			if (scene.GetLightCount()) {

				ImGui::Text("Select Light:");

				static int selectedLight = 0;
				for (int n = 0; n < scene.GetLightCount(); n++) {
					std::string camName = "Light ";
					camName.append(std::to_string(n));
					char bufc[64];
					sprintf(bufc, camName.c_str(), n);
					if (ImGui::Selectable(bufc, selectedLight == n))
						selectedLight = n;
				}

				Light& light = scene.GetLight(selectedLight);

				ImGui::Text("Local Translate:");                
				ImGui::InputFloat("Local Translate X", &light.localTranslateArray[0], 50.0f, 5, "%.2f");
				ImGui::InputFloat("Local Translate Y", &light.localTranslateArray[1], 50.0f, 5, "%.2f");
				ImGui::InputFloat("Local Translate Z", &light.localTranslateArray[2], 50.0f, 5, "%.2f");

				ImGui::InputFloat("high value1", &light.highValue1, 100, 5, "%.2f");
				ImGui::InputFloat("high value2", &light.highValue2, 100, 5, "%.2f");
				ImGui::InputFloat("high value3", &light.highValue3, 100, 5, "%.2f");

				ImGui::Text("Lighting Type:");
				ImGui::SameLine();
				ImGui::RadioButton("flat shading", &(light.lightingType), 0);
				ImGui::SameLine();
				ImGui::RadioButton("gouraud", &(light.lightingType), 1);
				ImGui::SameLine();
				ImGui::RadioButton("phong", &(light.lightingType), 2);

				ImGui::Separator();
				ImGui::InputFloat("alpha for specular", &(light.alpha), 0.1, 0.1, "%.2f");
				ImGui::Separator();
				ImGui::ColorEdit3("Light ambient Color", (float*)&light.ambientColor);

				ImGui::Separator();
				ImGui::ColorEdit3("Light diffuse Color", (float*)&light.diffuseColor);

				ImGui::Separator();
				ImGui::ColorEdit3("Light specular Color", (float*)&light.specularColor);
			}
		}

	ImGui::End();
}*/

#define _USE_MATH_DEFINES
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <nfd.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "ImguiMenus.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Utils.h"

double zoomFactor = 1;
int windowWidth = 1280;
int windowHeight = 720;
char* windowTitle = "OpenGL Demo";
glm::vec4 clearColor = glm::vec4(0.117f, 0.117f, 0.117f, 0.117f);
bool zoomChanged = false;
std::shared_ptr<Scene> scene;

ImGuiIO* imgui;
GLFWwindow* window;

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupImgui(GLFWwindow* window);
bool Setup(int windowWidth, int windowHeight, const char* windowName);
void Cleanup();

static void GlfwErrorCallback(int error, const char* description);
void RenderFrame(GLFWwindow* window, std::shared_ptr<Scene> scene, Renderer& renderer, ImGuiIO& io);

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height);
float GetAspectRatio();
void DrawImguiMenus();
void HandleImguiInput();

int main(int argc, char** argv)
{
	if (!Setup(windowWidth, windowHeight, windowTitle))
	{
		std::cerr << "Setup failed" << std::endl;
		return -1;
	}

	scene = std::make_shared<Scene>();
	glm::vec3 eye = glm::vec3(0, 0, 10);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	Camera camera = Camera(eye, at, up, GetAspectRatio());
	scene->AddCamera(camera);

	scene->AddLight(std::make_shared<PointLight>(glm::vec3(100, 0, 15), glm::vec3(0, 0, 1)));
	Renderer renderer;
	renderer.LoadShaders();
	renderer.LoadTextures();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		// Imgui stuff
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawImguiMenus();
		ImGui::Render();
		HandleImguiInput();

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene
		renderer.Render(scene);

		// Imgui stuff
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
bool Setup(int windowWidth, int windowHeight, const char* windowName)
{
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, windowName);
	if (!window)
	{
		std::cerr << "Window setup failed" << std::endl;
		return false;
	}

	imgui = &SetupImgui(window);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* SetupGlfwWindow(int windowWidth, int windowHeight, const char* windowName)
{
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create an OpenGL 3.3 core, forward compatible context window
	window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	// Make the window's context the current one
	glfwMakeContextCurrent(window);
	// Setup window events callbacks
	glfwSetFramebufferSizeCallback(window, glfw_OnFramebufferSize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// An error occured
		std::cerr << "GLAD initialization failed" << std::endl;
		return false;
	}

	return window;
}
ImGuiIO& SetupImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	glfwSetScrollCallback(window, glfw_OnMouseScroll);

	return io;
}

void HandleImguiInput()
{
	if (!imgui->WantCaptureKeyboard)
	{
		if (imgui->KeysDown[49]) // 1
		{
			scene->GetActiveModel()->RotateXModel(M_PI / 200);
		}
		if (imgui->KeysDown[50]) // 2
		{
			scene->GetActiveModel()->RotateXModel(-M_PI / 200);
		}
		if (imgui->KeysDown[51]) // 3
		{
			scene->GetActiveModel()->RotateYModel(M_PI / 200);
		}
		if (imgui->KeysDown[52]) // 4
		{
			scene->GetActiveModel()->RotateYModel(-M_PI / 200);
		}
		if (imgui->KeysDown[53]) // 5
		{
			scene->GetActiveModel()->RotateZModel(M_PI / 200);
		}
		if (imgui->KeysDown[54]) // 6
		{
			scene->GetActiveModel()->RotateZModel(-M_PI / 200);
		}
		if (imgui->KeysDown[45]) // -
		{
			scene->GetActiveModel()->ScaleModel(1 / 1.01);
		}
		if (imgui->KeysDown[61]) // +
		{
			scene->GetActiveModel()->ScaleModel(1.01);
		}
		if (imgui->KeysDown[65]) // a
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(-0.02, 0, 0));
		}
		if (imgui->KeysDown[68]) // d
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0.02, 0, 0));
		}

		if (imgui->KeysDown[83]) // s
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, -0.02, 0));
		}

		if (imgui->KeysDown[87]) // w
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0.02, 0));
		}
	}
	if (!imgui->WantCaptureMouse)
	{
		if (zoomChanged)
		{
			scene->GetActiveCamera().Zoom(zoomFactor);
			zoomChanged = false;
		}
	}
}
void Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}
//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
}

void glfw_OnMouseScroll(GLFWwindow * window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	zoomFactor = glm::pow(1.1, -yoffset);
	zoomChanged = true;
}
float GetAspectRatio()
{
	return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}
void DrawImguiMenus()
{
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	{
		ImGui::Begin("Scene Menu");
		if (ImGui::ColorEdit3("Clear Color", (float*)&clearColor))
		{
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		}
		if (ImGui::CollapsingHeader("Cameras"))
		{
			if (ImGui::Button("Add Camera"))
			{
				glm::vec3 eye = glm::vec3(0, 0, 10);
				glm::vec3 at = glm::vec3(0, 0, 0);
				glm::vec3 up = glm::vec3(0, 1, 0);
				Camera camera = Camera(eye, at, up, GetAspectRatio());
				scene->AddCamera(camera);
			}
			const char** items;
			std::vector<std::string> cameraStrings;
			items = new const char* [scene->GetCameraCount()];
			for (int i = 0; i < scene->GetCameraCount(); i++)
			{
				std::ostringstream s;
				s << "Camera " << i;
				std::string mystring = s.str();
				cameraStrings.push_back(mystring);
			}
			for (int i = 0; i < scene->GetCameraCount(); i++)
			{
				items[i] = cameraStrings[i].c_str();
			}
			int currentCamera = scene->GetActiveCameraIndex();
			ImGui::Combo("Active Camera", &currentCamera, items, scene->GetCameraCount());
			if (currentCamera != scene->GetActiveCameraIndex())
			{
				scene->SetActiveCameraIndex(currentCamera);
				scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
			}
			delete items;
			int newProjectionType = scene->GetActiveCamera().IsPrespective() ? 0 : 1;
			ImGui::RadioButton("Prespective", &newProjectionType, 0);
			ImGui::RadioButton("Orthographic", &newProjectionType, 1);
			if (newProjectionType == 0)
			{
				float fovy;
				float zNear;
				float zFar;
				scene->GetActiveCamera().SwitchToPresp();
				scene->GetActiveCamera().SwitchToPresp();
				if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetNear(zNear);
				}
				if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetFar(zFar);
				}
			}
			else if (newProjectionType == 1)
			{
				float height;
				float zNear;
				float zFar;
				scene->GetActiveCamera().SwitchToOrtho();
				if (ImGui::SliderFloat("Height", &height, 0.0f, M_PI))
				{
					scene->GetActiveCamera().SetHeight(height);
				}
				if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetNear(zNear);
				}
				if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetFar(zFar);
				}
			}
		}
		if (ImGui::CollapsingHeader("Models"))
		{
			const char** items;
			std::vector<std::string> modelStrings;
			items = new const char* [scene->GetModelCount()];
			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				std::ostringstream s;
				s << scene->GetModel(i)->GetModelName();
				std::string mystring = s.str();
				modelStrings.push_back(mystring);
			}

			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				items[i] = modelStrings[i].c_str();
			}
			int currentModelIndex = scene->GetActiveModelIndex();
			ImGui::Combo("Active Model", &currentModelIndex, items, scene->GetModelCount());
			if (currentModelIndex != scene->GetActiveModelIndex())
			{
				scene->SetActiveModelIndex(currentModelIndex);
			}
			delete items;
			glm::vec3 modelColor = scene->GetActiveModel()->GetColor();
			if (ImGui::ColorEdit3("Model Color", (float*)&modelColor))
			{
				scene->GetActiveModel()->SetColor(modelColor);
			}
			std::shared_ptr<MeshModel> meshModel = std::dynamic_pointer_cast<MeshModel>(scene->GetActiveModel());
			if (meshModel)
			{
				ImGui::Text("Object Addons:");
				ImGui::ColorEdit3("Object ambient", (float*)&scene->GetActiveModel()->ambientColor);

				ImGui::Separator();
				ImGui::ColorEdit3("Object diffuse", (float*)&scene->GetActiveModel()->diffuseColor);

				ImGui::Separator();
				ImGui::ColorEdit3("Object specular", (float*)&scene->GetActiveModel()->specularColor);
				ImGui::Text("Light Addons:");
				ImGui::InputFloat("light pos X", &scene->GetLight(0)->GetPosition().x, 50.0f, 5, "%.2f");
				ImGui::InputFloat("light pos Y", &scene->GetLight(0)->GetPosition().y, 50.0f, 5, "%.2f");
				ImGui::InputFloat("light pos Z", &scene->GetLight(0)->GetPosition().z, 50.0f, 5, "%.2f");
				ImGui::Text("Light Addons:");
				ImGui::InputFloat("light pos X", &scene->GetLight(0)->GetPosition().x, 50.0f, 5, "%.2f");
				ImGui::InputFloat("light pos Y", &scene->GetLight(0)->GetPosition().y, 50.0f, 5, "%.2f");
				ImGui::InputFloat("light pos Z", &scene->GetLight(0)->GetPosition().z, 50.0f, 5, "%.2f");
				ImGui::InputFloat("alpha for specular", (&scene->GetLight(0)->alpha), 0.1, 0.1, "%.2f");

				ImGui::Separator();
				ImGui::ColorEdit3("Light ambient Color", (float*)&scene->GetLight(0)->ambientColor);

				ImGui::Separator();
				ImGui::ColorEdit3("Light diffuse Colore", (float*)&scene->GetLight(0)->diffuseColor);
				ImGui::Separator();
				ImGui::ColorEdit3("Light specular Color", (float*)&scene->GetLight(0)->specularColor);
			}
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
				if (result == NFD_OKAY) {
					scene->AddModel(Utils::LoadMeshModel(outPath));
					free(outPath);
				}
				else if (result == NFD_CANCEL) {
				}
				else {
				}

			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	}
}