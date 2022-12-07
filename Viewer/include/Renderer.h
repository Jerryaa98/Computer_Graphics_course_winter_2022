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
	void PlotLineHigh(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void PlotLineLow(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void CreateBuffers(int w, int h);
	
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void Renderer::DrawCircleOfLines(int start_width, int start_height);
	void Renderer::DrawActualCircle(int start_width, int start_height);
	void Renderer::DrawPumpkin(int start_width, int start_height);
	void CreateOpenglBuffer();
	void InitOpenglRendering();

	float* color_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};
