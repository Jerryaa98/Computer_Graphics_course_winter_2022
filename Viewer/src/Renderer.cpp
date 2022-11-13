#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"
#include <iostream>

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

void Renderer::Render(const Scene& scene)
{
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
	DrawPumpkin(start_width, start_height);
	
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}