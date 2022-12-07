#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

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

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;
	//std::cout << "i is " << i << ", j is " << j << std::endl;
	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}

void Renderer::PlotLineHigh(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	int xi = 1;

	if (dx < 0) {
		xi = -1;
		dx = ((-1) * dx);
	}

	float d = (2 * dx) - dy;
	float x = p1.x;
	for (int y = p1.y; y < p2.y; y++) {
		PutPixel(x, y, color);
		if (d > 0) {
			x += xi;
			d += (2 * (dx - dy));
		}
		else {
			d += (2 * dx);
		}
	}
}


void Renderer::PlotLineLow(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	int yi = 1;

	if (dy < 0) {
		yi = -1;
		dy = ((-1) * dy);
	}

	float d = (2 * dy) - dx;
	float y = p1.y;
	for (int x = p1.x; x < p2.x; x++) {
		PutPixel(x, y, color);
		if (d > 0) {
			y += yi;
			d += (2 * (dy - dx));
		}
		else {
			d += (2 * dy);
		}
	}
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
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

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::DrawCircleOfLines(int start_width, int start_height) {
	const double pi = 3.14159265358979323846;
	for (int i = 1; i <= 80; i++) {
		Renderer::DrawLine(glm::ivec2(start_width, start_height), glm::ivec2(start_width + 200*(sin((2*pi*i)/45)), start_height + 200 * (cos((2 * pi * i) / 45))), glm::vec3(1, 1, 1));
	}
}

void Renderer::DrawActualCircle(int start_width, int start_height) {
	const double pi = 3.14159265358979323846;
	int length = 200;
	glm::ivec2 p1 = glm::ivec2(start_width, start_height);
	glm::ivec2 p2 = glm::ivec2(start_width + length * (sin((2 * pi * 0) / 360)), start_height + length * (cos((2 * pi * 0) / 360)));
	for (double i = 1; i <= 360; i+=0.5) {
		p1 = p2;
		p2 = glm::ivec2(start_width + length * (sin((2 * pi * i) / 360)), start_height + length * (cos((2 * pi * i) / 360)));
		Renderer::DrawLine(p1, p2, glm::vec3(1, 0, 0));
	}
}

void Renderer::DrawPumpkin(int start_width, int start_height) {
	// face
	glm::ivec2 p1 = glm::ivec2(start_width, start_height);
	glm::ivec2 p2 = glm::ivec2(start_width + 200, start_height);
	glm::vec3 orange = glm::vec3(0.99, 0.41, 0);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 50, p1.y + 50);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 50, p1.y + 75);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y + 150);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 50, p1.y + 75);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 50, p1.y + 50);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 200, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 50, p1.y - 50);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 50, p1.y - 75);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 150);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 50, p1.y - 75);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 50, p1.y - 50);
	Renderer::DrawLine(p1, p2, orange);

	// mouth
	p1 = glm::ivec2(start_width - 40, start_height + 170);
	p2 = glm::ivec2(p1.x + 80, p1.y - 60);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 60, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 60, p1.y - 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 80, p1.y + 60);
	Renderer::DrawLine(p1, p2, orange);

	p1 = glm::ivec2(start_width - 40, start_height + 170);
	p2 = glm::ivec2(p1.x + 80, p1.y - 120);
	Renderer::DrawLine(p1, p2, orange);

	for (int i = 0; i < 6; i++) {
		p1 = p2;
		p2 = glm::ivec2(p1.x + 10, p1.y + 10);
		Renderer::DrawLine(p1, p2, orange);
		p1 = p2;
		p2 = glm::ivec2(p1.x + 10, p1.y - 10);
		Renderer::DrawLine(p1, p2, orange);
	}

	p1 = p2;
	p2 = glm::ivec2(p1.x + 80, p1.y + 120);
	Renderer::DrawLine(p1, p2, orange);

	// eyes
	p1 = glm::ivec2(start_width + 10, start_height + 300);
	p2 = glm::ivec2(p1.x + 30, p1.y + 60);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 30, p1.y - 60);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 60, p1.y);
	Renderer::DrawLine(p1, p2, orange);

	p1 = glm::ivec2(start_width + 190, start_height + 300);
	p2 = glm::ivec2(p1.x - 30, p1.y + 60);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 30, p1.y - 60);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 60, p1.y);
	Renderer::DrawLine(p1, p2, orange);

	// nose
	p1 = glm::ivec2(start_width + 80, start_height + 220);
	p2 = glm::ivec2(p1.x + 40, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 20, p1.y - 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 20, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);

	//hat 

	glm::vec3 green = glm::vec3(0.1, 0.43, 0);
	p1 = glm::ivec2(start_width + 60, start_height + 400);
	p2 = glm::ivec2(p1.x + 10, p1.y + 15);
	Renderer::DrawLine(p1, p2, green);

	p1 = p2;
	p2 = glm::ivec2(p1.x + 30, p1.y + 25);
	Renderer::DrawLine(p1, p2, green);

	p1 = p2;
	p2 = glm::ivec2(p1.x + 60, p1.y + 30);
	Renderer::DrawLine(p1, p2, green);

	p1 = p2;
	glm::ivec2 p3 = p2;
	p2 = glm::ivec2(p1.x + 10, p1.y - 5);
	Renderer::DrawLine(p1, p2, green);

	glm::vec3 brown = glm::vec3(0.33, 0.21, 0.125);

	p1 = p2;
	p2 = glm::ivec2(p1.x + 16, p1.y - 7);
	Renderer::DrawLine(p3, p2, brown);

	p2 = glm::ivec2(p1.x + 20, p1.y - 5);
	Renderer::DrawLine(p3, p2, brown);
	p2 = glm::ivec2(p1.x + 29, p1.y - 2);
	Renderer::DrawLine(p3, p2, brown);
	p2 = glm::ivec2(p1.x + 32, p1.y - 0.5);
	Renderer::DrawLine(p3, p2, brown);
	p2 = glm::ivec2(p1.x + 35, p1.y);
	Renderer::DrawLine(p3, p2, brown);

	p1 = glm::ivec2(start_width + 110, start_height + 400);
	p2 = glm::ivec2(p1.x + 10, p1.y + 15);
	Renderer::DrawLine(p1, p2, green);

	p1 = p2;
	p2 = glm::ivec2(p1.x + 30, p1.y + 25);
	Renderer::DrawLine(p1, p2, green);

	p1 = p2;
	p2 = glm::ivec2(p1.x + 60, p1.y + 30);
	Renderer::DrawLine(p1, p2, green);
	Renderer::DrawLine(p3, p2, green);


	// happy halloweeen
		//H

	int y = start_height + 500;
	p1 = glm::ivec2(start_width - 130, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = glm::ivec2(p1.x, p1.y + 20);
	p2 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = glm::ivec2(p2.x, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);

	//A

	p1 = glm::ivec2(p2.x + 10, y);
	p2 = glm::ivec2(p1.x + 10, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 10, p1.y - 40);
	Renderer::DrawLine(p1, p2, orange);
	p3 = p2;
	p1 = p2;
	p2 = glm::ivec2(p1.x - 5, p1.y + 20);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 8, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p2 = p3;

	//P

	p1 = glm::ivec2(p2.x + 10, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 15, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 5, p1.y - 2);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 2, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 5, p1.y - 1);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 10, p1.y);
	Renderer::DrawLine(p1, p2, orange);

	//P

	p1 = glm::ivec2(p2.x + 30, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 15, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 5, p1.y - 2);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 2, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 2, p1.y - 4);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 5, p1.y - 1);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 10, p1.y);
	Renderer::DrawLine(p1, p2, orange);

	//Y

	p1 = glm::ivec2(p2.x + 30, y + 40);
	p2 = glm::ivec2(p1.x + 10, p1.y - 20);
	Renderer::DrawLine(p1, p2, orange);
	p3 = p2;
	p1 = p2;
	p2 = p2 = glm::ivec2(p1.x, p1.y - 20);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p3;
	p2 = glm::ivec2(p1.x + 10, p1.y + 20);
	Renderer::DrawLine(p1, p2, orange);

	//H

	y = start_height + 500;
	p1 = glm::ivec2(p2.x + 30, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = glm::ivec2(p1.x, p1.y + 20);
	p2 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = glm::ivec2(p2.x, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);

	//A

	p1 = glm::ivec2(p2.x + 10, y);
	p2 = glm::ivec2(p1.x + 10, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 10, p1.y - 40);
	Renderer::DrawLine(p1, p2, orange);
	p3 = p2;
	p1 = p2;
	p2 = glm::ivec2(p1.x - 5, p1.y + 20);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 8, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p2 = p3;

	//L

	p1 = glm::ivec2(p2.x + 10, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p2 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p2, orange);

	//L

	p1 = glm::ivec2(p2.x + 10, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p2 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p2, orange);

	//O
	p1 = glm::ivec2(p2.x + 10, y);
	p3 = p1;
	p2 = glm::ivec2(p1.x - 2, p1.y + 3);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y + 30);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 1, p1.y + 1);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 5, p1.y + 3);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 7, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 7, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 5, p1.y - 3);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 30);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 6, p1.y - 1);
	Renderer::DrawLine(p1, p2, orange);
	Renderer::DrawLine(p3, p2, orange);

	//O
	p1 = glm::ivec2(p2.x + 20, y);
	p3 = p1;
	p2 = glm::ivec2(p1.x - 2, p1.y + 3);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y + 30);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 1, p1.y + 1);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 5, p1.y + 3);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 7, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 7, p1.y);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 5, p1.y - 3);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y - 30);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 4, p1.y - 2);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x - 6, p1.y - 1);
	Renderer::DrawLine(p1, p2, orange);
	Renderer::DrawLine(p3, p2, orange);
	//W
	p1 = glm::ivec2(p2.x + 20, y + 40);
	p2 = glm::ivec2(p1.x + 10, p1.y - 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 10, p1.y + 20);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 10, p1.y - 20);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 10, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);

	//E

	p1 = glm::ivec2(p2.x + 10, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p3 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p3, orange);
	p3 = glm::ivec2(p2.x + 20, p2.y);
	Renderer::DrawLine(p2, p3, orange);
	p1 = glm::ivec2(p1.x, p1.y + 20);
	p3 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p3, orange);

	//E
	p1 = glm::ivec2(p2.x + 30, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p3 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p3, orange);
	p3 = glm::ivec2(p2.x + 20, p2.y);
	Renderer::DrawLine(p2, p3, orange);
	p1 = glm::ivec2(p1.x, p1.y + 20);
	p3 = glm::ivec2(p1.x + 20, p1.y);
	Renderer::DrawLine(p1, p3, orange);

	//N 
	p1 = glm::ivec2(p2.x + 30, y);
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x + 20, p1.y - 40);
	Renderer::DrawLine(p1, p2, orange);
	p1 = p2;
	p2 = glm::ivec2(p1.x, p1.y + 40);
	Renderer::DrawLine(p1, p2, orange);
	//lama'ss comment 
	
}

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

	for (int i = 0; i < scene.GetModelCount(); i++) {
		
		MeshModel& model = scene.GetModel(i);

		glm::mat4x4 trans = glm::mat4x4(1.0f);
		trans[3][0] = 0;
		trans[3][1] = 0;
		trans[3][2] = 1;

		//if (model.cameraIndex > -1)
		//    model.objectTransform = scene1.GetCamera(model.cameraIndex).drawTransformation;

		std::vector<std::vector<glm::vec2>> pairs = model.Draw(cameraTransform);

		for each (std::vector<glm::vec2> pair in pairs) {
			pair.at(0).x = (pair.at(0).x + 1) * half_width;
			pair.at(0).y = (pair.at(0).y + 1) * half_height;

			pair.at(1).x = (pair.at(1).x + 1) * half_width;
			pair.at(1).y = (pair.at(1).y + 1) * half_height;

			DrawLine(pair.at(0), pair.at(1), glm::vec3(255,255,255));
		}

		if (model.drawAxis) {
			std::vector<glm::vec2> points = model.transformedAxis;

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
			std::vector<glm::vec2> points = model.transformedBoundingBox;

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
			for each (std::vector<glm::vec2> pair in model.transformedVertexNormals) {
				pair.at(0).x = (pair.at(0).x + 1) * half_width;
				pair.at(0).y = (pair.at(0).y + 1) * half_height;

				pair.at(1).x = (pair.at(1).x + 1) * half_width;
				pair.at(1).y = (pair.at(1).y + 1) * half_height;

				DrawLine(pair.at(0), pair.at(1), glm::vec3(0, 0, 255));
			}
		}

		if (model.drawFaceNormals) {
			for each (std::vector<glm::vec2> pair in model.transformedFaceNormals) {
				pair.at(0).x = (pair.at(0).x + 1) * half_width;
				pair.at(0).y = (pair.at(0).y + 1) * half_height;

				pair.at(1).x = (pair.at(1).x + 1) * half_width;
				pair.at(1).y = (pair.at(1).y + 1) * half_height;

				DrawLine(pair.at(0), pair.at(1), glm::vec3(0, 255, 0));
			}
		}


	}

	if (scene.drawWorldAxis) {
		std::vector<glm::vec2> axis;
		// X axis
		glm::vec4 x1 = cameraTransform * glm::vec4(0, 0, 0, 1);
		glm::vec4 x2 = cameraTransform * glm::vec4(half_height / 2, 0, 0, 1);

		glm::vec2 x1Transformed = glm::vec2(x1 / x1.w);
		x1Transformed.x = (x1Transformed.x + 1) * half_width;
		x1Transformed.y = (x1Transformed.y + 1) * half_height;
		glm::vec2 x2Transformed = glm::vec2(x2 / x2.w);
		x2Transformed.x = (x2Transformed.x + 1) * half_width;
		x2Transformed.y = (x2Transformed.y + 1) * half_height;

		axis.push_back(x1Transformed);
		axis.push_back(x2Transformed);



		// Y axis
		glm::vec4 y1 = cameraTransform * glm::vec4(0, 0, 0, 1);
		glm::vec4 y2 = cameraTransform * glm::vec4(0, half_width / 2, 0, 1);
		glm::vec2 y1Transformed = glm::vec2(y1 / y1.w);
		y1Transformed.x = (y1Transformed.x + 1) * half_width;
		y1Transformed.y = (y1Transformed.y + 1) * half_height;
		glm::vec2 y2Transformed = glm::vec2(y2 / y2.w);
		y2Transformed.x = (y2Transformed.x + 1) * half_width;
		y2Transformed.y = (y2Transformed.y + 1) * half_height;

		axis.push_back(y1Transformed);
		axis.push_back(y2Transformed);

		// Z axis
		glm::vec4 z1 = cameraTransform * glm::vec4(0, 0, 0, 1);
		glm::vec4 z2 = cameraTransform * glm::vec4(0, 0, half_width / 2, 1);
		glm::vec2 z1Transformed = glm::vec2(z1 / z1.w);
		z1Transformed.x = (z1Transformed.x + 1) * half_width;
		z1Transformed.y = (z1Transformed.y + 1) * half_height;
		glm::vec2 z2Transformed = glm::vec2(z2 / z2.w);
		z2Transformed.x = (z2Transformed.x + 1) * half_width;
		z2Transformed.y = (z2Transformed.y + 1) * half_height;

		axis.push_back(z1Transformed);
		axis.push_back(z2Transformed);

		//axis.at(0).x += half_width;
		//axis.at(0).y += half_height;
		//axis.at(1).x += half_width;
		//axis.at(1).y += half_height;
		//axis.at(2).x += half_width;
		//axis.at(2).y += half_height;
		//axis.at(3).x += half_width;
		//axis.at(3).y += half_height;
		//axis.at(4).x += half_width;
		//axis.at(4).y += half_height;
		//axis.at(5).x += half_width;
		//axis.at(5).y += half_height;

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

			std::vector<std::vector<glm::vec2>> pairs = (*cameraModel).Draw(cameraTransform);

			for each (std::vector<glm::vec2> pair in pairs) {
				pair.at(0).x = (pair.at(0).x + 1) * half_width;
				pair.at(0).y = (pair.at(0).y + 1) * half_height;

				pair.at(1).x = (pair.at(1).x + 1) * half_width;
				pair.at(1).y = (pair.at(1).y + 1) * half_height;

				DrawLine(pair.at(0), pair.at(1), glm::vec3(255, 255, 255));;
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