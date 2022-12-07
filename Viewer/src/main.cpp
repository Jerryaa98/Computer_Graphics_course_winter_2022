#define _USE_MATH_DEFINES
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

/**
 * Fields
 */
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
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);

/**
 * Function implementation
 */
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
	    DrawImguiMenus(io, scene);
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
				// scene.GetActiveModel().localTranslateVector[0] += newCoordinates[0] - oldCoordinates[0];
				// scene.GetActiveModel().localTranslateVector[1] += -1 * (newCoordinates[1] - oldCoordinates[1]);

				scene.GetActiveCamera().worldRotateArray[1] += (newCoordinates[0] - oldCoordinates[0]) / 10;
				scene.GetActiveCamera().worldRotateArray[0] += (newCoordinates[1] - oldCoordinates[1]) / 10;
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


void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	/**
	 * MeshViewer menu
	 */
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
	// TODO: Add more controls as needed

	ImGui::End();

	/**
	 * Imgui demo - you can remove it once you are familiar with imgui
	 */

	 // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	 /*if (show_demo_window)
		 ImGui::ShowDemoWindow(&show_demo_window);*/

		 //// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		 //{
		 //	static float f = 0.0f;
		 //	static int counter = 0;

		 //	ImGui::Begin("Transformations Window");                          // Create a window called "Hello, world!" and append into it.
		 //	
		 //	ImGui::Text("Here we can control local and global transformations on mesh objects");               // Display some text (you can use a format strings too)
		 //	ImGui::Text("Local Transform:-");
		 //	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		 //	ImGui::Checkbox("Another Window", &show_another_window);

		 //	ImGui::SliderFloat("local scaling", &f, -100.0f, 100.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		 //	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		 //	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		 //		counter++;
		 //	ImGui::SameLine();
		 //	ImGui::Text("counter = %d", counter);

		 //	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		 //	ImGui::End();
		 //}

		 // 3. Show another simple window.
		 //if (show_another_window)
		 //{
		 //	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		 //	ImGui::Text("Hello from another window!");
		 //	if (ImGui::Button("Close Me"))
		 //		show_another_window = false;
		 //	ImGui::End();
		 //}

	int modelCount = scene.GetModelCount();
	int cameraCount = scene.GetCameraCount();
	// display transformations window if at least one model is loaded


	ImGui::Begin("Window");

	if (ImGui::BeginTabBar("Transformations"))
	{


		if (ImGui::BeginTabItem("Camera")) {
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
			ImGui::Separator();
			ImGui::Separator();

			ImGui::InputFloat("Camera Scale", &(*cameraModel).localScale, 0.01, 0.01, "%.2f");

			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("--- CAMERA LOCATION (EYE) ---");

			ImGui::InputFloat("Location X", &camera.eye[0], 0.1, 5, "%.2f");
			ImGui::InputFloat("Location Y", &camera.eye[1], 0.1, 5, "%.2f");
			ImGui::InputFloat("Location Z", &camera.eye[2], 0.1, 5, "%.2f");

			ImGui::Separator();

			ImGui::Text("--- CAMERA TARGET (AT) ---");

			ImGui::InputFloat("Target X", &camera.at[0], 0.1, 1, "%.2f");
			ImGui::InputFloat("Target Y", &camera.at[1], 0.1, 1, "%.2f");
			ImGui::InputFloat("Target Z", &camera.at[2], 0.1, 1, "%.2f");

			ImGui::Separator();
			ImGui::Separator();
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
				ImGui::InputFloat("Local Translate X", &camera.localTranslateArray[0], 0.1, 5, "%.2f");
				ImGui::InputFloat("Local Translate Y", &camera.localTranslateArray[1], 0.1, 5, "%.2f");
				ImGui::InputFloat("Local Translate Z", &camera.localTranslateArray[2], 0.1, 5, "%.2f");

				ImGui::Separator();

				ImGui::Text("Local Rotate:");
				ImGui::InputFloat("Rotate X", &camera.localRotateArray[0], 0.1, 1, "%.2f");
				ImGui::InputFloat("Rotate Y", &camera.localRotateArray[1], 0.1, 1, "%.2f");
				ImGui::InputFloat("Rotate Z", &camera.localRotateArray[2], 0.1, 1, "%.2f");

				ImGui::Separator();
				ImGui::Separator();
				ImGui::Separator();
				ImGui::Separator();
			}
			else {
				ImGui::Text("--- WORLD TRANSFORMATIONS ---");
				ImGui::Text("World Translate:");
				ImGui::InputFloat("World Translate X", &camera.worldTranslateArray[0], 0.1, 5, "%.2f");
				ImGui::InputFloat("World Translate Y", &camera.worldTranslateArray[1], 0.1, 5, "%.2f");
				ImGui::InputFloat("World Translate Z", &camera.worldTranslateArray[2], 0.1, 5, "%.2f");

				ImGui::Separator();

				ImGui::Text("World Rotate:");
				ImGui::InputFloat("Rotate X", &camera.worldRotateArray[0], 0.1, 1, "%.2f");
				ImGui::InputFloat("Rotate Y", &camera.worldRotateArray[1], 0.1, 1, "%.2f");
				ImGui::InputFloat("Rotate Z", &camera.worldRotateArray[2], 0.1, 1, "%.2f");

				ImGui::Separator();
				ImGui::Separator();
				ImGui::Separator();
				ImGui::Separator();
			}

			ImGui::Text("View Type:");
			ImGui::SameLine();
			ImGui::RadioButton("Orthographic", &(camera.OrthoPerspective), 1);
			ImGui::SameLine();
			ImGui::RadioButton("Perspective", &(camera.OrthoPerspective), 0);

			if (camera.OrthoPerspective) {
				ImGui::Text("Orthographic Settings:");
				ImGui::InputFloat("Camera near", &(camera.nearVal), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera far", &(camera.farVal), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera top", &(camera.top), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera bottom", &(camera.bottom), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera right", &(camera.right), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera left", &(camera.left), 0.01f, 1.0f, "%.3f");
			}
			else {
				ImGui::InputFloat("Camera near", &(camera.nearVal), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera far", &(camera.farVal), 0.01f, 1.0f, "%.3f");
				ImGui::InputFloat("Camera FOV", &(camera.fov), 1, 1, "%.0f");
			}

			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();


			ImGui::Text("Addons:");
			ImGui::Checkbox("Draw World Axis", &(scene.drawWorldAxis));

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Object"))
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
					ImGui::InputFloat("Local Scale X", &model.localScaleVector[0], 0.1f, 1.0f, "%.3f");
					ImGui::InputFloat("Local Scale Y", &model.localScaleVector[1], 0.1f, 1.0f, "%.3f");
					ImGui::InputFloat("Local Scale Z", &model.localScaleVector[2], 0.1f, 1.0f, "%.3f");

					ImGui::Separator();

					ImGui::Checkbox("Lock All Local Axis", &(model.uniformLocalScale));
					ImGui::InputFloat("Local Scale Locked", &(model.localScale), 0.1f, 1.0f, "%.3f");

					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();

					ImGui::Text("Local Translate:");
					ImGui::InputFloat("Local Translate X", &model.localTranslateVector[0], 0.05, 1, "%.2f");
					ImGui::InputFloat("Local Translate Y", &model.localTranslateVector[1], 0.05, 1, "%.2f");
					ImGui::InputFloat("Local Translate Z", &model.localTranslateVector[2], 0.05, 1, "%.2f");

					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();

					ImGui::Text("Local Rotate:");
					ImGui::InputFloat("Local Rotate X", &model.localRotateVector[0], 1, 1, "%.0f");
					ImGui::InputFloat("Local Rotate Y", &model.localRotateVector[1], 1, 1, "%.0f");
					ImGui::InputFloat("Local Rotate Z", &model.localRotateVector[2], 1, 1, "%.0f");

					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
				}
				else {
					ImGui::Text("--- WORLD TRANSFORMATIONS ---");
					ImGui::Text("World Scale:");
					ImGui::InputFloat("World Scale X", &model.worldScaleVector[0], 0.1f, 1.0f, "%.3f");
					ImGui::InputFloat("World Scale Y", &model.worldScaleVector[1], 0.1f, 1.0f, "%.3f");
					ImGui::InputFloat("World Scale Z", &model.worldScaleVector[2], 0.1f, 1.0f, "%.3f");
					ImGui::Separator();
					ImGui::Checkbox("Lock All World Axis", &(model.uniformWorldScale));
					ImGui::InputFloat("World Scale Locked", &(model.worldScale), 0.1f, 1.0f, "%.3f");

					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();

					ImGui::Text("World Translate:");
					ImGui::InputFloat("World Translate X", &model.worldTranslateVector[0], 0.05, 1, "%.2f");
					ImGui::InputFloat("World Translate Y", &model.worldTranslateVector[1], 0.05, 1, "%.2f");
					ImGui::InputFloat("World Translate Z", &model.worldTranslateVector[2], 0.05, 1, "%.2f");

					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();

					ImGui::Text("World Rotate:");
					ImGui::InputFloat("World Rotate X", &model.worldRotateVector[0], 1, 1, "%.0f");
					ImGui::InputFloat("World Rotate Y", &model.worldRotateVector[1], 1, 1, "%.0f");
					ImGui::InputFloat("World Rotate Z", &model.worldRotateVector[2], 1, 1, "%.0f");

					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();

				}
				ImGui::Text("Object Addons:");
				ImGui::Checkbox("Draw Axis", &(model.drawAxis));
				ImGui::Separator();
				ImGui::Checkbox("Draw Bounding Box", &(model.drawBoundingBox));
				ImGui::Separator();
				ImGui::Checkbox("Draw Vertex Normals", &(model.drawVertexNormals));
				ImGui::InputFloat("Vertex Normals Scale", &(model.vertexNormalsScale), 0.05, 1, "%.2f");
				ImGui::Separator();
				ImGui::Checkbox("Draw Face Normals", &(model.drawFaceNormals));
				ImGui::InputFloat("Face Normals Scale", &(model.faceNormalsScale), 0.0001, 1, "%.4f");
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}