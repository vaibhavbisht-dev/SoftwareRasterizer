# Software Rasterizer

> This is a Practical Implementation of a **Software Rasterizer** created by me in C++.

## What is a Software Rasterizer ?
A **Software Rasterizer** is a graphics engine that renders 3D objects into 2D images using CPU instead of a dedicated Graphics Hardware (**GPU**).

## Project Requirements
**This Project uses these following API and Languages:** 
- **C++**
- **SDL3 (For Rendering)**

## Creating a Frame Buffer
### What is a Frame Buffer?
Your monitor displays a picture Bu Lighting up a grid of pixels. Something has to decide what that each pixel should be. That something is a **FrameBuffer**. It is a block of memory where each slot corresponds to one pixel on the memory.<br>
Think of it as a flat 1D array representing a 2D grid.<br>
`std::vector<uint32_t> m_buffer;`
>Pixel at (x,y) lives at index: `y * width + x`<br>

But Why `y * width + x` and not `x * Height + y`?<br>

Let's Assume pixel in a screen is a grid: <br>

| | x=0 | x=1 | x=2 | x=3 |
| :--- | :---: | :---: | :---: | :---: |
| **y=0** | [A] | [B] | [C] | [D] |
| **y=1** | [E] | [F] | [G] | [H] |
| **y=2** | [I] | [J] | [K] | [L] |
| **y=3** | [M] | [N] | [O] | [P] |


And Memory is stored in a single Continuous line: <br>

And memory is stored in a continuous line.

| Memory Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | 
| **Stored Pixel** | [A] | [B] | [C] | [D] | [E] | [F] | [G] | [H] | [I]

So to find [F] pixel in memory: <br>
x = 1 <br>
y = 1 <br>
Width = 4

> Formula: `y * Width + x`. <br>

1 * 4 + 1 = 5<br>
That's How we Get the memory block location.

## Pixel Format
>As I am using SDL For Display. I'll use SDL_PIXELFORMAT_ARGB8888.

`SDL_PIXELFORMAT_ARGB8888` and the framebuffer will be `uint32_t` a 32 bit unsigned integer. Which means 1 pixel will take 32 bits = 4 bytes.<br>

The Window Resolution is `800 x 600` So that Means a total of `480000 pixels`.<br>
1 pixel = 4 bytes<br>
480000 pixels = 480000 x 4<br>
480000 pixels = 1920000 bytes = 1.92 MB<br>

**The format ARGB8888 means:**
- A  → Alpha → 8 bytes
- R  → Red → 8 bytes
- G  → Green → 8 bytes
- B  → Blue → 8 bytes

So, `0xFFFF0000` is fully opaque, full red with no green or blue.<br>

## Bresenham's Line Algorithm
>The Bresenham's line Algorithm is a highly efficient rasterization method used in computer graphics to approximate a mathematical straight line on a discreate pixel grid.

The algorithm is based on the equation of the line: y = mx + c. where., $$m = \frac{\Delta y}{\Delta x}$$
is the Slope.<br>

## Functions 
### Clear Screen
The most Basic Operation in any renderer is clearing the framebuffer before drawing anything.<br>
>I'll use `std::fill` as it is the most clear and idiomatic way.

```
void clearbuffer()
{
	std::fill(m_buffer.begin(), m_buffer.end(), 0xFF000000);
}
```
### Set Pixel
It is the most Basic function of the framebuffer as to draw anything You will have to Set a pixel to a given color.<br>
```
void setPixel(int x, int y, uint32_t color) {
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
	{
		m_buffer[y * m_width + x] = color;
	}
}
```
### Draw Line
Draw Line is one of the most important function of the framebuffer as the first step to draw anything is to create a line.
```
void DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if(x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	// as this is same for every line, we can calculate it once
	int dx = x1 - x0;
	int dy = y1 - y0;

  	// We use error term to decide when to step in y direction. The error term is the distance from the ideal line. When it exceeds a threshold (in this case, dx), we step in y direction and reset the error.
	int derror = std::abs(dy) * 2;
	int error = 0;


	int y = y0;

	for (int x = x0; x <= x1; x++) {
		if (steep) {
			setPixel(y, x, color); // if transposed, de−transpose 
		}
		else {
			setPixel(x, y, color);
		}

		// Update the error term and step in y direction if necessary
		error += derror;
		if (error > dx) {
			y += (y1 > y0 ? 1 : -1);
			error -= dx *2;
		}
	}
}
```

