#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
//#include "E:\opencv\build\include\opencv2\opencv.hpp"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))


Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenglRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer;
}

glm::vec3 Renderer::fog(glm::vec3 color, float z) {
	float f = (fogEnd - z) / (fogEnd / fogStart);
	return (f * color) + ((1 - f) * glm::vec3(0.5f, 0.5f, 0.5f));
}

void Renderer::blurFilter() {
	std::cout << 1 << std::endl;
	float* newColorBuffer = new float[3 * viewport_width * viewport_height];

	//float** GKernel = new float* [convolutionSize];
	//for (int i = 0; i < convolutionSize; ++i)
	//    GKernel[i] = new float[convolutionSize];

	std::vector<std::vector<float>> GKernel;
	for (int i = 0; i < convolutionSize; ++i) {
		std::vector<float> arr;
		for (int j = 0; j < convolutionSize; ++j) {
			arr.push_back(0);
		}
		GKernel.push_back(arr);
	}

	double r, s = 2.0 * std * std;

	float sum = 0.0;
	std::cout << 2 << std::endl;

	for (int x = -(convolutionSize - 1) / 2; x <= (convolutionSize - 1) / 2; x++) {
		for (int y = (convolutionSize - 1) / 2; y <= (convolutionSize - 1) / 2; y++) {
			r = sqrt(x * x + y * y);
			GKernel[x + ((convolutionSize - 1) / 2)][y + ((convolutionSize - 1) / 2)] = (exp(-(r * r) / s)) / (M_PI * s);
			sum += GKernel[x + ((convolutionSize - 1) / 2)][y + ((convolutionSize - 1) / 2)];
		}
	}
	std::cout << 3 << std::endl;

	for (int i = 0; i < convolutionSize; ++i)
		for (int j = 0; j < convolutionSize; ++j)
			GKernel[i][j] /= sum;

	std::cout << 3.5 << std::endl;

	int radius = (convolutionSize - 1) / 2;

	//glm::vec3** window = new glm::vec3* [convolutionSize];
	//for (int i = 0; i < convolutionSize; ++i) {
	//    window[i] = new glm::vec3[convolutionSize];
	//}

	std::vector<std::vector<glm::vec3>> window;
	for (int i = 0; i < convolutionSize; ++i) {
		std::vector<glm::vec3> arr;
		for (int j = 0; j < convolutionSize; ++j) {
			arr.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		window.push_back(arr);
	}

	std::cout << 4 << std::endl;
	for (int i = 0; i < viewport_width; i++) {
		for (int j = 0; j < radius; j++) {
			newColorBuffer[INDEX(viewport_width, i, j, 0)] = color_buffer[INDEX(viewport_width, i, j, 0)];
			newColorBuffer[INDEX(viewport_width, i, j, 1)] = color_buffer[INDEX(viewport_width, i, j, 1)];
			newColorBuffer[INDEX(viewport_width, i, j, 2)] = color_buffer[INDEX(viewport_width, i, j, 2)];
			newColorBuffer[INDEX(viewport_width, i, viewport_height - 1 - j, 0)] = color_buffer[INDEX(viewport_width, i, viewport_height - 1 - j, 0)];
			newColorBuffer[INDEX(viewport_width, i, viewport_height - 1 - j, 1)] = color_buffer[INDEX(viewport_width, i, viewport_height - 1 - j, 1)];
			newColorBuffer[INDEX(viewport_width, i, viewport_height - 1 - j, 2)] = color_buffer[INDEX(viewport_width, i, viewport_height - 1 - j, 2)];
		}
	}


	for (int j = 0; j < viewport_height; j++) {
		for (int i = 0; i < radius; i++) {
			newColorBuffer[INDEX(viewport_width, i, j, 0)] = color_buffer[INDEX(viewport_width, i, j, 0)];
			newColorBuffer[INDEX(viewport_width, i, j, 1)] = color_buffer[INDEX(viewport_width, i, j, 1)];
			newColorBuffer[INDEX(viewport_width, i, j, 2)] = color_buffer[INDEX(viewport_width, i, j, 2)];
			newColorBuffer[INDEX(viewport_width, viewport_width - 1 - i, j, 0)] = color_buffer[INDEX(viewport_width, viewport_width - 1 - i, j, 0)];
			newColorBuffer[INDEX(viewport_width, viewport_width - 1 - i, j, 1)] = color_buffer[INDEX(viewport_width, viewport_width - 1 - i, j, 1)];
			newColorBuffer[INDEX(viewport_width, viewport_width - 1 - i, j, 2)] = color_buffer[INDEX(viewport_width, viewport_width - 1 - i, j, 2)];

		}
	}
	std::cout << 5 << std::endl;

	for (int i = radius; i < viewport_width - radius; i++) {
		for (int j = radius; j < viewport_height - radius; j++) {
			// make window
			int windowIndexI = 0;
			int windowIndexJ = 0;
			for (int indexI = i - radius; indexI <= i + radius; indexI++, windowIndexI++) {
				for (int indexJ = j - radius; indexJ <= j + radius; indexJ++, windowIndexJ++) {
					glm::vec3 color = glm::vec3(color_buffer[INDEX(viewport_width, indexI, indexJ, 0)], color_buffer[INDEX(viewport_width, indexI, indexJ, 1)], color_buffer[INDEX(viewport_width, indexI, indexJ, 2)]);
					window[windowIndexI][windowIndexJ] = color;
				}
			}

			glm::vec3 sum = glm::vec3(0.0f, 0.0f, 0.0f);
			for (windowIndexI = 0; windowIndexI < convolutionSize; windowIndexI++) {
				for (windowIndexJ = 0; windowIndexJ < convolutionSize; windowIndexJ++) {
					sum += (window[windowIndexI][windowIndexJ] * GKernel[windowIndexI][windowIndexJ]);
				}
			}

			//float maximum = max(max(sum.x, sum.y), sum.z);
			float maximum = 1.0f;

			newColorBuffer[INDEX(viewport_width, i, j, 0)] = sum.x / maximum;
			newColorBuffer[INDEX(viewport_width, i, j, 1)] = sum.y / maximum;
			newColorBuffer[INDEX(viewport_width, i, j, 2)] = sum.z / maximum;

		}
	}

	//std::cout << 6 << std::endl;
	//
	//for (int i = 0; i < convolutionSize; ++i)
	//    delete[] GKernel[i];
	//std::cout << 7 << std::endl;
	//
	//delete[] GKernel;
	//std::cout << 8 << std::endl;
	//
	//for (int i = 0; i < convolutionSize; ++i)
	//    delete[] window[i];
	//std::cout << 9 << std::endl;
	//
	//delete[] window;
	//std::cout << 10 << std::endl;

	//delete[] color_buffer;
	std::cout << 11 << std::endl;
	color_buffer = newColorBuffer;
	std::cout << 12 << std::endl;
}


void Renderer::PutPixel(int i, int j, const glm::vec3& color, float depth) {
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;

	if (z_buffer[Z_INDEX(viewport_width, i, j)] > depth)
		return;
	
	//std::cout << "i is " << i << ", j is " << j << std::endl;
	glm::vec3 newColor = glm::vec3(color);

	if (fogEffect) {
		newColor = fog(color, depth);
	}

	color_buffer[INDEX(viewport_width, i, j, 0)] = newColor.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = newColor.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = newColor.z;
	z_buffer[Z_INDEX(viewport_width, i, j)] = depth;
}

void Renderer::PlotLineHigh(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	int xi = 1;

	float z = (p1.z + p2.z) / 2;

	if (dx < 0) {
		xi = -1;
		dx = ((-1) * dx);
	}

	float d = (2 * dx) - dy;
	float x = p1.x;
	for (int y = p1.y; y < p2.y; y++) {
		PutPixel(x, y, color, z);
		if (d > 0) {
			x += xi;
			d += (2 * (dx - dy));
		}
		else {
			d += (2 * dx);
		}
	}
}


void Renderer::PlotLineLow(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	int yi = 1;

	float z = (p1.z + p2.z) / 2;

	if (dy < 0) {
		yi = -1;
		dy = ((-1) * dy);
	}

	float d = (2 * dy) - dx;
	float y = p1.y;


	for (int x = p1.x; x < p2.x; x++) {
		PutPixel(x, y, color, z);
		if (d > 0) {
			y += yi;
			d += (2 * (dy - dx));
		}
		else {
			d += (2 * dy);
		}
	}
}

void Renderer::DrawLine(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color)
{
	// bresenham algorithm https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	float dx = abs(p1.x - p2.x);
	float dy = abs(p1.y - p2.y);

	if (dy < dx) {
		if (p1.x > p2.x) {
			PlotLineLow(p2, p1, color);
			return;
		}
		else {
			PlotLineLow(p1, p2, color);
			return;
		}
	}
	else {
		if (p1.y > p2.y) {
			PlotLineHigh(p2, p1, color);
			return;
		}
		else {
			PlotLineHigh(p1, p2, color);
			return;
		}
	}


}

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	z_buffer = new float[w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenglRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::CreateOpenglBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::blurFilter1() {
//	cv::Mat R = cv::Mat::zeros(viewport_height, viewport_width, CV_8UC1);
//	cv::Mat G = cv::Mat::zeros(viewport_height, viewport_width, CV_8UC1);
//	cv::Mat B = cv::Mat::zeros(viewport_height, viewport_width, CV_8UC1);
//	std::cout << 1 << std::endl;
//
//	for (int i = 0; i < viewport_width; i++) {
//		for (int j = 0; j < viewport_height; j++) {
//			std::cout << i << " " << j << std::endl;
//			color_buffer[INDEX(viewport_width, i, j, 0)];
//			std::cout << 1 << std::endl;
//			R.at<float>(j, i);
//			std::cout << 2 << std::endl;
//			R.at<float>(j, i) = color_buffer[INDEX(viewport_width, i, j, 0)];
//			G.at<float>(j, i) = color_buffer[INDEX(viewport_width, i, j, 1)];
//			B.at<float>(j, i) = color_buffer[INDEX(viewport_width, i, j, 2)];
//			std::cout << 3 << std::endl;
//		}
//	}
//	std::cout << 2 << std::endl;
//	cv::Mat Ro;
//	cv::Mat Go;
//	cv::Mat Bo;
//
//	cv::GaussianBlur(R, Ro, cv::Size(convolutionSize, convolutionSize), std);
//	cv::GaussianBlur(G, Go, cv::Size(convolutionSize, convolutionSize), std);
//	cv::GaussianBlur(B, Bo, cv::Size(convolutionSize, convolutionSize), std);
//	std::cout << 3 << std::endl;
//
//	for (int i = 0; i < viewport_width; i++) {
//		for (int j = 0; j < viewport_height; j++) {
//			color_buffer[INDEX(viewport_width, i, j, 0)] = Ro.at<float>(i, j);
//			color_buffer[INDEX(viewport_width, i, j, 1)] = Go.at<float>(i, j);
//			color_buffer[INDEX(viewport_width, i, j, 2)] = Bo.at<float>(i, j);
//		}
//	}
//	std::cout << 4 << std::endl;
//
}


void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::MSAA(float minX, float maxX, float minY, float maxY) {
	for (int i = minX; i < maxX; i++) {
		for (int j = minY; j < maxY; j++) {
			if (z_buffer[Z_INDEX(viewport_width, i, j)] != -1.0f * FLT_MAX)
			{
				if (i != 0 && j != 0 && i != (viewport_width - 1) && j != (viewport_height - 1))
				{

					bool yesFlag = false;
					for (int i1 = i - 1; i1 <= i + 1; i1++) {
						for (int j1 = j - 1; j1 <= j + 1; j1++) {
							if (z_buffer[Z_INDEX(viewport_width, i1, j1)] == -1.0f * FLT_MAX)
								yesFlag = true;
						}
					}
					glm::vec3 finalColor = glm::vec3(0.0f, 0.0f, 0.0f);

					if (yesFlag) {
						for (int i1 = i - 1; i1 <= i + 1; i1++) {
							for (int j1 = j - 1; j1 <= j + 1; j1++) {
								glm::vec3 pointColor = glm::vec3(color_buffer[INDEX(viewport_width, i1, j1, 0)], color_buffer[INDEX(viewport_width, i1, j1, 1)], color_buffer[INDEX(viewport_width, i1, j1, 2)]);
								int distance = pow((i - i1), 2) + pow((j - j1), 2);

								// give different weights according to pixel distance
								if (distance == 0)
									finalColor += (pointColor / 4.0f);
								if (distance == 1)
									finalColor += (pointColor / 8.0f);
								if (distance == 2)
									finalColor += (pointColor / 16.0f);
							}
						}
						PutPixel(i, j, finalColor, 100.0f);
					}
				}
			}
		}
	}
}


void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
			color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
			color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
			z_buffer[Z_INDEX(viewport_width, i, j)] = -1.0f * FLT_MAX;
		}
	}
}

void Renderer::DrawCircleOfLines(int start_width, int start_height) {
	const double pi = 3.14159265358979323846;
	for (int i = 1; i <= 80; i++) {
		Renderer::DrawLine(glm::ivec3(start_width, start_height, 1), glm::ivec3(start_width + 200*(sin((2*pi*i)/45)), start_height + 200 * (cos((2 * pi * i) / 45)), 1), glm::vec3(1, 1, 1));
	}
}

void Renderer::DrawActualCircle(int start_width, int start_height) {
	const double pi = 3.14159265358979323846;
	int length = 200;
	glm::ivec3 p1 = glm::ivec3(start_width, start_height, 1);
	glm::ivec3 p2 = glm::ivec3(start_width + length * (sin((2 * pi * 0) / 360)), start_height + length * (cos((2 * pi * 0) / 360)), 1);
	for (double i = 1; i <= 360; i+=0.5) {
		p1 = p2;
		p2 = glm::ivec3(start_width + length * (sin((2 * pi * i) / 360)), start_height + length * (cos((2 * pi * i) / 360)), 1);
		Renderer::DrawLine(p1, p2,  glm::vec3(1, 0, 0));
	}
}

//void Renderer::DrawPumpkin(int start_width, int start_height) {
//	// face
//	glm::ivec3 p1 = glm::ivec3(start_width, start_height, 1);
//	glm::ivec3 p2 = glm::ivec3(start_width + 200, start_height, 1);
//	glm::vec3 orange = glm::vec3(0.99, 0.41, 0);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 50, p1.y + 50);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 50, p1.y + 75);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y + 150);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 50, p1.y + 75);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 50, p1.y + 50);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 200, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 50, p1.y - 50);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 50, p1.y - 75);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 150);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 50, p1.y - 75);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 50, p1.y - 50);
//	Renderer::DrawLine(p1, p2, orange);
//
//	// mouth
//	p1 = glm::ivec2(start_width - 40, start_height + 170);
//	p2 = glm::ivec2(p1.x + 80, p1.y - 60);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 60, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 60, p1.y - 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 80, p1.y + 60);
//	Renderer::DrawLine(p1, p2, orange);
//
//	p1 = glm::ivec2(start_width - 40, start_height + 170);
//	p2 = glm::ivec2(p1.x + 80, p1.y - 120);
//	Renderer::DrawLine(p1, p2, orange);
//
//	for (int i = 0; i < 6; i++) {
//		p1 = p2;
//		p2 = glm::ivec2(p1.x + 10, p1.y + 10);
//		Renderer::DrawLine(p1, p2, orange);
//		p1 = p2;
//		p2 = glm::ivec2(p1.x + 10, p1.y - 10);
//		Renderer::DrawLine(p1, p2, orange);
//	}
//
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 80, p1.y + 120);
//	Renderer::DrawLine(p1, p2, orange);
//
//	// eyes
//	p1 = glm::ivec2(start_width + 10, start_height + 300);
//	p2 = glm::ivec2(p1.x + 30, p1.y + 60);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 30, p1.y - 60);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 60, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//
//	p1 = glm::ivec2(start_width + 190, start_height + 300);
//	p2 = glm::ivec2(p1.x - 30, p1.y + 60);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 30, p1.y - 60);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 60, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//
//	// nose
//	p1 = glm::ivec2(start_width + 80, start_height + 220);
//	p2 = glm::ivec2(p1.x + 40, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 20, p1.y - 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 20, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//hat 
//
//	glm::vec3 green = glm::vec3(0.1, 0.43, 0);
//	p1 = glm::ivec2(start_width + 60, start_height + 400);
//	p2 = glm::ivec2(p1.x + 10, p1.y + 15);
//	Renderer::DrawLine(p1, p2, green);
//
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 30, p1.y + 25);
//	Renderer::DrawLine(p1, p2, green);
//
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 60, p1.y + 30);
//	Renderer::DrawLine(p1, p2, green);
//
//	p1 = p2;
//	glm::ivec2 p3 = p2;
//	p2 = glm::ivec2(p1.x + 10, p1.y - 5);
//	Renderer::DrawLine(p1, p2, green);
//
//	glm::vec3 brown = glm::vec3(0.33, 0.21, 0.125);
//
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 16, p1.y - 7);
//	Renderer::DrawLine(p3, p2, brown);
//
//	p2 = glm::ivec2(p1.x + 20, p1.y - 5);
//	Renderer::DrawLine(p3, p2, brown);
//	p2 = glm::ivec2(p1.x + 29, p1.y - 2);
//	Renderer::DrawLine(p3, p2, brown);
//	p2 = glm::ivec2(p1.x + 32, p1.y - 0.5);
//	Renderer::DrawLine(p3, p2, brown);
//	p2 = glm::ivec2(p1.x + 35, p1.y);
//	Renderer::DrawLine(p3, p2, brown);
//
//	p1 = glm::ivec2(start_width + 110, start_height + 400);
//	p2 = glm::ivec2(p1.x + 10, p1.y + 15);
//	Renderer::DrawLine(p1, p2, green);
//
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 30, p1.y + 25);
//	Renderer::DrawLine(p1, p2, green);
//
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 60, p1.y + 30);
//	Renderer::DrawLine(p1, p2, green);
//	Renderer::DrawLine(p3, p2, green);
//
//
//	// happy halloweeen
//		//H
//
//	int y = start_height + 500;
//	p1 = glm::ivec2(start_width - 130, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = glm::ivec2(p1.x, p1.y + 20);
//	p2 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = glm::ivec2(p2.x, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//A
//
//	p1 = glm::ivec2(p2.x + 10, y);
//	p2 = glm::ivec2(p1.x + 10, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 10, p1.y - 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p3 = p2;
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 5, p1.y + 20);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 8, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p2 = p3;
//
//	//P
//
//	p1 = glm::ivec2(p2.x + 10, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 15, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 5, p1.y - 2);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 2, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 5, p1.y - 1);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 10, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//P
//
//	p1 = glm::ivec2(p2.x + 30, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 15, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 5, p1.y - 2);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 2, p1.y - 4);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 5, p1.y - 1);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 10, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//Y
//
//	p1 = glm::ivec2(p2.x + 30, y + 40);
//	p2 = glm::ivec2(p1.x + 10, p1.y - 20);
//	Renderer::DrawLine(p1, p2, orange);
//	p3 = p2;
//	p1 = p2;
//	p2 = p2 = glm::ivec2(p1.x, p1.y - 20);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p3;
//	p2 = glm::ivec2(p1.x + 10, p1.y + 20);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//H
//
//	y = start_height + 500;
//	p1 = glm::ivec2(p2.x + 30, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = glm::ivec2(p1.x, p1.y + 20);
//	p2 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = glm::ivec2(p2.x, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//A
//
//	p1 = glm::ivec2(p2.x + 10, y);
//	p2 = glm::ivec2(p1.x + 10, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 10, p1.y - 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p3 = p2;
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 5, p1.y + 20);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 8, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p2 = p3;
//
//	//L
//
//	p1 = glm::ivec2(p2.x + 10, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p2 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//L
//
//	p1 = glm::ivec2(p2.x + 10, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p2 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//O
//	p1 = glm::ivec2(p2.x + 10, y);
//	p3 = p1;
//	p2 = glm::ivec2(p1.x - 2, p1.y + 3);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y + 30);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 1, p1.y + 1);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 5, p1.y + 3);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 7, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 7, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 5, p1.y - 3);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 30);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 6, p1.y - 1);
//	Renderer::DrawLine(p1, p2, orange);
//	Renderer::DrawLine(p3, p2, orange);
//
//	//O
//	p1 = glm::ivec2(p2.x + 20, y);
//	p3 = p1;
//	p2 = glm::ivec2(p1.x - 2, p1.y + 3);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y + 30);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 1, p1.y + 1);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 5, p1.y + 3);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 7, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 7, p1.y);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 5, p1.y - 3);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y - 30);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x - 6, p1.y - 1);
//	Renderer::DrawLine(p1, p2, orange);
//	Renderer::DrawLine(p3, p2, orange);
//	//W
//	p1 = glm::ivec2(p2.x + 20, y + 40);
//	p2 = glm::ivec2(p1.x + 10, p1.y - 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 10, p1.y + 20);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 10, p1.y - 20);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 10, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//
//	//E
//
//	p1 = glm::ivec2(p2.x + 10, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p3 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p3, orange);
//	p3 = glm::ivec2(p2.x + 20, p2.y);
//	Renderer::DrawLine(p2, p3, orange);
//	p1 = glm::ivec2(p1.x, p1.y + 20);
//	p3 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p3, orange);
//
//	//E
//	p1 = glm::ivec2(p2.x + 30, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p3 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p3, orange);
//	p3 = glm::ivec2(p2.x + 20, p2.y);
//	Renderer::DrawLine(p2, p3, orange);
//	p1 = glm::ivec2(p1.x, p1.y + 20);
//	p3 = glm::ivec2(p1.x + 20, p1.y);
//	Renderer::DrawLine(p1, p3, orange);
//
//	//N 
//	p1 = glm::ivec2(p2.x + 30, y);
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x + 20, p1.y - 40);
//	Renderer::DrawLine(p1, p2, orange);
//	p1 = p2;
//	p2 = glm::ivec2(p1.x, p1.y + 40);
//	Renderer::DrawLine(p1, p2, orange);
//	//lama'ss comment 
//	
//}

bool Renderer::pointInTriangle(const glm::ivec2& point, const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3){
	float val1 = (p1.x - p3.x) * (point.y - p3.y) - (p1.y - p3.y) * (point.x - p3.x);
	float val2 = (p2.x - p1.x) * (point.y - p1.y) - (p2.y - p1.y) * (point.x - p1.x);
	if ((val1 < 0) != (val2 < 0) && val1 != 0 && val2 != 0)
		return false;
	float val3 = (p3.x - p2.x) * (point.y - p2.y) - (p3.y - p2.y) * (point.x - p2.x);
	if (val3 == 0 || ((val3 < 0) == (val1 + val2 <= 0)))
		return true;
	return false;
}

float Renderer::triangleArea(glm::vec3 p1, glm::vec3 p2, glm::vec2 w) {
	float vu, uw, vw, dx, dy, s;
	dx = p1.x - w.x;
	dy = p1.y - w.y;
	vw = sqrt(dx * dx + dy * dy);
	dx = p2.x - w.x;
	dy = p2.y - w.y;
	uw = sqrt(dx * dx + dy * dy);
	dx = p1.x - p2.x;
	dy = p1.y - p2.y;
	vu = sqrt(dx * dx + dy * dy);
	s = (vu + uw + vw) / 2;
	return  sqrt(s * (s - vu) * (s - uw) * (s - vw));
}


void Renderer::flatShading(Scene& scene, MeshModel& model, Light& light, float xMin, float xMax, float yMin, float yMax, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, bool phong, int faceIndex) {
	float a1 = 0;
	float a2 = 0;
	float a3 = 0;
	float A = 0;
	float z = 0;

	glm::vec3 U = p2 - p1;
	glm::vec3 V = p3 - p1;

	float x = (U.y * V.z) - (U.z * V.y);
	float y = (U.z * V.x) - (U.x * V.z);
	float z1 = (U.x * V.y) - (U.y * V.x);

	glm::vec3 normal = glm::normalize(glm::vec3(x, y, z1));

	const Face& face = model.GetFace(faceIndex);
	int p1Index = face.GetVertexIndex(0) - 1;
	int p2Index = face.GetVertexIndex(1) - 1;
	int p3Index = face.GetVertexIndex(2) - 1;

	int p1NormalIndex = face.GetNormalIndex(0) - 1;
	int p2NormalIndex = face.GetNormalIndex(1) - 1;
	int p3NormalIndex = face.GetNormalIndex(2) - 1;

	glm::vec3 p1normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p2normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p3normal = glm::vec3(0.0f, 0.0f, 0.0f);

	p1normal = model.normals[p1NormalIndex];
	p2normal = model.normals[p2NormalIndex];
	p3normal = model.normals[p3NormalIndex];

	glm::vec4 normalTemp = model.objectTransform * glm::vec4(p1normal, 1.0f);
	p1normal = glm::vec3(normalTemp) / normalTemp.w;

	normalTemp = model.objectTransform * glm::vec4(p2normal, 1.0f);
	p2normal = glm::vec3(normalTemp) / normalTemp.w;

	normalTemp = model.objectTransform * glm::vec4(p3normal, 1.0f);
	p3normal = glm::vec3(normalTemp) / normalTemp.w;


	for (int y = yMax; y >= yMin; y--) {
		for (int x = xMin; x <= xMax; x++) {
			glm::ivec2 point = glm::ivec2(x, y);
			if (pointInTriangle(point, p1, p2, p3)) {
				a1 = triangleArea(p1, p2, point);
				a2 = triangleArea(p2, p3, point);
				a3 = triangleArea(p1, p3, point);

				A = a1 + a2 + a3;
				z = ((a1 * p3.z) / A) + ((a2 * p1.z) / A) + ((a3 * p2.z) / A);

				glm::vec3 tempNormal;
				if (phong) {
					tempNormal = ((a1 * p3normal) / A) + ((a2 * p1normal) / A) + ((a3 * p2normal) / A);
				}
				else {
					tempNormal = normal;
				}

				// Ambient
				glm::vec3 Ia = light.highValue1 * light.ambientColor * model.ambientColor;
				//cout << glm::to_string(Ia) << endl;


				// Diffuse
				glm::vec3 lightVector = glm::normalize(light.updatedLocation - glm::vec3(x, y, z));
				glm::vec3 Id = light.highValue1 * 1000.0f * light.diffuseColor * model.diffuseColor * max((float)glm::dot(tempNormal, lightVector), 0.0f);
				//cout << glm::to_string(Id) << endl;


				// Specular
				float* arr = scene.GetActiveCamera().eye;
				glm::vec3 R = glm::normalize((2 * glm::dot(lightVector, tempNormal) * tempNormal) - lightVector);				glm::vec3 cameraLocation = glm::vec3(arr[0], arr[1], arr[2]);
				glm::vec3 V = glm::normalize(cameraLocation - glm::vec3(x, y, z));
				glm::vec3 Is = light.highValue1 * light.specularColor * model.specularColor * pow(max((float)glm::dot(R, V), 0.0f), light.alpha);
				glm::vec3 finalColor = 0.3f * (Ia + Id + Is);
				//finalColor /= max(max(finalColor.x, finalColor.y), finalColor.z);
				PutPixel(x, y, finalColor, z);
			}
		}
	}
}

void Renderer::gouraudShading(Scene& scene, MeshModel& model, Light& light, float xMin, float xMax, float yMin, float yMax, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, int faceIndex) {
	float a1 = 0;
	float a2 = 0;
	float a3 = 0;
	float A = 0;
	float z = 0;

	const Face& face = model.GetFace(faceIndex);
	int p1Index = face.GetVertexIndex(0) - 1;
	int p2Index = face.GetVertexIndex(1) - 1;
	int p3Index = face.GetVertexIndex(2) - 1;

	int p1NormalIndex = face.GetNormalIndex(0) - 1;
	int p2NormalIndex = face.GetNormalIndex(1) - 1;
	int p3NormalIndex = face.GetNormalIndex(2) - 1;

	glm::vec3 p1normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p2normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p3normal = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < model.verticesToNormals[p1Index].size(); i++)
		p1normal += model.transformedNormals[model.verticesToNormals[p1Index][i]];
	//p1normal /= model.verticesToNormals[p1Index].size();

	for (int i = 0; i < model.verticesToNormals[p2Index].size(); i++)
		p2normal += model.transformedNormals[model.verticesToNormals[p2Index][i]];
	//p2normal /= model.verticesToNormals[p2Index].size();

	for (int i = 0; i < model.verticesToNormals[p3Index].size(); i++)
		p3normal += model.transformedNormals[model.verticesToNormals[p3Index][i]];
	//p3normal /= model.verticesToNormals[p3Index].size();

	p1normal = glm::normalize(p1normal);
	p2normal = glm::normalize(p2normal);
	p3normal = glm::normalize(p3normal);

	p1normal = model.normals[p1NormalIndex];
	p2normal = model.normals[p2NormalIndex];
	p3normal = model.normals[p3NormalIndex];

	glm::vec4 normalTemp = model.objectTransform * glm::vec4(p1normal, 1.0f);
	p1normal = glm::vec3(normalTemp) / normalTemp.w;

	normalTemp = model.objectTransform * glm::vec4(p2normal, 1.0f);
	p2normal = glm::vec3(normalTemp) / normalTemp.w;

	normalTemp = model.objectTransform * glm::vec4(p3normal, 1.0f);
	p3normal = glm::vec3(normalTemp) / normalTemp.w;


	////////// p1 color
	// Ambient
	glm::vec3 Ia = light.highValue1 * light.ambientColor * model.ambientColor;

	// Diffuse
	glm::vec3 lightVector = glm::normalize(light.updatedLocation - p1);
	glm::vec3 Id = light.highValue2 * light.diffuseColor * max((float)glm::dot(p1normal, lightVector), 0.0f) * model.diffuseColor;
	// Specular
	float* arr = scene.GetActiveCamera().eye;
	glm::vec3 R = glm::normalize((2 * glm::dot(lightVector, p1normal) * p1normal) - lightVector);
	glm::vec3 cameraLocation = glm::vec3(arr[0], arr[1], arr[2]);
	glm::vec3 V = glm::normalize(cameraLocation - p1);
	glm::vec3 Is = light.highValue3 * light.specularColor * model.specularColor * pow(max((float)glm::dot(R, V), 0.0f), light.alpha);

	glm::vec3 p1Color = (Ia + Id + Is);
	PutPixel(p1.x, p1.y, p1Color, p1.z);


	////////// p2 color
	// Ambient
	Ia = light.highValue1 * light.ambientColor * model.ambientColor;

	// Diffuse
	lightVector = glm::normalize(light.updatedLocation - p2);
	Id = light.highValue2 * light.diffuseColor * max((float)glm::dot(p2normal, lightVector), 0.0f) * model.diffuseColor;
	// Specular
	arr = scene.GetActiveCamera().eye;
	R = glm::normalize((2 * glm::dot(lightVector, p2normal) * p2normal) - lightVector);
	cameraLocation = glm::vec3(arr[0], arr[1], arr[2]);
	V = glm::normalize(cameraLocation - p2);
	Is = light.highValue3 * light.specularColor * pow(max((float)glm::dot(R, V), 0.0f), light.alpha) * model.specularColor;
	glm::vec3 p2Color = (Ia + Id + Is);
	PutPixel(p2.x, p2.y, p2Color, p2.z);


	////////// p3 color
	// Ambient
	Ia = light.highValue1 * light.ambientColor * model.ambientColor;

	// Diffuse
	lightVector = glm::normalize(light.updatedLocation - p3);
	Id = light.highValue2 * light.diffuseColor * max((float)glm::dot(p3normal, lightVector), 0.0f) * model.diffuseColor;
	// Specular
	arr = scene.GetActiveCamera().eye;
	R = glm::normalize((2 * glm::dot(lightVector, p3normal) * p3normal) - lightVector);
	cameraLocation = glm::vec3(arr[0], arr[1], arr[2]);
	V = glm::normalize(cameraLocation - p3);
	Is = light.highValue3 * light.specularColor * model.specularColor * pow(max((float)glm::dot(R, V), 0.0f), light.alpha);

	glm::vec3 p3Color = (Ia + Id + Is);
	PutPixel(p3.x, p3.y, p3Color, p3.z);


	for (int y = yMax; y >= yMin; y--) {
		for (int x = xMin; x <= xMax; x++) {
			glm::ivec2 point = glm::ivec2(x, y);

			if (pointInTriangle(point, p1, p2, p3)) {
				a1 = triangleArea(p1, p2, point);
				a2 = triangleArea(p2, p3, point);
				a3 = triangleArea(p1, p3, point);

				A = a1 + a2 + a3;
				z = ((a1 * p3.z) / A) + ((a2 * p1.z) / A) + ((a3 * p2.z) / A);

				glm::vec3 finalColor = ((a1 * p3Color) / A) + ((a2 * p1Color) / A) + ((a3 * p2Color) / A);
				//finalColor /= max(max(finalColor.x, finalColor.y), finalColor.z);
				PutPixel(x, y, finalColor, z);
			}
		}
	}
}

void Renderer::DrawTriangle(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, MeshModel& model, int faceIndex, Scene& scene) {
	if (model.trianglesOutlines) {
		DrawLine(p1, p2, glm::vec3(1, 1, 1));
		DrawLine(p2, p3, glm::vec3(1, 1, 1));
		DrawLine(p1, p3, glm::vec3(1, 1, 1));
	}


	float xMin = min(min(p1.x, p2.x), p3.x);
	float yMin = min(min(p1.y, p2.y), p3.y);

	float xMax = max(max(p1.x, p2.x), p3.x);
	float yMax = max(max(p1.y, p2.y), p3.y);

	if (model.trianglesBoundingBoxes) {
		glm::vec3 randomColor = glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
		DrawLine(glm::vec3(xMin, yMin, 100), glm::vec3(xMin, yMax, 100), randomColor);
		DrawLine(glm::vec3(xMax, yMin, 100), glm::vec3(xMax, yMax, 100), randomColor);
		DrawLine(glm::vec3(xMin, yMax, 100), glm::vec3(xMax, yMax, 100), randomColor);
		DrawLine(glm::vec3(xMax, yMin, 100), glm::vec3(xMin, yMin, 100), randomColor);
	}


	if (model.coloredTriangles) {
		for (int i = 0; i < scene.GetLightCount(); i++) {
			Light& light = scene.GetLight(i);

			// flat shading
			if (light.lightingType == 0) {
				flatShading(scene, model, light, xMin, xMax, yMin, yMax, p1, p2, p3, false, faceIndex);
			}
			// gouraud
			if (light.lightingType == 1) {
				gouraudShading(scene, model, light, xMin, xMax, yMin, yMax, p1, p2, p3, faceIndex);
			}
			// phong
			if (light.lightingType == 2) {
				flatShading(scene, model, light, xMin, xMax, yMin, yMax, p1, p2, p3, true, faceIndex);
			}
		}
	}
}

glm::vec3 color = glm::vec3(255, 255, 255);

void Renderer::Render(const Scene& scene, std::shared_ptr<MeshModel> cameraModel) {

	Scene scene1 = scene;

	// TODO: Replace this code with real scene rendering code
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	// draw circle
	// for assignment 1 circle of lines
	//DrawCircleOfLines(half_width, half_height);

	//// for bonus, ACTUAL circle
	int start_width = viewport_width / 2;
	int start_height =  viewport_height / 2;
	//DrawActualCircle(half_width, half_height);

	// drawing pumpkin
	start_width = 2 * viewport_width / 5;
	start_height =  viewport_height / 10;
	//DrawPumpkin(start_width, start_height);
	
	// scaling model for assignment 1 part 2

	scene1.GetActiveCamera().half_height = half_height;
	scene1.GetActiveCamera().half_width = half_width;
	glm::mat4x4 cameraTransform = scene1.GetActiveCamera().GetTransform();

	glm::vec2 point1, point2, point3;
	maxZ = -1.0f * FLT_MAX;
	minZ = FLT_MAX;

	for (int i = 0; i < scene.GetModelCount(); i++) {
		MeshModel& model = scene.GetModel(i);        
		this->maxZ = max(this->maxZ, model.maxZ);
		this->minZ = min(this->minZ, model.minZ);
	}

	for (int i = 0; i < scene.GetLightCount(); i++) {
		scene1.GetLight(i).GetTransform(cameraTransform);
	}


	for (int i = 0; i < scene.GetModelCount(); i++) {
		
		MeshModel& model = scene.GetModel(i);

		float maxX = -1.0f * FLT_MAX;
		float minX = FLT_MAX;

		float maxY = -1.0f * FLT_MAX;
		float minY = FLT_MAX;

		glm::mat4x4 trans = glm::mat4x4(1.0f);
		trans[3][0] = 0;
		trans[3][1] = 0;
		trans[3][2] = 1;

		std::vector<glm::vec3> newVertices = model.Draw(cameraTransform);

		for (int j = 0; j < model.GetFacesCount(); j++) {
			
			Face face = model.GetFace(j);
			// VERTICES
			int v1Index = face.GetVertexIndex(0) - 1;
			int v2Index = face.GetVertexIndex(1) - 1;
			int v3Index = face.GetVertexIndex(2) - 1;
		
			glm::vec3 v1 = newVertices.at(v1Index);
			glm::vec3 v2 = newVertices.at(v2Index);
			glm::vec3 v3 = newVertices.at(v3Index);

			v1.x = (v1.x + 1) * half_width;
			v1.y = (v1.y + 1) * half_height;

			v2.x = (v2.x + 1) * half_width;
			v2.y = (v2.y + 1) * half_height;

			v3.x = (v3.x + 1) * half_width;
			v3.y = (v3.y + 1) * half_height;

			minX = min(minX, v1.x);
			minX = min(minX, v2.x);
			minX = min(minX, v3.x);

			maxX = max(maxX, v1.x);
			maxX = max(maxX, v2.x);
			maxX = max(maxX, v3.x);

			minY = min(minY, v1.y);
			minY = min(minY, v2.y);
			minY = min(minY, v3.y);

			maxY = max(maxY, v1.y);
			maxY = max(maxY, v2.y);
			maxY = max(maxY, v3.y);



			DrawTriangle(v1, v2, v3, model, j, scene1);
		}

		if (model.drawAxis) {
			std::vector<glm::vec3> points = model.transformedAxis;

			points.at(0).x = (points.at(0).x + 1) * half_width;
			points.at(0).y = (points.at(0).y + 1) * half_height;
			points.at(1).x = (points.at(1).x + 1) * half_width;
			points.at(1).y = (points.at(1).y + 1) * half_height;

			points.at(2).x = (points.at(2).x + 1) * half_width;
			points.at(2).y = (points.at(2).y + 1) * half_height;

			points.at(3).x = (points.at(3).x + 1) * half_width;
			points.at(3).y = (points.at(3).y + 1) * half_height;

			points.at(4).x = (points.at(4).x + 1) * half_width;
			points.at(4).y = (points.at(4).y + 1) * half_height;

			points.at(5).x = (points.at(5).x + 1) * half_width;
			points.at(5).y = (points.at(5).y + 1) * half_height;

			//X axis
			DrawLine(points.at(0), points.at(1), glm::vec3(255, 0, 0));
			//Y axis
			DrawLine(points.at(2), points.at(3), glm::vec3(0, 255, 0));
			//Z axis
			DrawLine(points.at(4), points.at(5), glm::vec3(0, 0, 255));
		}
		if (model.drawBoundingBox) {
			std::vector<glm::vec3> points = model.transformedBoundingBox;

			points.at(0).x = (points.at(0).x + 1) * half_width;
			points.at(0).y = (points.at(0).y + 1) * half_height;

			points.at(1).x = (points.at(1).x + 1) * half_width;
			points.at(1).y = (points.at(1).y + 1) * half_height;

			points.at(2).x = (points.at(2).x + 1) * half_width;
			points.at(2).y = (points.at(2).y + 1) * half_height;

			points.at(3).x = (points.at(3).x + 1) * half_width;
			points.at(3).y = (points.at(3).y + 1) * half_height;

			points.at(4).x = (points.at(4).x + 1) * half_width;
			points.at(4).y = (points.at(4).y + 1) * half_height;

			points.at(5).x = (points.at(5).x + 1) * half_width;
			points.at(5).y = (points.at(5).y + 1) * half_height;

			points.at(6).x = (points.at(6).x + 1) * half_width;
			points.at(6).y = (points.at(6).y + 1) * half_height;

			points.at(7).x = (points.at(7).x + 1) * half_width;
			points.at(7).y = (points.at(7).y + 1) * half_height;

			DrawLine(points.at(0), points.at(1), glm::vec3(255, 0, 0));
			DrawLine(points.at(0), points.at(3), glm::vec3(255, 0, 0));
			DrawLine(points.at(0), points.at(4), glm::vec3(255, 0, 0));
			DrawLine(points.at(1), points.at(2), glm::vec3(255, 0, 0));
			DrawLine(points.at(1), points.at(5), glm::vec3(255, 0, 0));
			DrawLine(points.at(2), points.at(3), glm::vec3(255, 0, 0));
			DrawLine(points.at(2), points.at(6), glm::vec3(255, 0, 0));
			DrawLine(points.at(3), points.at(7), glm::vec3(255, 0, 0));
			DrawLine(points.at(4), points.at(5), glm::vec3(255, 0, 0));
			DrawLine(points.at(4), points.at(7), glm::vec3(255, 0, 0));
			DrawLine(points.at(5), points.at(6), glm::vec3(255, 0, 0));
			DrawLine(points.at(6), points.at(7), glm::vec3(255, 0, 0));
		}

		if (model.drawVertexNormals) {
			for each (std::vector<glm::vec3> pair in model.transformedVertexNormals) {
				pair.at(0).x = (pair.at(0).x + 1) * half_width;
				pair.at(0).y = (pair.at(0).y + 1) * half_height;

				pair.at(1).x = (pair.at(1).x + 1) * half_width;
				pair.at(1).y = (pair.at(1).y + 1) * half_height;

				DrawLine(pair.at(0), pair.at(1), glm::vec3(0, 0, 255));
			}
		}

		if (model.drawFaceNormals) {
			for each (std::vector<glm::vec3> pair in model.transformedFaceNormals) {
				pair.at(0).x = (pair.at(0).x + 1) * half_width;
				pair.at(0).y = (pair.at(0).y + 1) * half_height;

				pair.at(1).x = (pair.at(1).x + 1) * half_width;
				pair.at(1).y = (pair.at(1).y + 1) * half_height;

				DrawLine(pair.at(0), pair.at(1), glm::vec3(0, 255, 0));
			}
		}
		if (msaaFlag) {
			MSAA(minX, maxX, minY, maxY);
		}
	}

	if (scene.drawWorldAxis) {
		std::vector<glm::vec3> axis;
		// X axis
		glm::vec4 x1 = cameraTransform * glm::vec4(0, 0, 0, 1);
		glm::vec4 x2 = cameraTransform * glm::vec4(half_height / 2, 0, 0, 1);

		glm::vec3 x1Transformed = glm::vec3(x1 / x1.w);
		x1Transformed.x = (x1Transformed.x + 1) * half_width;
		x1Transformed.y = (x1Transformed.y + 1) * half_height;
		glm::vec3 x2Transformed = glm::vec3(x2 / x2.w);
		x2Transformed.x = (x2Transformed.x + 1) * half_width;
		x2Transformed.y = (x2Transformed.y + 1) * half_height;

		axis.push_back(x1Transformed);
		axis.push_back(x2Transformed);



		// Y axis
		glm::vec4 y1 = cameraTransform * glm::vec4(0, 0, 0, 1);
		glm::vec4 y2 = cameraTransform * glm::vec4(0, half_width / 2, 0, 1);
		glm::vec3 y1Transformed = glm::vec3(y1 / y1.w);
		y1Transformed.x = (y1Transformed.x + 1) * half_width;
		y1Transformed.y = (y1Transformed.y + 1) * half_height;
		glm::vec3 y2Transformed = glm::vec3(y2 / y2.w);
		y2Transformed.x = (y2Transformed.x + 1) * half_width;
		y2Transformed.y = (y2Transformed.y + 1) * half_height;

		axis.push_back(y1Transformed);
		axis.push_back(y2Transformed);

		// Z axis
		glm::vec4 z1 = cameraTransform * glm::vec4(0, 0, 0, 1);
		glm::vec4 z2 = cameraTransform * glm::vec4(0, 0, half_width / 2, 1);
		glm::vec3 z1Transformed = glm::vec3(z1 / z1.w);
		z1Transformed.x = (z1Transformed.x + 1) * half_width;
		z1Transformed.y = (z1Transformed.y + 1) * half_height;
		glm::vec3 z2Transformed = glm::vec3(z2 / z2.w);
		z2Transformed.x = (z2Transformed.x + 1) * half_width;
		z2Transformed.y = (z2Transformed.y + 1) * half_height;

		axis.push_back(z1Transformed);
		axis.push_back(z2Transformed);


		//X axis
		DrawLine(axis.at(0), axis.at(1), glm::vec3(255, 0, 0));
		//Y axis
		DrawLine(axis.at(2), axis.at(3), glm::vec3(0, 255, 0));
		//Z axis
		DrawLine(axis.at(4), axis.at(5), glm::vec3(0, 0, 255));
	}

	int current_camera = scene.GetActiveCameraIndex();
	int camera_num = scene.GetCameraCount();
	
	if (camera_num > 1) {
		for (int i = 0; i < camera_num; i++) {
			if (i == current_camera)
				continue;

			(*cameraModel).localTranslateVector[0] = scene1.GetCamera(i).localTranslateArray[0] + scene1.GetCamera(i).eye[0];
			(*cameraModel).localTranslateVector[1] = scene1.GetCamera(i).localTranslateArray[1] + scene1.GetCamera(i).eye[1];
			(*cameraModel).localTranslateVector[2] = scene1.GetCamera(i).localTranslateArray[2] + scene1.GetCamera(i).eye[2];

			(*cameraModel).localRotateVector[0] = scene1.GetCamera(i).localRotateArray[0];
			(*cameraModel).localRotateVector[1] = scene1.GetCamera(i).localRotateArray[1];
			(*cameraModel).localRotateVector[2] = scene1.GetCamera(i).localRotateArray[2];

			(*cameraModel).worldTranslateVector[0] = scene1.GetCamera(i).worldTranslateArray[0];
			(*cameraModel).worldTranslateVector[1] = scene1.GetCamera(i).worldTranslateArray[1];
			(*cameraModel).worldTranslateVector[2] = scene1.GetCamera(i).worldTranslateArray[2];

			(*cameraModel).worldRotateVector[0] = scene1.GetCamera(i).worldRotateArray[0];
			(*cameraModel).worldRotateVector[1] = scene1.GetCamera(i).worldRotateArray[1];
			(*cameraModel).worldRotateVector[2] = scene1.GetCamera(i).worldRotateArray[2];

			std::vector<glm::vec3> newVertices = (*cameraModel).Draw(cameraTransform);

			for (int j = 0; j < (*cameraModel).GetFacesCount(); j++) {
				Face face = (*cameraModel).GetFace(j);

				// VERTICES
				int v1Index = face.GetVertexIndex(0) - 1;
				int v2Index = face.GetVertexIndex(1) - 1;
				int v3Index = face.GetVertexIndex(2) - 1;

				glm::vec3 v1 = newVertices.at(v1Index);
				glm::vec3 v2 = newVertices.at(v2Index);
				glm::vec3 v3 = newVertices.at(v3Index);

				v1.x = (v1.x + 1) * half_width;
				v1.y = (v1.y + 1) * half_height;

				v2.x = (v2.x + 1) * half_width;
				v2.y = (v2.y + 1) * half_height;

				v3.x = (v3.x + 1) * half_width;
				v3.y = (v3.y + 1) * half_height;

				DrawLine(v1, v2, color);
				DrawLine(v2, v3, color);
				DrawLine(v1, v3, color);
			}
		}

	}

}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}

void Renderer::setViewportWidth(const int width) {
	this->viewport_width = width;
}
void Renderer::setViewportHeight(const int height) {
	this->viewport_height = height;
}