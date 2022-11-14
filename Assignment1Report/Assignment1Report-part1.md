# Assignment 1 Report - Bresenham's algorithm for line drawing

using the provided wiki link as a refrence we also used 2 helping functions to implement the algorithm.

the algorithm's code :-

```  sh
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
```

in order to test it's correctness we drew a circle of lines, then drew an actual circle as a circumference around them in a different color as shown in the image below:-

![circle of lines in an actual circle](assignment1/pics/circle%20of%20lines%20in%20an%20actual%20circle.jpg)

code for circles :-

``` sh
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
```

furthermore, we thought it would be cool to draw a pumpkin since it is halloween!

![halloween_pumpkin](assignment1/pics/halloween_pumpkin.jpg)

