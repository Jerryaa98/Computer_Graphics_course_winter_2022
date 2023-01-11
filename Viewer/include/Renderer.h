#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void Render(const Scene& scene, std::shared_ptr<MeshModel> cameraModel);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void setViewportWidth(const int width);
	void setViewportHeight(const int height);
	void PlotLineHigh(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color);
	void PlotLineLow(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color);
	void DrawLine(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color);
	void CreateBuffers(int w, int h);
	bool pointInTriangle(const glm::ivec2& curr_point, const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3);
	float triangleArea(glm::vec3 v, glm::vec3 u, glm::vec2 w);    
	void DrawTriangle(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, MeshModel& model, int faceIndex, Scene& scene);
	void flatShading(Scene& scene, MeshModel& model, Light& light, float xMin, float xMax, float yMin, float yMax, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3);
	void phongLighting();
	void gouraudShading(Scene& scene, MeshModel& model, Light& light, float xMin, float xMax, float yMin, float yMax, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, int faceIndex);
	void flatShading(Scene& scene, MeshModel& model, Light& light, float xMin, float xMax, float yMin, float yMax, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, bool phong, int faceIndex);
	void phongLighting(Scene& scene, MeshModel& model, Light& light, float xMin, float xMax, float yMin, float yMax, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, int faceIndex);
	void MSAA(float minX, float maxX, float minY, float maxY);
	float maxZ = -1.0f * FLT_MAX;
	float minZ = FLT_MAX;
	bool fogEffect = false;
	float fogStart = -4.5f;
	float fogEnd = -4.1f;
	bool blur = false;
	bool blurred = false;
	int convolutionSize = 3;
	float std = 3;
	bool msaaFlag = false;

private:
	void PutPixel(const int i, const int j, const glm::vec3& color, float depth);
	void Renderer::DrawCircleOfLines(int start_width, int start_height);
	void Renderer::DrawActualCircle(int start_width, int start_height);
	void Renderer::DrawPumpkin(int start_width, int start_height);
	void CreateOpenglBuffer();
	void InitOpenglRendering();
	glm::vec3 fog(glm::vec3 color, float z);
	void blurFilter();
	void blurFilter1();

	float* color_buffer;
	float* z_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};
