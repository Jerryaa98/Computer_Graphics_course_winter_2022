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
glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);
ImVec2 oldCoordinates;

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
				scene.GetActiveModel().localTranslateVector[0] += newCoordinates[0] - oldCoordinates[0];
				scene.GetActiveModel().localTranslateVector[1] += -1 * (newCoordinates[1] - oldCoordinates[1]);
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
	renderer.Render(scene);
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
	// display transformations window if at least one model is loaded
	if (modelCount) {
		ImGui::Begin("Transformations Window");

		ImGui::Text("Loaded Models List:");

		static int selected = 0;
		for (int n = 0; n < modelCount; n++) {
			char buf[64];
			sprintf(buf, scene.modelsList[n].c_str(), n);
			if (ImGui::Selectable(buf, selected == n))
				selected = n;
		}

		scene.SetActiveModelIndex(selected);
		MeshModel& model = scene.GetActiveModel();

		// scaling
		ImGui::Text("Local Scaling:");
		ImGui::SliderFloat3("Local X,Y,Z Axis Scaling", model.localScaleVector, 1, model.maxScale);

		ImGui::Checkbox("Local Uniform Scaling", &(model.uniformLocalScale));

		ImGui::SliderFloat("Local Uniform Scale Bar", &(model.localScale), 1, model.maxScale);

		ImGui::Text("World Scaling:");
		ImGui::SliderFloat3("World X,Y,Z Axis Scaling", model.worldScaleVector, 1, 3000);

		ImGui::Checkbox("World Uniform Scaling", &(model.uniformWorldScale));
		ImGui::SliderFloat("World Uniform Scale Bar", &(model.worldScale), 1, 1000);

		// translates
		ImGui::Text("Local Translate:");
		ImGui::SliderFloat3("Local Translate", model.localTranslateVector, -1000, 1000);

		ImGui::Text("World Translate:");
		ImGui::SliderFloat3("World Translate", model.worldTranslateVector, -1000, 1000);

		// rotation
		ImGui::Text("Local Rotate:");
		ImGui::SliderFloat3("Local Rotate", model.localRotateVector, -360, 360);


		ImGui::Text("World Rotate:");
		ImGui::SliderFloat3("World Rotate", model.worldRotateVector, -360, 360);

		ImGui::End();
	}
}